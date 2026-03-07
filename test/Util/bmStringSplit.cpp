//#define AST_WITH_ABSEIL
#ifdef AST_WITH_ABSEIL
#include "absl/strings/str_split.h"
#include "AstUtil/StringSplit.hpp"
#include <benchmark/benchmark.h>

void bmStringViewSplitAbseil(benchmark::State& state)
{
    std::string str = "a,b,c,d";
    for (auto _ : state)
    {
        std::vector<absl::string_view> parts = absl::StrSplit(str, ',');
        benchmark::DoNotOptimize(parts);
    }
}

BENCHMARK(bmStringViewSplitAbseil);

void bmStringViewSplitAstUtil(benchmark::State& state)
{
    AST_USING_NAMESPACE
    std::string str = "a,b,c,d";
    for (auto _ : state)
    {
        std::vector<StringView> parts = aStrSplit(str, ',');
        benchmark::DoNotOptimize(parts);
    }
}

BENCHMARK(bmStringViewSplitAstUtil);


void bmStringSplitAbseil(benchmark::State& state)
{
    std::string str = "a,b,c,d";
    for (auto _ : state)
    {
        std::vector<std::string> parts = absl::StrSplit(str, ',');
        benchmark::DoNotOptimize(parts);
    }
}

BENCHMARK(bmStringSplitAbseil);

void bmStringSplitAstUtil(benchmark::State& state)
{
    AST_USING_NAMESPACE
    std::string str = "a,b,c,d";
    for (auto _ : state)
    {
        std::vector<std::string> parts = aStrSplit(str, ',');
        benchmark::DoNotOptimize(parts);
    }
}

BENCHMARK(bmStringSplitAstUtil);

BENCHMARK_MAIN();

#else
int main(){}
#endif