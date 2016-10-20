#include "tracing.h"

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include <atomic>
#include <csignal>

// local data
namespace {

struct Event {
    char            type;
    char            ph;
    char            _padding[2];
    int32_t         id; // event id (only used to track async event pairs)
    const char*     name;
    const char*     cat;
    double          ts; // timestamp in seconds
    std::thread::id tid;
    double          dur; // duration in seconds (only used in duration events)
    const char*     arg_name;
    union {
        const char* arg_str;
        int32_t     arg_int;
    };
};

thread_local std::thread::id curr_thread_id = std::this_thread::get_id();

class TracingBuffer final
{
public:
    TracingBuffer(const char *output_filename, unsigned int max_num_events=1000000) 
    : event_buffer_(new Event[max_num_events]), max_num_events_(max_num_events), count_(0)
    {
        memset(event_buffer_,0,max_num_events_*sizeof(Event));
        ofile_.open(output_filename);
        ofile_ << "{\"traceEvents\":[\n";
        base_time_ = std::chrono::high_resolution_clock::now();
    }

    TracingBuffer(const TracingBuffer&) = delete;

    ~TracingBuffer()
    {
        flush();
        ofile_ << "{}\n]}\n";
        ofile_.close();
        delete [] event_buffer_;
        event_buffer_ = nullptr;
        // free temp strings
        for (auto &c: temp_strings_)
            free(c);
    }

    /// create an event with no arguments
    /// @return the index in the event buffer
    Event* create_event(double curr_time, const char *category, const char *name, char type, char ph, int32_t id=0, double dur=0.0)
    {
        int idx = count_.fetch_add(1,std::memory_order_relaxed); // atomic increment to prevent race conditions
        Event *ev = &event_buffer_[idx%max_num_events_]; // use event_buffer_ as a circular buffer
            ev->type = type;
            ev->ph = ph;
            ev->name = name;
            ev->cat = category;
            ev->ts = curr_time;
            ev->tid = curr_thread_id;
            ev->id = id;
            ev->dur = dur;
        return ev;
    }

    /// create an event with an int argument
    Event* create_event(double curr_time, const char *category, const char *name, char type, char ph, int32_t id, double dur, const char *arg_name, int arg_int)
    {
        auto ev  = create_event(curr_time, category, name, type, ph, id, dur);
        ev->arg_name = arg_name;
        ev->arg_int = arg_int;
        return ev;
    }

    /// create an event with a string argument
    Event* create_event(double curr_time, const char *category, const char *name, char type, char ph, int32_t id, double dur, const char *arg_name, const char *arg_str, bool copy_arg=false)
    {
        auto ev  = create_event(curr_time, category, name, type, ph, id, dur);
        ev->arg_name = arg_name;
        ev->arg_str = arg_str;
        // if the argument string is a temp string, we must copy and free the allocated memory at exit
        if (copy_arg)
        {
            auto newstr = strdup(arg_str);
            temp_strings_.push_back(newstr);
            ev->arg_str = newstr;
        }
        return ev;
    }

    Event* create_event(const char *category, const char *name, char type, char ph, int32_t id=0, double dur=0.0)
    {
        auto curr_time = get_time();
        return create_event(curr_time, category, name, type, ph, id, dur);
    }

    Event* create_event(const char *category, const char *name, char type, char ph, int32_t id, double dur, const char *arg_name, int arg_int)
    {
        auto curr_time = get_time();
        return create_event(curr_time, category, name, type, ph, id, dur, arg_name, arg_int);
    }

    Event* create_event(const char *category, const char *name, char type, char ph, int32_t id, double dur, const char *arg_name, const char *arg_str, bool copy_arg=false)
    {
        auto curr_time = get_time();
        return create_event(curr_time, category, name, type, ph, id, dur, arg_name, arg_str, copy_arg);
    }

    void flush()
    {
        std::stringstream ss;
        for (auto i=0; i<count_; i++)
        {
            auto &ev = event_buffer_[i];
            ss << "{" << "\"cat\":\"" << ev.cat << "\"" << ",\"pid\":\"" << 0 << "\"" << ",\"tid\":\"" << ev.tid << "\""
               << ",\"ts\":" << (uint64_t)(ev.ts*1000000) << ",\"ph\":\"" << ev.ph << "\""
               << ",\"name\":\"" << ev.name << "\"";
            if (ev.type & TRC_TYPE_DURATION)
                ss << ",\"dur\":\"" << (uint64_t)(ev.dur*1000000) << "\"";
            if (ev.type & TRC_TYPE_ASYNC)
                ss << ",\"id\":\"" << ev.id << "\"";
            if (ev.type & TRC_ARG_INT)
                ss << ",\"args\":{" << "\"" << ev.arg_name << "\":" << ev.arg_int << "}";
            else if (ev.type & TRC_ARG_STR)
                ss << ",\"args\":{" << "\"" << ev.arg_name << "\":\"" << ev.arg_str << "\"}";
            ss << "},\n";
        }
                
        ofile_ << ss.str();
        count_ = 0;
    }

    double get_time()
    {
        auto curr_t = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(curr_t-base_time_).count();
    }

protected:
    Event                   *event_buffer_  {nullptr};
    uint32_t                max_num_events_ {1000000};
    std::atomic<int>        count_          {0}; // atomic variable to ensure that each thread is working on his own instance of an event
    std::chrono::time_point<std::chrono::high_resolution_clock> base_time_;
    std::ofstream           ofile_;
    std::vector<char*>      temp_strings_;

};


// global variable for the tracing class
TracingBuffer *g_tracing {nullptr};

}


// initialize the tracing system, specifying the output file name and the maximum number of events that will be stored in the internal buffer
void trc_init(const char *output_file, unsigned int event_buffer_size)
{
    g_tracing = new TracingBuffer(output_file, event_buffer_size);
}

// shuts down tracing, flushing the recorded data to disk
void trc_shutdown()
{
    delete g_tracing;
    g_tracing = nullptr;
}

// get current time in seconds
double trc_get_time()
{
    return g_tracing->get_time();
}

// internal functions for creating events: should be used through the macros
void create_event(const char *category, const char *name, char type, char ph, int id, double dur)
{
    g_tracing->create_event(category, name, type, ph, id, dur);

}

void create_event_arg_int(const char *category, const char *name, char type, char ph, int id, double dur, const char *arg_name, int arg_int)
{
    g_tracing->create_event(category, name, type | TRC_ARG_INT, ph, id, dur, arg_name, arg_int);
}
void create_event_arg_str(const char *category, const char *name, char type, char ph, int id, double dur, const char *arg_name, const char *arg_str, bool copy_arg)
{
    g_tracing->create_event(category, name, type | TRC_ARG_STR, ph, id, dur, arg_name, arg_str, copy_arg);
}

void create_explicit_time_event(double timestamp, const char *category, const char *name, char type, char ph, int id, double dur)
{
    g_tracing->create_event(timestamp, category, name, type, ph, id, dur);
}

void create_explicit_time_event_arg_int(double timestamp, const char *category, const char *name, char type, char ph, int id, double dur, const char *arg_name, int arg_int)
{
    g_tracing->create_event(timestamp, category, name, type | TRC_ARG_INT, ph, id, dur, arg_name, arg_int);
}

void create_explicit_time_event_arg_str(double timestamp, const char *category, const char *name, char type, char ph, int id, double dur, const char *arg_name, const char *arg_str, bool copy_arg)
{
    g_tracing->create_event(timestamp, category, name, type | TRC_ARG_STR, ph, id, dur, arg_name, arg_str, copy_arg);
}

//
// #else  // MTR_ENABLED
//
// void mtr_register_sigint_handler()                                                            { return; }
// void mtr_init(const char *json_file, unsigned int max_num_events_)                            { return; }
// void mtr_shutdown()                                                                           { return; }
// void mtr_flush()                                                                              { return; }
// void internal_mtr_raw_event(const char *category, const char *name, char ph, void *id)        { return; }
// void internal_mtr_raw_event_arg(const char *category, const char *name, char ph, void *id, 
//                                 mtr_arg_type arg_type, const char *arg_name, void *arg_value) { return; }
//
// #endif // MTR_ENABLED 
