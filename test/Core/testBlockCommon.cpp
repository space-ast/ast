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

TEST(BlockCommon, SimpleCase)
{
    BlockSin sinBlock;                                      // 正弦函数块
    BlockGain gainBlock;                                    // 增益函数块
    gainBlock.setGain(2.0);                                 // 设置增益为2.0
    FuncBlock::connect(&sinBlock, 0, &gainBlock, 0);        // 连接正弦函数块的输出到增益函数块的输入
    SimTime time;                                           // 仿真时间
    time.setElapsedTime(1);

    sinBlock.run(time);                                // 执行正弦函数块
    gainBlock.run(time);                               // 执行增益函数块
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

// 测试数学运算block
TEST(BlockCommon, MathOperations)
{
    SimTime time;
    time.setElapsedTime(0);

    // 测试BlockSum/BlockAdd
    {   
        BlockConstant const1;
        BlockConstant const2;
        BlockSum sum;
        
        const1.setValue(5.0);
        const2.setValue(3.0);
        
        FuncBlock::connect(&const1, 0, &sum, 0);
        FuncBlock::connect(&const2, 0, &sum, 1);
        
        const1.run(time);
        const2.run(time);
        sum.run(time);
        
        double* result = aGetOutputPortRealSignal(&sum, 0);
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 8.0);
    }
    
    // 测试BlockSubtract
    {   
        BlockConstant const1;
        BlockConstant const2;
        BlockSubtract subtract;
        
        const1.setValue(5.0);
        const2.setValue(3.0);
        
        FuncBlock::connect(&const1, 0, &subtract, 0);
        FuncBlock::connect(&const2, 0, &subtract, 1);
        
        const1.run(time);
        const2.run(time);
        subtract.run(time);
        
        double* result = aGetOutputPortRealSignal(&subtract, 0);
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 2.0);
    }
    
    // 测试BlockProduct
    {   
        BlockConstant const1;
        BlockConstant const2;
        BlockProduct product;
        
        const1.setValue(5.0);
        const2.setValue(3.0);
        
        FuncBlock::connect(&const1, 0, &product, 0);
        FuncBlock::connect(&const2, 0, &product, 1);
        
        const1.run(time);
        const2.run(time);
        product.run(time);
        
        double* result = aGetOutputPortRealSignal(&product, 0);
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 15.0);
    }
    
    // 测试BlockDivide
    {   
        BlockConstant const1;
        BlockConstant const2;
        BlockDivide divide;
        
        const1.setValue(6.0);
        const2.setValue(3.0);
        
        FuncBlock::connect(&const1, 0, &divide, 0);
        FuncBlock::connect(&const2, 0, &divide, 1);
        
        const1.run(time);
        const2.run(time);
        divide.run(time);
        
        double* result = aGetOutputPortRealSignal(&divide, 0);
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 2.0);
    }
    
    // 测试BlockGain
    {   
        BlockConstant const1;
        BlockGain gain;
        
        const1.setValue(5.0);
        gain.setGain(3.0);
        
        FuncBlock::connect(&const1, 0, &gain, 0);
        
        const1.run(time);
        gain.run(time);
        
        double* result = aGetOutputPortRealSignal(&gain, 0);
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 15.0);
    }
}

// 测试逻辑运算block
TEST(BlockCommon, LogicalOperations)
{
    SimTime time;
    time.setElapsedTime(0);

    // 测试BlockLogicalOperator - AND
    {   
        BlockLogicalOperator logicOp(ELogicalOperatorType::eAnd);
        
        // 创建临时变量并将地址分配给输入指针
        int temp1 = 1; // true
        int temp2 = 1; // true
        
        // 获取输入端口的指针指针
        int** input1Ptr = (int**)logicOp.getInputPort(0)->getSignalPtr();
        int** input2Ptr = (int**)logicOp.getInputPort(1)->getSignalPtr();
        
        // 分配临时变量的地址
        *input1Ptr = &temp1;
        *input2Ptr = &temp2;
        
        logicOp.run(time);
        
        int* result = (int*)logicOp.getOutputPort(0)->getSignal();
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 1); // true AND true = true
    }
    
    // 测试BlockLogicalOperator - OR
    {   
        BlockLogicalOperator logicOp(ELogicalOperatorType::eOr);
        
        // 创建临时变量并将地址分配给输入指针
        int temp1 = 1; // true
        int temp2 = 0; // false
        
        // 获取输入端口的指针指针
        int** input1Ptr = (int**)logicOp.getInputPort(0)->getSignalPtr();
        int** input2Ptr = (int**)logicOp.getInputPort(1)->getSignalPtr();
        
        // 分配临时变量的地址
        *input1Ptr = &temp1;
        *input2Ptr = &temp2;
        
        logicOp.run(time);
        
        int* result = (int*)logicOp.getOutputPort(0)->getSignal();
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 1); // true OR false = true
    }
    
    // 测试BlockLogicalOperator - NOT
    {   
        BlockLogicalOperator logicOp(ELogicalOperatorType::eNot);
        
        // 创建临时变量并将地址分配给输入指针
        int temp1 = 0; // false
        
        // 获取输入端口的指针指针
        int** input1Ptr = (int**)logicOp.getInputPort(0)->getSignalPtr();
        
        // 分配临时变量的地址
        *input1Ptr = &temp1;
        
        logicOp.run(time);
        
        int* result = (int*)logicOp.getOutputPort(0)->getSignal();
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 1); // NOT false = true
    }
}

// 测试关系运算block
TEST(BlockCommon, RelationalOperations)
{
    SimTime time;
    time.setElapsedTime(0);

    // 测试BlockRelationalOperator - Equal
    {   
        BlockConstant const1;
        BlockConstant const2;
        BlockRelationalOperator relationalOp(ERelationalOperatorType::eEqual);
        
        const1.setValue(5.0);
        const2.setValue(5.0);
        
        FuncBlock::connect(&const1, 0, &relationalOp, 0);
        FuncBlock::connect(&const2, 0, &relationalOp, 1);
        
        const1.run(time);
        const2.run(time);
        relationalOp.run(time);
        
        int* result = (int*)relationalOp.getOutputPort(0)->getSignal();
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 1); // 5 == 5 = true
    }
    
    // 测试BlockRelationalOperator - GreaterThan
    {   
        BlockConstant const1;
        BlockConstant const2;
        BlockRelationalOperator relationalOp(ERelationalOperatorType::eGreaterThan);
        
        const1.setValue(10.0);
        const2.setValue(5.0);
        
        FuncBlock::connect(&const1, 0, &relationalOp, 0);
        FuncBlock::connect(&const2, 0, &relationalOp, 1);
        
        const1.run(time);
        const2.run(time);
        relationalOp.run(time);
        
        int* result = (int*)relationalOp.getOutputPort(0)->getSignal();
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 1); // 10 > 5 = true
    }
}

// 测试不连续函数block
TEST(BlockCommon, Discontinuities)
{
    SimTime time;
    time.setElapsedTime(0);

    // 测试BlockSaturation
    {   
        BlockConstant const1;
        BlockSaturation saturation;
        
        // 设置饱和上下限
        saturation.setUpperLimit(10.0);
        saturation.setLowerLimit(-10.0);
        
        // 测试超出上限
        const1.setValue(15.0);
        FuncBlock::connect(&const1, 0, &saturation, 0);
        
        const1.run(time);
        saturation.run(time);
        
        double* result = aGetOutputPortRealSignal(&saturation, 0);
        EXPECT_TRUE(result != nullptr);
        EXPECT_EQ(*result, 10.0);
    }
}

GTEST_MAIN();