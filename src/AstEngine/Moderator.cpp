///
/// @file      Moderator.cpp
/// @brief
/// @details
/// @author    axel
/// @date      2026-09-08
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
///
///    http://www.apache.org/licenses/LICENSE-2.0
///
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Moderator.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/Logger.hpp"
#include <cstdlib>
#include <type_traits>

AST_NAMESPACE_BEGIN

namespace {
std::aligned_storage<sizeof(Moderator), alignof(Moderator)>::type buf;  // 单例存储区
bool singletonDestroyed = false;                                        // 单例是否已销毁
struct SingletonGuard {
    SingletonGuard()
    {
        new(&buf) Moderator;
        singletonDestroyed = false;
    }
    ~SingletonGuard()
    {
        reinterpret_cast<Moderator*>(&buf)->~Moderator();
        singletonDestroyed = true;
    }
};
}

Moderator& Moderator::Instance()
{
    static SingletonGuard guard;

    // 采用凤凰单例模式安全承接调用，避免 use-after-destroy。
    if (A_UNLIKELY(singletonDestroyed))
    {
        new(&buf) Moderator;
        singletonDestroyed = false;
        std::atexit([]() {
            if (!singletonDestroyed)
            {
                reinterpret_cast<Moderator*>(&buf)->~Moderator();
                singletonDestroyed = true;
            }
        });
    }
    return *reinterpret_cast<Moderator*>(&buf);
}

Object* Moderator::addObject(StringView type, Object* parent)
{
    auto newObject = aNewObject(type, parent);
    if (newObject == nullptr)
    {
        aWarning(_("创建类型为 '%.*s' 的对象失败"), type.size(), type.data());
        return nullptr;
    }
    observers_.onObjectAdded(*newObject);
    return newObject;
}

errc_t Moderator::removeObject(Object& object)
{
    // 保持对象引用计数，避免在事件回调中对象被销毁导致悬垂指针
    SharedPtr<Object> objectHolder(&object);
    observers_.onBeforeObjectRemove(object);
    aRemoveObject(&object);
    observers_.onObjectRemoved(object);
    return eNoError;
}

errc_t Moderator::renameObject(Object& object, StringView name)
{
    // 拷贝，避免 setName 重分配字符串后 oldName 指向失效缓冲（悬垂）
    std::string oldName = object.getName();
    observers_.onBeforeObjectRename(object, oldName, name);
    object.setName(name);
    observers_.onObjectRenamed(object, oldName, name);
    return eNoError;
}

AST_NAMESPACE_END
