/// @file      testFileSystem.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
/// @date      23.11.2025
/// @copyright 版权所有 (C) 2025-present, ast项目.

/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

//#define _A_STD_FILESYSTEM_USE_EXPERIMENTAL
#include "AstUtil/FileSystem.hpp"
#include "AstTest/AstTestMacro.h"

AST_USING_NAMESPACE

 
void listDirectory(const fs::path& path)
{
    if (!fs::exists(path)) {
        std::cout << "路径不存在: " << path.string() << std::endl;
        return;
    }

    try {
        for (const auto& entry : fs::directory_iterator(path)) {
			std::string filename = entry.path().filename().string();
            std::cout << "filename: " << filename << std::endl;
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cout << "文件系统错误: " << e.what() << std::endl;
    }
}
 

TEST(FileSystem, CurrentPath)
{
    namespace fs = _AST filesystem;
    {
        std::string filepath = u8"新建文件夹";
        fs::path current_path = filepath;
        bool same = filepath == current_path.string();
        EXPECT_TRUE(same);
    }
    {
        fs::path current_path = fs::current_path();
        printf("current_path: %s\n", current_path.string().c_str());
		listDirectory(current_path);
    }
}
 

GTEST_MAIN()
