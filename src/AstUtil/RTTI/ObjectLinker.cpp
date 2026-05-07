///
/// @file      ObjectLinker.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-01
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

#include "ObjectLinker.hpp"
#include <memory>
#include <vector>

AST_NAMESPACE_BEGIN


// begin declaration


class ObjectLinkerManager
{
public:
    ObjectLinkerManager() = default;

    ~ObjectLinkerManager() = default;

    static ObjectLinkerManager& Instance();

    /// @brief 解析所有对象的延迟链接
    errc_t resolveForAllObject();

    /// @brief 解析对象的延迟链接
    /// @details 解析对象的延迟链接
    /// @return errc_t 错误码
    errc_t resolveForObject(Object* object);

    /// @brief 添加对象延迟链接
    void addObjectLinker(ObjectLinker* linker);
protected:
    void eraseResolvedLinkers();
public:
    std::vector<std::unique_ptr<ObjectLinker>> linkers_;
};


// begin implementation


ObjectLinkerManager& ObjectLinkerManager::Instance()
{
    static ObjectLinkerManager instance;
    return instance;
}

errc_t ObjectLinkerManager::resolveForAllObject()
{
    for (auto& linker : linkers_)
    {
        if(linker == nullptr)
        {
            continue;
        }
        errc_t rc = linker->resolve();
        if (rc == 0)
        {
            linker.reset(nullptr);
        }
    }
    eraseResolvedLinkers();
    return 0;
}

errc_t ObjectLinkerManager::resolveForObject(Object *object)
{
    for (auto& linker : linkers_)
    {
        if(linker == nullptr)
        {
            continue;
        }
        if(object != linker->object())
        {
            continue;
        }
        errc_t rc = linker->resolve();
        if (rc == 0)
        {
            linker.reset(nullptr);
        }
    }
    eraseResolvedLinkers();
    return 0;
}

void ObjectLinkerManager::eraseResolvedLinkers()
{
    for (auto iter = linkers_.begin(); iter != linkers_.end(); ) 
    {
        auto linker = iter->get();
        if (linker == nullptr || linker->object() == nullptr)
        {
            iter = linkers_.erase(iter);  
        }
        else
        {
            ++iter;
        }
    }
}

void ObjectLinkerManager::addObjectLinker(ObjectLinker* linker)
{
    linkers_.push_back(std::unique_ptr<ObjectLinker>(linker));
}

void aAddObjectLinker(ObjectLinker* linker)
{
    ObjectLinkerManager::Instance().addObjectLinker(linker);
}


errc_t aObject_ResolveLinks(Object* object)
{
    return ObjectLinkerManager::Instance().resolveForObject(object);
}

errc_t aResolveAllLinks()
{
    return ObjectLinkerManager::Instance().resolveForAllObject();
}



AST_NAMESPACE_END
