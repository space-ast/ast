///
/// @file      testSpiceParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-08
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

#include "ast/PCKParser.hpp"
#include "ast/RunTime.hpp"
#include "ast/Test.h"

AST_USING_NAMESPACE

TEST(PCKParser, KernelData)
{
    {
        char buf[] = "KEY1 = VALUE12\n";
        KernelData data = std::vector<char>(buf, buf + sizeof(buf));
        {
            auto doubleData = data.getDoubleData();
            EXPECT_TRUE(doubleData != nullptr);
            std::vector<double> dData = *doubleData;
            printf("dData.size(): %d\n", (int)dData.size());
        }
        int size = (int)data.getCharData()->size();
        EXPECT_EQ(size, sizeof(buf));
    }
    {
        KernelData data = std::vector<double>{1.0, 2.0, 3.0};
        auto charData = data.getCharData();
        EXPECT_EQ(charData->size(), 3 * sizeof(double));
    }
}

TEST(PCKParser, readData)
{
    PCKParser parser(aTestGetConfigValue("PCK_FILE").toString());
    KernelPool kernelPool;
    errc_t ret = parser.readData(kernelPool);
    EXPECT_EQ(ret, 0);
    size_t size = kernelPool.size();
    EXPECT_GT(size, 0);
    {
        auto doubleData = kernelPool.getDoubleData("BODY609_POLE_DEC");
        EXPECT_TRUE(doubleData != nullptr);
    }
}



GTEST_MAIN()
