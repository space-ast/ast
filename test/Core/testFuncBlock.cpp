///
/// @file      testFuncBlock.cpp
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

#include "AstCore/Simulation.hpp"
#include "AstCore/SimulationAPI.hpp"
#include "AstTest/Test.h"

AST_USING_NAMESPACE

TEST(FuncBlock, Connect)
{
    BlockSin sinBlock;                                      // 正弦函数块
    BlockGain gainBlock;                                    // 增益函数块
    gainBlock.setGain(2.0);                                 // 设置增益为2.0
    FuncBlock::connect(&sinBlock, 0, &gainBlock, 0);        // 连接正弦函数块的输出到增益函数块的输入
    SimTime time;                                           // 仿真时间
    time.setElapsedTime(1);

    sinBlock.evaluate(time);                                // 执行正弦函数块
    gainBlock.evaluate(time);                               // 执行增益函数块
    auto port = gainBlock.getOutputPort(0);                 // 获取增益函数块的输出端口
    double* result = port->getSignalDouble();               // 获取增益函数块的输出端口的双精度信号指针
    EXPECT_TRUE(result != nullptr);
    printf("result: %f\n", *result);
    EXPECT_EQ(*result, 2.0 * sin(time.elapsedTime()));

    double* result2 = aGetOutputPortRealSignal(&gainBlock, 0);  // 获取增益函数块的输出端口的双精度信号指针
    EXPECT_TRUE(result2 != nullptr);
    printf("result2: %f\n", *result2);
    EXPECT_EQ(*result2, 2.0 * sin(time.elapsedTime()));
}


GTEST_MAIN();