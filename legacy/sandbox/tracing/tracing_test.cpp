// example taken from https://github.com/hrydgard/minitrace
#include "tracing/tracing.h"

#include <thread>
#include <chrono>

void c() {
	TRC_SCOPE("function", "c()");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void b() {
	TRC_SCOPE("function", "b()");
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
	c();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

void a() {
	TRC_SCOPE("function", "a()");
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
	b();
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}


int main(int argc, const char *argv[]) {
	int i;
	trc_init("test_trace.json",10000);

	TRC_META_PROCESS_NAME("tracing_test");
	TRC_META_THREAD_NAME("main thread");

	int async_id1{101};
	int async_id2{102};

	TRC_ASYNC_START("background", "async", async_id1);
	TRC_ASYNC_START("background", "async", async_id2);

	TRC_COUNTER("main", "greebles", 3);
	TRC_BEGIN("main", "outer");
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
	for (i = 0; i < 3; i++) {
		TRC_BEGIN("main", "inner");
        std::this_thread::sleep_for(std::chrono::milliseconds(40));
		TRC_END("main", "inner");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
		TRC_COUNTER("main", "greebles", 3 * i + 10);
	}
	TRC_ASYNC_STEP("background", "async", async_id1, "async step");
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
	TRC_END("main", "outer");
	TRC_COUNTER("main", "greebles", 0);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
	a();

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
	TRC_INSTANT("main", "the end");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
	TRC_ASYNC_FINISH("background", "async", async_id1);
	TRC_ASYNC_FINISH("background", "async", async_id2);

	trc_shutdown();
	return 0;
}
