///
/// @file      bmParse.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-10
/// @copyright 版权所有 (C) 2025-present, ast项目.
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

#include "AstUtil/ParseFormat.hpp"
#include <benchmark/benchmark.h>

AST_USING_NAMESPACE

const std::string str_int = "123456789";

const std::string str_double = "123456789.0123456789";

static void parseInt_LibC_1(benchmark::State& state)
{
    int value = 0;
    for (auto _ : state)
    {
        _aParseInt_LibC_1(str_int, value);
    }
}
BENCHMARK(parseInt_LibC_1);


static void parseInt_LibC_2(benchmark::State& state)
{
    int value = 0;
    for (auto _ : state)
    {
        _aParseInt_LibC_2(str_int, value);
    }
}
BENCHMARK(parseInt_LibC_2);

#if A_CXX_VERSION >= 17
static void parseInt_FromChars(benchmark::State& state)
{
    int value = 0;
    for (auto _ : state)
    {
        _aParseInt_FromChars(str_int, value);
    }
}
BENCHMARK(parseInt_FromChars);
#endif

static void parseInt_StringStream(benchmark::State& state)
{
    int value = 0.0;
    for (auto _ : state)
    {
        _aParseInt_StringStream(str_int, value);
    }
}

BENCHMARK(parseInt_StringStream);


static void parseInt_Simple(benchmark::State& state)
{
    int value = 0;
    for (auto _ : state)
    {
        _aParseInt_Simple(str_int, value);
    }
}

BENCHMARK(parseInt_Simple);


static void parseInt_Scanf(benchmark::State& state)
{
    int value = 0;
    for (auto _ : state)
    {
        _aParseInt_Scanf(str_int, value);
    }
}
BENCHMARK(parseInt_Scanf);


static void parseDouble_LibC_1(benchmark::State& state)
{
    double value = 0.0;
    for (auto _ : state)
    {
        _aParseDouble_LibC_1(str_double, value);
    }
}
BENCHMARK(parseDouble_LibC_1);


static void parseDouble_LibC_2(benchmark::State& state)
{
    double value = 0.0;
    for (auto _ : state)
    {
        _aParseDouble_LibC_2(str_double, value);
    }
}
BENCHMARK(parseDouble_LibC_2);


#if A_CXX_VERSION>=17


static void parseDouble_FromChars(benchmark::State& state)
{
    double value = 0.0;
    for (auto _ : state)
    {
        _aParseDouble_FromChars(str_double, value);
    }
}
BENCHMARK(parseDouble_FromChars);

#endif

#ifdef AST_WITH_ABSEIL

static void parseDouble_FromChars_Abseil(benchmark::State& state)
{
    double value = 0.0;
    for (auto _ : state)
    {
        _aParseDouble_FromChars_Abseil(str_double, value);
    }
}
BENCHMARK(parseDouble_FromChars_Abseil);

#endif

static void parseDouble_StringStream(benchmark::State& state)
{
    double value = 0.0;
    for (auto _ : state)
    {
        _aParseDouble_StringStream(str_double, value);
    }
}
BENCHMARK(parseDouble_StringStream);


static void parseDouble_Scanf(benchmark::State& state)
{
    double value = 0.0;
    for (auto _ : state)
    {
        _aParseDouble_Scanf(str_double, value);
    }
}
BENCHMARK(parseDouble_Scanf);



BENCHMARK_MAIN();

