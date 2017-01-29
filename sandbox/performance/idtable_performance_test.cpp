// example taken from https://github.com/hrydgard/minitrace
#include "common/idtable.h"
#include "common/static_idtable.h"
#include "common/format.h"
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

struct PerfResults {
    int64_t creation;
    int64_t replace;
    int64_t access_by_id;
    int64_t access_by_iter;
};

PerfResults performance_std_map(unsigned int N) {
    int64_t uuid = 0;
    std::vector<MapID> ids;
    std::map<MapID,TestData> mymap;
    srand(0);
    // creation
    auto tp_creation_start = std::chrono::high_resolution_clock::now();
    for (uint32_t i=0; i<N; i++) {
        auto id = uuid++;
        mymap[id] = TestData { .counter=i, .value=0.0, .data={0,0,0,0,0,0,0,0,0,0}, .timestamp=10010101 };
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
            mymap[id] = TestData { .counter=100, .value=1.0, .data={0,0,0,0,0,0,0,0,0,0}, .timestamp=20020202 };
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
    return PerfResults {
              .creation       = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_creation_end-tp_creation_start).count(),
              .replace        = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_replace_end-tp_replace_start).count(),
              .access_by_id   = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_id_end-tp_increase_id_start).count(),
              .access_by_iter = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_iter_end-tp_increase_iter_start).count()
    };
}


PerfResults performance_idtable(unsigned int N) {

    std::vector<ID> ids;
    IDTable<TestData> mytable;
    srand(0);
    // creation
    auto tp_creation_start = std::chrono::high_resolution_clock::now();
    for (uint32_t i=0; i<N; i++) {
        auto id = mytable.add(TestData { .counter=i, .value=0.0, .data={0,0,0,0,0,0,0,0,0,0}, .timestamp=10010101 });
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
            auto id = mytable.add(TestData { .counter=100, .value=1.0, .data={0,0,0,0,0,0,0,0,0,0}, .timestamp=20020202 });
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
    for (auto i=0; i<mytable.size(); i++) {
        mytable._objects[i].counter +=1;
    }
    auto tp_increase_iter_end = std::chrono::high_resolution_clock::now();
    // summary
    return PerfResults {
              .creation       = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_creation_end-tp_creation_start).count(),
              .replace        = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_replace_end-tp_replace_start).count(),
              .access_by_id   = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_id_end-tp_increase_id_start).count(),
              .access_by_iter = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_iter_end-tp_increase_iter_start).count()
    };
}


PerfResults performance_static_idtable(unsigned int N) {
    std::vector<ID> ids;
    StaticIDTable<TestData,100> mytable;
    srand(0);
    // creation
    auto tp_creation_start = std::chrono::high_resolution_clock::now();
    for (uint32_t i=0; i<N; i++) {
        auto id = mytable.add(TestData { .counter=i, .value=0.0, .data={0,0,0,0,0,0,0,0,0,0}, .timestamp=10010101 });
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
            auto id = mytable.add(TestData { .counter=100, .value=1.0, .data={0,0,0,0,0,0,0,0,0,0}, .timestamp=20020202 });
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
    for (auto i=0; i<mytable.size(); i++) {
        mytable._objects[i].counter +=1;
    }
    auto tp_increase_iter_end = std::chrono::high_resolution_clock::now();
    // summary
    return PerfResults {
              .creation       = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_creation_end-tp_creation_start).count(),
              .replace        = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_replace_end-tp_replace_start).count(),
              .access_by_id   = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_id_end-tp_increase_id_start).count(),
              .access_by_iter = std::chrono::duration_cast<std::chrono::nanoseconds>(tp_increase_iter_end-tp_increase_iter_start).count()
    };
}


// test the idtable performance on the following scenario:
// 1) create N objects
// 2) replace N/2 random objects
// 3) increase the counter by 1 accessing by id
// 4) increase the counter by one accessing by iteration

int main(int argc, char *argv[]) {

    // std::string tmp;
    // std::cout << "DEBUG MODE: type something...\n";
    // std::cin >> tmp;

    int N = argc>1 ? atoi(argv[1]) : 1000;


    // /////// //
    // IDTable //
    // /////// //

    std::cout << std::endl << "testing std::map and IDTable with " << N << " elements" << std::endl;
    PerfResults map_res     = performance_std_map(N);
    PerfResults idtable_res = performance_idtable(N);

    fmt::print("\n{:-^72}\n", " Time Stats ");
    fmt::print("|{:>10}|{:>14}|{:>14}|{:>14}|{:>14}|\n", "","creation", "replace", "access_by_id", "access_by_iter");
    fmt::print("|{:<10}|{:>14}|{:>14}|{:>14}|{:>14}|\n", "std::map",map_res.creation, map_res.replace, map_res.access_by_id, map_res.access_by_iter);
    fmt::print("|{:<10}|{:>14}|{:>14}|{:>14}|{:>14}|\n", "IDTable",idtable_res.creation, idtable_res.replace, idtable_res.access_by_id, idtable_res.access_by_iter);
    fmt::print("{:-^72}\n", "");
    fmt::print("|{:<10}|{:>14}|{:>14}|{:>14}|{:>14}|\n", "ratio",(float)map_res.creation/idtable_res.creation,
                                                                (float)map_res.replace/idtable_res.replace,
                                                                (float)map_res.access_by_id/idtable_res.access_by_id,
                                                                (float)map_res.access_by_iter/idtable_res.access_by_iter);

    // ///////////// //
    // StaticIDTable //
    // ///////////// //

    const unsigned int Nstatic = 90;
    std::cout << std::endl << "testing IDTable and StaticIDTable with " << Nstatic << " elements" << std::endl;
    PerfResults dyn_idtable_res    = performance_idtable(Nstatic);
    PerfResults static_idtable_res = performance_static_idtable(Nstatic);

    fmt::print("\n{:-^72}\n", " IDTable perf comparison ");
    fmt::print("|{:>10}|{:>14}|{:>14}|{:>14}|{:>14}|\n", "","creation", "replace", "access_by_id", "access_by_iter");
    fmt::print("|{:<10}|{:>14}|{:>14}|{:>14}|{:>14}|\n", "Dynamic",dyn_idtable_res.creation, dyn_idtable_res.replace, dyn_idtable_res.access_by_id, dyn_idtable_res.access_by_iter);
    fmt::print("|{:<10}|{:>14}|{:>14}|{:>14}|{:>14}|\n", "Static",static_idtable_res.creation, static_idtable_res.replace, static_idtable_res.access_by_id, static_idtable_res.access_by_iter);
    fmt::print("{:-^72}\n", "");
    fmt::print("|{:<10}|{:>14}|{:>14}|{:>14}|{:>14}|\n", "ratio",(float)dyn_idtable_res.creation/static_idtable_res.creation,
                                                                (float)dyn_idtable_res.replace/static_idtable_res.replace,
                                                                (float)dyn_idtable_res.access_by_id/static_idtable_res.access_by_id,
                                                                (float)dyn_idtable_res.access_by_iter/static_idtable_res.access_by_iter);

    return 0;
}


