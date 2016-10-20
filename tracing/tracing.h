// tracing functions/macros that will generate a trace file compatible with the chrome://tracing format
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// bitmask for specifying the event and argument combinations
#define TRC_TYPE_BASE     (0x01 << 0)
#define TRC_TYPE_DURATION (0x01 << 1)
#define TRC_TYPE_ASYNC    (0x01 << 2)
#define TRC_ARG_INT       (0x01 << 3)
#define TRC_ARG_STR       (0x01 << 4)

// initialize the tracing system, specifying the output file name and the maximum number of events that will be stored in the internal buffer
void trc_init(const char *output_file, unsigned int event_buffer_size=1000000);

// shuts down tracing, flushing the recorded data to disk
void trc_shutdown();

// get current time in seconds
double trc_get_time();

// internal functions for creating events: should be used through the macros
void create_event        (const char *category, const char *name, char type, char ph, int id, double dur);
void create_event_arg_int(const char *category, const char *name, char type, char ph, int id, double dur, const char *arg_name, int arg_int);
void create_event_arg_str(const char *category, const char *name, char type, char ph, int id, double dur, const char *arg_name, const char *arg_str, bool copy_arg);
void create_explicit_time_event        (double timestamp, const char *category, const char *name, char type, char ph, int id, double dur);
void create_explicit_time_event_arg_int(double timestamp, const char *category, const char *name, char type, char ph, int id, double dur, const char *arg_name, int arg_int);
void create_explicit_time_event_arg_str(double timestamp, const char *category, const char *name, char type, char ph, int id, double dur, const char *arg_name, const char *arg_str, bool copy_arg);

// Sync events. Must begin/end in the same thread
#define TRC_BEGIN(c, n) create_event(c, n, TRC_TYPE_BASE, 'B', 0, 0)
#define TRC_END(  c, n) create_event(c, n, TRC_TYPE_BASE, 'E', 0, 0)
#define TRC_SCOPE(c, n) TRCGuard _trc__guard__(c,n) // this can only be used in C++
// same as above, but with metadata added:
// _I for int, _C is for a const string, _S is for a non-const string (will be copied in a temp object: slow but necessary)
#define TRC_BEGIN_I(c, n, arg_name, arg_int) create_event_arg_int(c, n, TRC_TYPE_BASE | TRC_ARG_INT, 'B', 0, 0, arg_name, arg_int)
#define TRC_BEGIN_C(c, n, arg_name, arg_str) create_event_arg_str(c, n, TRC_TYPE_BASE | TRC_ARG_STR, 'B', 0, 0, arg_name, arg_str, false)
#define TRC_BEGIN_S(c, n, arg_name, arg_str) create_event_arg_str(c, n, TRC_TYPE_BASE | TRC_ARG_STR, 'B', 0, 0, arg_name, arg_str, true)

// Async events. Can span threads. ID identifies the events that will be connected
#define TRC_ASYNC_START( c, n, id)       create_event(c, n, TRC_TYPE_ASYNC, 'b', id, 0)
#define TRC_ASYNC_STEP(  c, n, id, step) create_event_arg_str(c, n, TRC_TYPE_ASYNC | TRC_ARG_STR, 'n', id, 0, "step", step, false)
#define TRC_ASYNC_FINISH(c, n, id)       create_event(c, n, TRC_TYPE_ASYNC, 'e', id, 0)

// Flow events (similar to async, with a different visualization)
#define TRC_FLOW_START( c, n, id)       create_event(c, n, TRC_TYPE_ASYNC, 's', id, 0)
#define TRC_FLOW_STEP(  c, n, id, step) create_event_arg_str(c, n, TRC_TYPE_ASYNC, 't', id, 0, "step", step, false)
#define TRC_FLOW_FINISH(c, n, id)       create_event(c, n, TRC_TYPE_ASYNC, 'f', id, 0)


// Instant events
#define TRC_INSTANT(c, n)                      create_event(c, n, TRC_TYPE_BASE, 'i', 0, 0)
#define TRC_INSTANT_I(c, n, arg_name, arg_int) create_event_arg_int(c, n, TRC_TYPE_BASE | TRC_ARG_INT, 'i', 0, 0, arg_name, arg_int)
#define TRC_INSTANT_C(c, n, arg_name, arg_str) create_event_arg_str(c, n, TRC_TYPE_BASE | TRC_ARG_STR, 'i', 0, 0, arg_name, arg_str, false)
#define TRC_INSTANT_S(c, n, arg_name, arg_str) create_event_arg_str(c, n, TRC_TYPE_BASE | TRC_ARG_STR, 'i', 0, 0, arg_name, arg_str, true)

// Counters
#define TRC_COUNTER(c, n, val) create_event_arg_int(c, n, TRC_TYPE_BASE | TRC_ARG_INT, 'C', 0, 0, n, val)

// Metadata. Call at the start preferably.
#define TRC_META_PROCESS_NAME(n)      create_event_arg_str("", "process_name",      TRC_TYPE_BASE | TRC_ARG_STR, 'M', 0, 0, "name", n, true)
#define TRC_META_THREAD_NAME(n)       create_event_arg_str("", "thread_name",       TRC_TYPE_BASE | TRC_ARG_STR, 'M', 0, 0, "name", n, true)
#define TRC_META_THREAD_SORT_INDEX(i) create_event_arg_int("", "thread_sort_index", TRC_TYPE_BASE | TRC_ARG_INT, 'M', 0, 0, "sort_index", i)

// commodity macros that automatically use the file and function name
#define TRC_BEGIN_FUNC()   TRC_BEGIN(  __FILE__, __FUNCTION__)
#define TRC_END_FUNC()     TRC_END(    __FILE__, __FUNCTION__)
#define TRC_SCOPE_FUNC()   TRC_SCOPE(  __FILE__, __FUNCTION__)
#define TRC_INSTANT_FUNC() TRC_INSTANT(__FILE__, __FUNCTION__)


#ifdef __cplusplus
} // extern "C"

// These are optimized to use X events (combined B and E). Much easier to do in C++ than in C.
class TRCGuard
{
public:
	TRCGuard(const char *category, const char *name)
    : category_(category), name_(name)
    {
		start_time_ = trc_get_time();
	}
	~TRCGuard()
    {
        create_explicit_time_event(start_time_, category_, name_, TRC_TYPE_DURATION, 'X', 0, trc_get_time()-start_time_);
	}

private:
	const char *category_;
	const char *name_;
	double start_time_;
};


#endif
