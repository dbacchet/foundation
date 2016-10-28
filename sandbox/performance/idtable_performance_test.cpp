// example taken from https://github.com/hrydgard/minitrace
#include "common/idtable.h"
#include "tracing/tracing.h"

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <thread>
#include <chrono>
#include <map>
#include <vector>
#include <string>


struct TestData {
    int64_t counter;
    double   value;
    char     data[10];
    int64_t  timestamp;
};

typedef int64_t MapID;

void performance_std_map(unsigned int N) {
    int64_t uuid = 0;
    std::vector<MapID> ids;
    std::map<MapID,TestData> mymap;
    srand(0);
    // creation
    auto tp_creation_start = std::chrono::high_resolution_clock::now();
    for (uint32_t i=0; i<N; i++) {
        auto id = uuid++;
        mymap[id] = TestData { .counter=i, .value=0.0, .timestamp=10010101 };
        ids.push_back(id);
    }
    auto tp_creation_end = std::chrono::high_resolution_clock::now();
    // replace N/2 elements by removing and adding batches
    auto tp_replace_start = std::chrono::high_resolution_clock::now();
    const int num_batches = 10;
    for (auto k=0; k<num_batches; k++) {
        auto n_elem = N/2/num_batches;
        std::vector<int> candidates(n_elem);
        // remove
        for (auto&& c: candidates) {
            c = rand() % N;
            mymap.erase(ids[c]);
            ids[c] = -1;
        }
        // add
        for (auto c: candidates) {
            auto id = uuid++;
            mymap[id] = TestData { .counter=100, .value=1.0, .timestamp=20020202 };
            ids.push_back(id);
        }
    }
    auto tp_replace_end = std::chrono::high_resolution_clock::now();
    auto tp_replace = std::chrono::high_resolution_clock::now();
    // increase counter accessing by id
    auto tp_increase_id_start = std::chrono::high_resolution_clock::now();
    for (auto id: ids) {
        if (id>0)
            mymap[id].counter += 1;
    }
    auto tp_increase_id_end = std::chrono::high_resolution_clock::now();
    // increase counter accessing by iterator
    auto tp_increase_iter_start = std::chrono::high_resolution_clock::now();
    for (auto&& it:mymap) {
        it.second.counter +=1;
    }
    auto tp_increase_iter_end = std::chrono::high_resolution_clock::now();
    // summary
    std::cout << "std::map time stats:\n"
              << " creation: "         << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_creation_end-tp_creation_start).count() << std::endl
              << " replace: "          << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_replace_end-tp_replace_start).count() << std::endl
              << " increase by id: "   << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_id_end-tp_increase_id_start).count() << std::endl
              << " increase by iter: " << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_iter_end-tp_increase_iter_start).count() << std::endl;
}

void performance_idtable(unsigned int N) {
    std::vector<ID> ids;
    IDTable<TestData> mytable;
    srand(0);
    // creation
    auto tp_creation_start = std::chrono::high_resolution_clock::now();
    for (uint32_t i=0; i<N; i++) {
        auto id = mytable.add(TestData { .counter=i, .value=0.0, .timestamp=10010101 });
        ids.push_back(id);
    }
    auto tp_creation_end = std::chrono::high_resolution_clock::now();
    // replace N/2 elements by removing and adding batches
    auto tp_replace_start = std::chrono::high_resolution_clock::now();
    const int num_batches = 10;
    for (auto k=0; k<num_batches; k++) {
        auto n_elem = N/2/num_batches;
        std::vector<int> candidates(n_elem);
        // remove
        for (auto&& c: candidates) {
            c = rand() % N;
            mytable.remove(ids[c]);
            ids[c].index = UINT32_MAX;
        }
        // add
        for (auto c: candidates) {
            auto id = mytable.add(TestData { .counter=100, .value=1.0, .timestamp=20020202 });
            ids.push_back(id);
        }
    }
    auto tp_replace_end = std::chrono::high_resolution_clock::now();
    auto tp_replace = std::chrono::high_resolution_clock::now();
    // increase counter accessing by id
    auto tp_increase_id_start = std::chrono::high_resolution_clock::now();
    for (auto id: ids) {
        // if(mytable.has(id))
        if (id.index<UINT32_MAX)
            mytable.get(id).counter += 1;
    }
    auto tp_increase_id_end = std::chrono::high_resolution_clock::now();
    // increase counter accessing by iterator
    auto tp_increase_iter_start = std::chrono::high_resolution_clock::now();
    for (auto&& obj: mytable._objects) {
        obj.counter +=1;
    }
    auto tp_increase_iter_end = std::chrono::high_resolution_clock::now();
    // summary
    std::cout << "IDTable time stats:\n"
              << " creation: "         << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_creation_end-tp_creation_start).count() << std::endl
              << " replace: "          << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_replace_end-tp_replace_start).count() << std::endl
              << " increase by id: "   << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_id_end-tp_increase_id_start).count() << std::endl
              << " increase by iter: " << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_iter_end-tp_increase_iter_start).count() << std::endl;
}

void performance_idtable2(unsigned int N) {
    // int64_t uuid = 0;
    std::vector<ID> ids;
    IDTable<TestData> mytable;
    srand(0);
    auto tp_start = std::chrono::high_resolution_clock::now();
    // creation
    for (uint32_t i=0; i<N; i++) {
        auto id = mytable.add(TestData { .counter=i, .value=0.0, .timestamp=10010101 });
        ids.push_back(id);
    }
    auto tp_creation = std::chrono::high_resolution_clock::now();
    // replace N/2 elements by removing and adding batches
    const int num_batches = 1;
    for (auto k=0; k<num_batches; k++) {
        auto n_elem = N/2/num_batches;
        std::vector<int> candidates(n_elem);
        // remove
        for (auto&& c: candidates) {
            c = rand() % N;
            mytable.remove(ids[c]);
            // ids.erase(ids[c])
        }
        // add
        for (auto c: candidates) {
            std::cout << c << " ";
            ids[c] = mytable.add(TestData { .counter=100, .value=1.0, .timestamp=20020202 });
            std::cout << ids[c].index << " " << ids[c].internal_id << std::endl;
        }
    }
    auto tp_replace = std::chrono::high_resolution_clock::now();
    // increase counter accessing by id
    for (auto id: ids) {
        if (mytable.has(id))
            mytable.get(id).counter += 1;
        else
            std::cout << "not found: " << id.index << " " << id.internal_id << std::endl;
    }
    auto tp_increase_id = std::chrono::high_resolution_clock::now();
    // increase counter accessing by iterator
    for (auto&& it:mytable._objects) {
        it.counter +=1;
    }
    auto tp_increase_iter = std::chrono::high_resolution_clock::now();
    // summary
    std::cout << "IDTable time stats:\n"
              << " creation: "         << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_creation-tp_start).count() << std::endl
              << " replace: "          << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_replace-tp_creation).count() << std::endl
              << " increase by id: "   << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_id-tp_replace).count() << std::endl
              << " increase by iter: " << std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_iter-tp_increase_id).count() << std::endl;
}


// test the idtable performance on the following scenario:
// 1) create N objects
// 2) replace N/2 random objects
// 3) increase the counter by 1 accesing by id
// 4) increase the counter by one accessing by iteration

int main(int argc, char *argv[]) {

    // std::string tmp;
    // std::cout << "DEBUG MODE: type something...\n";
    // std::cin >> tmp;

    int N = argc>1 ? atoi(argv[1]) : 1000;

    std::cout << "testing std::map with " << N << " elements" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
    auto elapsed_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
    auto elapsed_nanoseconds  = std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
    std::cout << "elapsed time: " << elapsed_milliseconds << " " << elapsed_microseconds << " " << elapsed_nanoseconds << " " << std::endl;


    performance_std_map(N);
    performance_idtable(N);
    return 0;
}


