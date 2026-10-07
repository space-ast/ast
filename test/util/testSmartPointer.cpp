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
 

#include "ast/Object.hpp"
#include "ast/StateCartesian.hpp"
#include "ast/HPOPForceModel.hpp"
#include "ast/ScopedPtr.hpp"
#include "ast/SharedPtr.hpp"
#include "ast/WeakPtr.hpp"
#include "ast/IO.hpp"
#include "ast/AstTestMacro.h"
#include <memory>

AST_USING_NAMESPACE

// 是否在 AddressSanitizer 下构建。
// 栈对象离开作用域后，智能指针中仍保留指向该栈内存的指针，
// 再去读取它（哪怕是判断是否已析构）属于 use-after-scope，
// ASan 会直接报错，因此下面演示"不要用智能指针管理栈对象"的用例在 ASan 下跳过。
#if defined(__SANITIZE_ADDRESS__)
#  define _AST_TEST_WITH_ASAN 1
#elif defined(__has_feature)
#  if __has_feature(address_sanitizer)
#    define _AST_TEST_WITH_ASAN 1
#  endif
#endif
#ifndef _AST_TEST_WITH_ASAN
#  define _AST_TEST_WITH_ASAN 0
#endif

// 测试SharedPtr
TEST(SmartPointer, SharedPtr)
{
    auto obj = new Referenced{  };
    {
        SharedPtr<Referenced> ptr = obj;
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
        auto obj = new Referenced{  };
        WeakPtr<Referenced> ptrweak = obj;
        EXPECT_EQ(obj->weakRefCount(), 2);
        obj->destruct();
        EXPECT_TRUE(ptrweak.get() == nullptr);
        EXPECT_EQ(obj->weakRefCount(), 1);
    }
    {
        WeakPtr<Referenced> ptrweak;
        {
            auto obj = new Referenced{  };
            ptrweak = obj;
            EXPECT_EQ(obj->weakRefCount(), 2);
            obj->destruct();
            EXPECT_TRUE(ptrweak.get() == nullptr);
            EXPECT_EQ(obj->weakRefCount(), 1);
        }
    }
    {
        WeakPtr<Referenced> ptrweak;
        {
            auto obj = new Referenced{  };
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
    Referenced *obj = new Referenced{};
    {
        ScopedPtr<Referenced> ptr{obj};
    }
}

// 测试栈上的对象
TEST(SmartPointer, StackObject_SharedPtr)
{
    if (_AST_TEST_WITH_ASAN)
        GTEST_SKIP();  // 访问已析构的栈对象是 use-after-scope
    {
        SharedPtr<Class> ptr;
        {
            Class obj;
            ptr = &obj;
        }
        if(auto p = ptr.get())
            std::printf("refCount: %d\n", p->refCount());
    }
    {
        SharedPtr<Class> ptr;
        {
            Class obj;
            ptr = &obj;
            ptr.reset();
        }
        if(auto p = ptr.get())
            std::printf("refCount: %d\n", p->refCount());
    }
}



// 测试栈上的对象
TEST(SmartPointer, StackObject_WeakPtr)
{
    if (_AST_TEST_WITH_ASAN)
        GTEST_SKIP();  // 判断栈对象是否已析构需要读取已失效的栈内存
    {
        WeakPtr<StateCartesian> ptrweak;
        {
            StateCartesian obj;
            ptrweak = &obj;
        }
        auto ptr = ptrweak.get();
        EXPECT_TRUE(ptr == nullptr);
    }
    {
        WeakPtr<StateCartesian> ptrweak;
        {
            StateCartesian obj;
            ptrweak = &obj;
            ptrweak.reset();
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
            ScopedPtr<std::FILE> file(std::fopen(filepath, "w"));
            std::fprintf(file, "%s", content);
        }
        {
            ScopedPtr<std::FILE> file(std::fopen(filepath, "r"));
            char buffer[1025]{};
            size_t size = std::fread(buffer, 1, 1024, file);
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
            std::FILE* file = std::fopen(filepath, "w");
            std::fprintf(file, "%s", content);
        }
        {
            std::FILE* file = std::fopen(filepath, "r");
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
            ScopedPtr<std::FILE> file(std::fopen(filepath, "w"));
            std::fwprintf(file, content);
        }
        {
            ScopedPtr<std::FILE> file(std::fopen(filepath, "r"));
            char buffer[1025]{};
            size_t size = std::fread(buffer, 1, 1024, file);
            ast_printf("%s\n", buffer);
            EXPECT_TRUE(size != 0);
        }
    }
}

GTEST_MAIN()
 
