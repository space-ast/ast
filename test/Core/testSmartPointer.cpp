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
#include "AstCore/ScopedPtr.hpp"
#include "AstCore/SharedPtr.hpp"
#include "AstCore/WeakPtr.hpp"
#include "AstUtil/IO.hpp"
#include "AstTest/AstTestMacro.h"
#include <memory>

AST_USING_NAMESPACE

TEST(SmartPointer, SharedPtr)
{
    Type* type = (Type*) 1; // only for test
    auto obj = new Object{ type };
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

TEST(SmartPointer, WeakPtr)
{
    Type* type = (Type*)1; // only for test
    {
        auto obj = new Object{ type };
        WeakPtr<Object> ptrweak = obj;
        EXPECT_EQ(obj->weakRefCount(), 2);
        obj->destruct();
        EXPECT_TRUE(ptrweak.get() == nullptr);
        EXPECT_EQ(obj->weakRefCount(), 1);
    }
    {
        WeakPtr<Object> ptrweak;
        {
            auto obj = new Object{ type };
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
            auto obj = new Object{ type };
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


TEST(SmartPointer, ScopedPtr)
{
    ScopedPtr<double> ptr{new double{1.0}};
    ptr = nullptr;
    Type* type = (Type*)1; // only for test
    Object *obj = new Object{type};
    {
        ScopedPtr<Object> ptr{obj};
    }
}


TEST(SmartPointer, FILE)
{
    // without scopedptr
    {
        const char* filepath = "testSmartPointer_FILE1.txt";
        const char* content = u8"testcontent_中文_😊😀_Русский контент";
        {
            ScopedPtr<std::FILE> file(fopen(filepath, "w"));
            fprintf(file, content);
        }
        {
            ScopedPtr<std::FILE> file(fopen(filepath, "r"));
            char buffer[1025]{};
            fread(buffer, 1024, 1, file);
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
            fprintf(file, content);
        }
        {
            std::FILE* file = fopen(filepath, "r");
            char buffer[1025]{};
            fread(buffer, 1024, 1, file);
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
            fwprintf(file, content);
        }
        {
            ScopedPtr<std::FILE> file(fopen(filepath, "r"));
            char buffer[1025]{};
            fread(buffer, 1024, 1, file);
            nothing();
        }
    }
}

GTEST_MAIN()
 
