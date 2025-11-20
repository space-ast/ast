/// @file      testSmartPointer.cpp
/// @brief     
/// @details   ~
/// @author    jinke18
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
#include "AstTest/AstTestMacro.h"
#include <memory>

AST_USING_NAMESPACE

int testSharedPtr()
{
    Type* type = (Type*) 1; // only for test
    auto obj = new Object{ type };
    {
        SharedPtr<Object> ptr = obj;
        ASSERT_EQ(ptr->refCount(), 1);
        {
            auto ptr2 = ptr;
            ASSERT_EQ(ptr2->refCount(), 2);
        }
        ASSERT_EQ(ptr->refCount(), 1);
    }
    return 0;
}

int testWeakPtr()
{
    Type* type = (Type*)1; // only for test
    {
        auto obj = new Object{ type };
        WeakPtr<Object> ptrweak = obj;
        ASSERT_EQ(obj->weakRefCount(), 2);
        obj->destruct();
        ASSERT_TRUE(ptrweak.get() == nullptr);
        ASSERT_EQ(obj->weakRefCount(), 1);
    }
    {
        WeakPtr<Object> ptrweak;
        {
            auto obj = new Object{ type };
            ptrweak = obj;
            ASSERT_EQ(obj->weakRefCount(), 2);
            obj->destruct();
            ASSERT_TRUE(ptrweak.get() == nullptr);
            ASSERT_EQ(obj->weakRefCount(), 1);
        }
    }
    {
        WeakPtr<Object> ptrweak;
        {
            auto obj = new Object{ type };
            ptrweak = obj;
            ASSERT_EQ(obj->weakRefCount(), 2);
            auto ptrweak2 = ptrweak;
            ASSERT_EQ(obj->weakRefCount(), 3);
            ptrweak2 = nullptr;
            obj->destruct();
            ASSERT_TRUE(ptrweak.get() == nullptr);
            ASSERT_EQ(obj->weakRefCount(), 1);
        }
    }
    return 0;
}


int testScopedPtr()
{
    ScopedPtr<double> ptr = new double{1.0};
    ptr = nullptr;
    Type* type = (Type*)1; // only for test
    Object *obj = new Object{type};
    {
        ScopedPtr<Object> ptr{obj};
    }
    return 0;
}

int main()
{
    int rc = 0 ;
    rc |= testSharedPtr();
    rc |= testScopedPtr();
    rc |= testWeakPtr();
    return rc;
}
 
 
