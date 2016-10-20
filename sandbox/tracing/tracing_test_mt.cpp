// example taken from https://github.com/hrydgard/minitrace
#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "tracing/tracing.h"

// Does some meaningless work.
int work(int cycles) {
    int a = cycles;
    for (int i = 0; i < cycles; i++) {
        a ^= 373;
        a = (a << 13) | (a >> (32 - 13));
    }
    return a;
}

int worker_thread(int id) {
    char temp[256]; sprintf(temp, "Worker Thread %i", id);
    TRC_META_THREAD_NAME(temp);
    TRC_FLOW_START("flow","phase2",id);
    int x = 0;
    for (int i = 0; i < 32; i++) {
        TRC_BEGIN_I(__FILE__, "Worker", "ID", id);
        x += work((rand() & 0x7fff) * 1000);
        TRC_END(__FILE__, "Worker");
    }
    TRC_FLOW_FINISH("flow","phase2",id);
    return x;
}

void phase2() {
    TRC_SCOPE_FUNC();
    for (int i = 0; i < 10000; i++) {
        TRC_BEGIN_FUNC();
        work(3000);
        TRC_END_FUNC();
    }
}

int main(int argc, const char *argv[]) {
    // get number of threads from command line
    int num_threads = 8;
    if (argc>1)
        num_threads = atoi(argv[1]);

    // init the tracing system
    trc_init("test_trace_mt.json");
    TRC_META_PROCESS_NAME("Multithreaded Test");
    TRC_META_THREAD_NAME("Main Thread");

    TRC_BEGIN_FUNC();
    std::vector<std::thread> threads(num_threads);
    for (int i = 0; i < threads.size(); i++) {
        threads[i] = std::thread(worker_thread,i);
    }
    for (int i = 0; i < threads.size(); i++) {
        threads[i].join();
    }
    phase2();

    TRC_END_FUNC();
    // terminate the tracing system
    trc_shutdown();

    return 0;
}
