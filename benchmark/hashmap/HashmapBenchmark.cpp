#include <benchmark/benchmark.h>

#include "Hashmap.hpp"

#include <stdlib.h>
#include <unordered_map>

static void DISABLED_BM_MyHashmapCreation(benchmark::State& state) {
    for (auto _: state) {
        Hashmap<int, int> map;
    }
}

BENCHMARK(DISABLED_BM_MyHashmapCreation);

static void DISABLED_BM_StdUnorderedMapCreation(benchmark::State& state) {
    for (auto _: state) {
        std::unordered_map<int, int> map;
    }
}

BENCHMARK(DISABLED_BM_StdUnorderedMapCreation);

static void BM_MyHashmapInsertion(benchmark::State& state) {
    Hashmap<int, int> map;
    for (auto _: state) {
        state.PauseTiming();
        int x = std::rand();
        state.ResumeTiming();
        for (int i = 0; i < state.range(0); i++) {
            map.insert({x, x});
        }
    }
}

BENCHMARK(BM_MyHashmapInsertion)->RangeMultiplier(2)->Range(8<<8, 8<<16);

static void BM_StdUnorderedMapInsertion(benchmark::State& state) {
    std::unordered_map<int, int> map;
    for (auto _: state) {
        state.PauseTiming();
        int x = std::rand();
        state.ResumeTiming();
        for (int i = 0; i < state.range(0); i++) {
            map.insert({x, x});
        }
    }
}

BENCHMARK(BM_StdUnorderedMapInsertion)->RangeMultiplier(2)->Range(8<<8, 8<<16);

static void BM_StdOrderedMapInsertion(benchmark::State& state) {
    std::map<int, int> map;
    for (auto _: state) {
        state.PauseTiming();
        int x = std::rand();
        state.ResumeTiming();
        for (int i = 0; i < state.range(0); i++) {
            map.insert({x, x});
        }
    }
}

BENCHMARK(BM_StdOrderedMapInsertion)->RangeMultiplier(2)->Range(8<<8, 8<<16);

BENCHMARK_MAIN();
