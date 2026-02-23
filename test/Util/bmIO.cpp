///
/// @file      bmIO.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-23
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

#include "AstUtil/IO.hpp"
#include <benchmark/benchmark.h>

AST_USING_NAMESPACE


void bmGetFilePath(benchmark::State& state)
{
    std::FILE* file = posix::fopen("./data/README.md", "r");
    if(file == nullptr) {
        printf("failed to open file");
        return;
    }
    for (auto _ : state) {
        std::string filepath;
        auto rc = aGetFilePath(file, filepath);
        A_UNUSED(rc);
        benchmark::DoNotOptimize(filepath);
    }
}


BENCHMARK(bmGetFilePath);


BENCHMARK_MAIN();
