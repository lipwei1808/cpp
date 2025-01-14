#include <benchmark/benchmark.h>

#include "String.hpp"

static void BM_StdEmptyStringCreation(benchmark::State& state) {
    for (auto _ : state)
        std::string empty_string;
}

BENCHMARK(BM_StdEmptyStringCreation);

static void BM_MyEmptyStringCreation(benchmark::State& state) {
    for (auto _ : state)
        String empty_string;
}

BENCHMARK(BM_MyEmptyStringCreation);

static void BM_StdStringCreation(benchmark::State& state) {
    for (auto _ : state)
        std::string empty_string{"hello world"};
}

BENCHMARK(BM_StdStringCreation);

static void BM_MyStringCreation(benchmark::State& state) {
    for (auto _ : state)
        String empty_string{"hello world"};
}

BENCHMARK(BM_MyStringCreation);

static void BM_StdLargeStringCreation(benchmark::State& state) {
    char largeString[200] = {0};
    for (auto _ : state)
        std::string empty_string{largeString};
}

BENCHMARK(BM_StdLargeStringCreation);

static void BM_MyLargeStringCreation(benchmark::State& state) {
    char largeString[200] = {0};
    for (auto _ : state)
        String empty_string{largeString};
}

BENCHMARK(BM_MyLargeStringCreation);

static void BM_StdStringCopy(benchmark::State& state) {
    std::string x = "hello";
    for (auto _ : state) {
        std::string copy(x);
        copy[0] = 'a';
    }
}
BENCHMARK(BM_StdStringCopy);

static void BM_MyStringCopy(benchmark::State& state) {
    String x = "hello";
    for (auto _ : state) {
        String copy(x);
        copy[0] = 'a';
    }
}
BENCHMARK(BM_MyStringCopy);

BENCHMARK_MAIN();
