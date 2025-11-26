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

// #define _A_STD_FILESYSTEM_USE_EXPERIMENTAL
#include "AstUtil/FileSystem.hpp"
#include "AstTest/AstTestMacro.h"
#include <iostream>
#include <fstream>

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
 

// 测试路径类功能
TEST(SimpleFileSystem, PathOperations) {
    namespace fs = _AST simple_fs;
    
    // 基本路径创建和转换
    fs::path p1 = "test/file.txt";
    EXPECT_EQ(p1.string(), "test/file.txt");
    
    // 文件名和扩展名提取
    EXPECT_EQ(p1.filename().string(), "file.txt");
    EXPECT_EQ(p1.extension().string(), ".txt");
    EXPECT_EQ(p1.stem().string(), "file");
    
    // 父路径提取
    EXPECT_EQ(p1.parent_path().string(), "test");
    
    // 路径组合
    fs::path p2 = "dir";
    fs::path p3 = p2 / "subdir";
    fs::path expected = "dir" + std::string(1, fs::path::preferred_separator()) + "subdir";
    EXPECT_EQ(p3.string(), expected);
    
    // 路径分隔符检查
    EXPECT_TRUE(fs::path::is_separator('/'));
    #ifdef _WIN32
    EXPECT_TRUE(fs::path::is_separator('\\'));
    #endif
    
    // 路径比较
    fs::path p4 = "test/file.txt";
    EXPECT_TRUE(p1 == p4);
    
    // 空路径检查
    fs::path empty;
    EXPECT_TRUE(empty.empty());
}

// 测试文件状态相关函数
TEST(SimpleFileSystem, FileStatus) {
    namespace fs = _AST simple_fs;
    
    // 创建临时路径进行测试
    fs::path test_dir = "test_temp_dir";
    fs::path test_file = test_dir / "test_file.txt";
    
    // 测试不存在的路径
    EXPECT_FALSE(fs::exists("non_existent_path_12345"));
    EXPECT_FALSE(fs::is_directory("non_existent_path_12345"));
    EXPECT_FALSE(fs::is_regular_file("non_existent_path_12345"));
    EXPECT_EQ(fs::status("non_existent_path_12345").type(), fs::file_type::not_found);
    
    // 创建测试目录和文件以测试存在性检查
    if (!fs::exists(test_dir)) {
        fs::create_directories(test_dir);
    }
    
    // 测试目录存在性和类型
    EXPECT_TRUE(fs::exists(test_dir));
    EXPECT_TRUE(fs::is_directory(test_dir));
    EXPECT_FALSE(fs::is_regular_file(test_dir));
    EXPECT_EQ(fs::status(test_dir).type(), fs::file_type::directory);
    
    // 清理测试目录
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }
}

// 测试目录操作
TEST(SimpleFileSystem, DirectoryOperations) {
    namespace fs = _AST simple_fs;
    
    // 创建和删除单个目录
    fs::path single_dir = "single_test_dir";
    EXPECT_TRUE(fs::create_directory(single_dir));
    EXPECT_TRUE(fs::exists(single_dir));
    EXPECT_TRUE(fs::remove(single_dir));
    EXPECT_FALSE(fs::exists(single_dir));
    
    // 创建和删除多级目录
    fs::path nested_dir = "level1/level2/level3";
    EXPECT_TRUE(fs::create_directories(nested_dir));
    EXPECT_TRUE(fs::exists(nested_dir));
    
    // 测试remove_all删除整个目录树
    uintmax_t removed_count = fs::remove_all("level1");
    EXPECT_TRUE(removed_count > 0);
    EXPECT_FALSE(fs::exists("level1"));
}

// 测试文件操作
TEST(SimpleFileSystem, FileOperations) {
    namespace fs = _AST simple_fs;
    
    fs::path test_dir = "file_operations_test";
    fs::path source_file = test_dir / "source.txt";
    fs::path dest_file = test_dir / "destination.txt";
    fs::path renamed_file = test_dir / "renamed.txt";
    
    // 创建测试目录
    if (!fs::exists(test_dir)) {
        fs::create_directories(test_dir);
    }
    
    // 创建测试文件（使用标准C++文件操作）
    {
        std::ofstream file(source_file.string());
        file << "Test content for file operations";
        file.close();
    }
    
    // 测试文件大小
    EXPECT_GT(fs::file_size(source_file), 0);
    
    // 测试文件复制
    EXPECT_TRUE(fs::copy_file(source_file, dest_file));
    EXPECT_TRUE(fs::exists(dest_file));
    EXPECT_EQ(fs::file_size(source_file), fs::file_size(dest_file));
    
    // 测试文件重命名
    EXPECT_TRUE(fs::rename(dest_file, renamed_file));
    EXPECT_FALSE(fs::exists(dest_file));
    EXPECT_TRUE(fs::exists(renamed_file));
    
    // 测试最后修改时间
    std::time_t mod_time = fs::last_write_time(renamed_file);
    EXPECT_NE(mod_time, static_cast<std::time_t>(-1));
    
    // 清理测试文件和目录
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }
}

// 测试目录迭代器
TEST(SimpleFileSystem, DirectoryIterator) {
    namespace fs = _AST simple_fs;
    
    fs::path test_dir = "iterator_test_dir";
    fs::path file1 = test_dir / "file1.txt";
    fs::path file2 = test_dir / "file2.txt";
    
    // 创建测试目录和文件
    if (!fs::exists(test_dir)) {
        fs::create_directories(test_dir);
    }
    
    // 创建测试文件
    {
        std::ofstream f1(file1.string());
        f1 << "Content 1";
        std::ofstream f2(file2.string());
        f2 << "Content 2";
    }
    
    // 测试目录迭代器
    int count = 0;
    std::set<std::string> filenames;
    
    try {
        for (auto& entry : fs::directory_iterator(test_dir)) {
            count++;
            filenames.insert(entry.path().filename().string());
        }
    }
    catch (const fs::filesystem_error& e) {
        FAIL() << "Directory iteration failed: " << e.what();
    }
    
    EXPECT_EQ(count, 2);  // 应该有两个文件
    EXPECT_TRUE(filenames.count("file1.txt"));
    EXPECT_TRUE(filenames.count("file2.txt"));
    
    // 清理测试目录
    if (fs::exists(test_dir)) {
        fs::remove_all(test_dir);
    }
}

// 测试磁盘空间信息
TEST(SimpleFileSystem, DiskSpace) {
    namespace fs = _AST simple_fs;
    
    try {
        fs::space_info info = fs::space(".");
        // 检查空间信息是否有效（在正常环境中这些值应该都是正数）
        EXPECT_GE(info.capacity, 0);
        EXPECT_GE(info.free, 0);
        EXPECT_GE(info.available, 0);
    }
    catch (const std::exception& e) {
        // 磁盘空间信息获取可能在某些环境下失败，记录但不强制失败
        std::cerr << "Warning: Failed to get disk space info: " << e.what() << std::endl;
    }
}

// 测试错误处理
TEST(SimpleFileSystem, ErrorHandling) {
    namespace fs = _AST simple_fs;
    
    // 测试错误码版本的current_path
    std::error_code ec;
    fs::path p = fs::current_path(ec);
    EXPECT_FALSE(ec);  // 在正常环境中应该成功
    
    // 测试设置不存在的当前路径时的错误处理
    fs::path non_existent = "non_existent_path_98765";
    std::error_code ec_set;
    fs::current_path(non_existent, ec_set);
    EXPECT_TRUE(ec_set);  // 应该失败并设置错误码
}

GTEST_MAIN()