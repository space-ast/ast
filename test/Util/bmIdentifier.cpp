///
/// @file      bmIdentifier.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-18
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "AstUtil/IdentifierTable.hpp"
#include "AstUtil/IdentifierAPI.hpp"
#include <benchmark/benchmark.h>


AST_USING_NAMESPACE 

struct Param1 {
    std::string str;
};

struct Param2{
    Identifier* id;
};


void bmStringAssign(benchmark::State& state) {
    Param1 param;
    int length = state.range(0);
    std::string str(length, 'a');
    for (auto _ : state) {
        param.str = str;
        benchmark::DoNotOptimize(param.str);
    }
}

BENCHMARK(bmStringAssign)->Range(1, 100);


void bmIdentitiferAssign(benchmark::State& state) {
    Param2 param;
    int length = state.range(0);
    std::string str(length, 'a');
    for (auto _ : state) {
        param.id = aIdentifier(str);
        benchmark::DoNotOptimize(param.id);
    }
}

BENCHMARK(bmIdentitiferAssign)->Range(1, 100);


void bmStringCompare(benchmark::State& state) {
    Param1 param;
    int length = state.range(0);
    std::string str(length, 'a');
    param.str = str;
    str = std::string(length, 'b');
    for (auto _ : state) {
        bool isEqual = param.str == str;
        benchmark::DoNotOptimize(isEqual);
    }
}

BENCHMARK(bmStringCompare)->Range(1, 100);


void bmIdentifierCompare(benchmark::State& state) {
    Param2 param;
    int length = state.range(0);
    std::string str(length, 'a');
    param.id = aIdentifier(str);
    str = std::string(length, 'b');
    auto id2 = aIdentifier(str);
    for (auto _ : state) {
        bool isEqual = param.id == id2;
        benchmark::DoNotOptimize(isEqual);
    }
}

BENCHMARK(bmIdentifierCompare)->Range(1, 100);


std::vector<std::string> strings = {
    "1234",
    "abcdetdu1272312"
    "5678",
    "9012",
    "3456",
    "1234",
    "test",
    "37910274",
    "12",
    "abcdetdu127312"
};

void bmStringUnoderedMap(benchmark::State& state) {
    std::unordered_map<std::string, int> map;
    std::string str(10, 'a');
    int length = state.range(0);
    int i = 0;
    for (const auto& str : strings) {
        map.insert({str + std::to_string(i), 0});
        if (i++ >= length) {
            break;
        }
    }
    for (auto _ : state) {
        auto it = map.find(str);
        benchmark::DoNotOptimize(it);
    }
}

BENCHMARK(bmStringUnoderedMap)->Range(1, 100);

void bmIdentifierUnoderedMap(benchmark::State& state) {
    std::unordered_map<Identifier*, int> map;
    std::string str(10, 'a');
    int length = state.range(0);
    int i = 0;
    auto id = aIdentifier(str);
    for (const auto& str : strings) {
        map.insert({aIdentifier(str + std::to_string(i)), 0});
        if (i++ >= length) {
            break;
        }
    }
    for (auto _ : state) {
        auto it = map.find(id);
        benchmark::DoNotOptimize(it);
    }
}

BENCHMARK(bmIdentifierUnoderedMap)->Range(1, 100);

BENCHMARK_MAIN();

