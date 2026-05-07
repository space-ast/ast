/// @file      testSmartPointer.cpp
/// @brief     
/// @details   ~
/// @author    axel
/// @date      20.11.2025
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
 

#include "AstCore/Object.hpp"
#include "AstCore/StateCartesian.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstCore/ScopedPtr.hpp"
#include "AstCore/SharedPtr.hpp"
#include "AstCore/WeakPtr.hpp"
#include "AstUtil/IO.hpp"
#include "AstTest/AstTestMacro.h"
#include <memory>

AST_USING_NAMESPACE

// 测试SharedPtr
TEST(SmartPointer, SharedPtr)
{
    auto obj = new ObjectNamed{  };
    {
        SharedPtr<Object> ptr = obj;
        EXPECT_EQ(ptr->refCount(), 1);
        {
            auto ptr2 = ptr;
            EXPECT_EQ(ptr2->refCount(), 2);
        }
        EXPECT_EQ(ptr->refCount(), 1);
    }
}

// 测试WeakPtr
TEST(SmartPointer, WeakPtr)
{
    {
        auto obj = new ObjectNamed{  };
        WeakPtr<Object> ptrweak = obj;
        EXPECT_EQ(obj->weakRefCount(), 2);
        obj->destruct();
        EXPECT_TRUE(ptrweak.get() == nullptr);
        EXPECT_EQ(obj->weakRefCount(), 1);
    }
    {
        WeakPtr<Object> ptrweak;
        {
            auto obj = new ObjectNamed{  };
            ptrweak = obj;
            EXPECT_EQ(obj->weakRefCount(), 2);
            obj->destruct();
            EXPECT_TRUE(ptrweak.get() == nullptr);
            EXPECT_EQ(obj->weakRefCount(), 1);
        }
    }
    {
        WeakPtr<Object> ptrweak;
        {
            auto obj = new ObjectNamed{  };
            ptrweak = obj;
            EXPECT_EQ(obj->weakRefCount(), 2);
            auto ptrweak2 = ptrweak;
            EXPECT_EQ(obj->weakRefCount(), 3);
            ptrweak2 = nullptr;
            obj->destruct();
            EXPECT_TRUE(ptrweak.get() == nullptr);
            EXPECT_EQ(obj->weakRefCount(), 1);
        }
    }
}

// 测试ScopedPtr
TEST(SmartPointer, ScopedPtr)
{
    ScopedPtr<double> ptr{new double{1.0}};
    ptr = nullptr;
    ObjectNamed *obj = new ObjectNamed{};
    {
        ScopedPtr<Object> ptr{obj};
    }
}


// 测试栈上的对象
TEST(SmartPointer, StackObject)
{
    {
        WeakPtr<StateCartesian> ptrweak;
        {
            StateCartesian obj;
            ptrweak = &obj;
        }
        auto ptr = ptrweak.get();
        EXPECT_TRUE(ptr == nullptr);
    }
    // 测试成员变量
    {
        WeakPtr<Object> ptrweak;
        {
            HPOPForceModel obj;
            ptrweak = &obj.drag();
        }
        auto ptr = ptrweak.get();
        EXPECT_TRUE(ptr == nullptr);
    }
    // 测试成员变量
    {
        WeakPtr<Object> ptrweak;
        HPOPForceModel obj;
        ptrweak = &obj.gravity();
        obj.pointMass();
        auto ptr = ptrweak.get();
        EXPECT_TRUE(ptr == nullptr);
    }
    // 测试成员变量
    {
        WeakPtr<Object> ptrweak;
        HPOPForceModel obj;
        ptrweak = &obj.pointMass();
        auto ptr = ptrweak.get();
        EXPECT_TRUE(ptr != nullptr);
    }

}

// 测试文件
TEST(SmartPointer, FILE)
{
    // with scopedptr
    {
        const char* filepath = "testSmartPointer_FILE1.txt";
        const char* content = u8"testcontent_中文_😊😀_Русский контент";
        {
            ScopedPtr<std::FILE> file(fopen(filepath, "w"));
            fprintf(file, "%s", content);
        }
        {
            ScopedPtr<std::FILE> file(fopen(filepath, "r"));
            char buffer[1025]{};
            size_t size = fread(buffer, 1, 1024, file);
            ast_printf("%s\n", buffer);
            EXPECT_TRUE(size != 0);
            int eq = strcmp(buffer, content);
            EXPECT_EQ(eq, 0);
        }
    }
    // without scopedptr
    {
        const char* filepath = "testSmartPointer_FILE2.txt";
        const char* content = u8"testcontent_中文_😊_Русский контент";
        {
            std::FILE* file = fopen(filepath, "w");
            fprintf(file, "%s", content);
        }
        {
            std::FILE* file = fopen(filepath, "r");
            char buffer[1025]{'\0'};
            size_t size = fread(buffer, 1, 1024, file);
            ast_printf("%s\n", buffer);
            EXPECT_TRUE(size == 0);
            size_t len = strlen(buffer);
            EXPECT_EQ(len, 0);
            int eq = strcmp(buffer, content);
            EXPECT_NE(eq, 0);
        }
    }
    // wchar_t
    {
        const char* filepath = "testSmartPointer_FILE3.txt";
        const wchar_t* content = L"testcontent_中文_😊_Русский контент";
        {
            ScopedPtr<std::FILE> file(fopen(filepath, "w"));
            posix::fwprintf(file, content);
        }
        {
            ScopedPtr<std::FILE> file(fopen(filepath, "r"));
            char buffer[1025]{};
            size_t size = fread(buffer, 1, 1024, file);
            ast_printf("%s\n", buffer);
            EXPECT_TRUE(size != 0);
        }
    }
}

GTEST_MAIN()
 
