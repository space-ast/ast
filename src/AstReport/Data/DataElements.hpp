///
/// @file      DataElements.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-07-06
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

#pragma once

#include "AstGlobal.h"
#include "DataElement.hpp"
#include "AstUtil/Dimension.hpp"
#include "AstUtil/VariantVector.hpp"
#include <map>
#include <vector>
#include <string>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup 
    @{
*/


/// @brief 数据元素容器
class DataElements;

namespace detail
{
    /// @brief 生成成员变量提取函数
    /// @tparam T 数据类类型
    /// @tparam MemberType 成员变量类型
    /// @tparam Member 成员变量指针
    template<typename T, typename MemberType, MemberType T::*Member>
    ElementExtractFunc makeExtractFunc()
    {
        return [](const VariantVector& data, VariantVector& element) -> errc_t
        {
            auto span = data.asSpan<const T>();
            if (!span.data()) return -1;
            using ElementType = std::remove_cv_t<std::remove_reference_t<MemberType>>;
            element.reset<ElementType>(span.size());
            auto* out = element.as<ElementType>();
            if (!out) return -1;
            for (size_t i = 0; i < span.size(); ++i)
                out[i] = span[i].*Member;
            return 0;
        };
    }

    /// @brief 生成成员函数(getter)提取函数
    /// @tparam T 数据类类型
    /// @tparam Ret getter 返回值类型
    /// @tparam Getter const 成员函数指针
    template<typename T, typename Ret, Ret (T::*Getter)() const>
    ElementExtractFunc makeExtractFunc()
    {
        return [](const VariantVector& data, VariantVector& element) -> errc_t
        {
            auto span = data.asSpan<const T>();
            if (!span.data()) return -1;
            using ElementType = std::remove_cv_t<std::remove_reference_t<Ret>>;
            element.reset<ElementType>(span.size());
            auto* out = element.as<ElementType>();
            if (!out) return -1;
            for (size_t i = 0; i < span.size(); ++i)
                out[i] = (span[i].*Getter)();
            return 0;
        };
    }
} // namespace detail


/// @brief 数据元素容器
class DataElements
{
public:
    DataElements() = default;
    ~DataElements() = default;

    /// @brief 获取数据元素
    const DataElement* getElement(StringView name) const;
    const DataElement* find(StringView name) const{return getElement(name);}
    
    /// @brief 获取所有数据元素
    const std::vector<DataElement>& getElements() const { return elements_; }

    /// @brief 添加数据元素
    void addElement(const DataElement& element);

    /// @brief 添加别名
    /// @param alias 别名
    /// @param name 数据元素名称
    void addAlias(const char* alias, const char* name);

    /// @brief 添加数据元素（成员变量版本）
    /// @tparam T 数据类类型
    /// @tparam MemberType 成员变量类型
    /// @tparam Member 成员变量指针
    template<typename T, typename MemberType, MemberType T::*Member>
    void addElement(const char* name, Dimension dimension = Dimension::Unit())
    {
        DataElement element;
        element.setName(name);
        element.setDimension(dimension);
        element.setExtractFunc(detail::makeExtractFunc<T, MemberType, Member>());
        addElement(element);
    }

    /// @brief 添加数据元素（const 成员函数版本）
    /// @tparam T 数据类类型
    /// @tparam Ret getter 返回值类型
    /// @tparam Getter const 成员函数指针
    template<typename T, typename Ret, Ret (T::*Getter)() const>
    void addElement(const char* name, Dimension dimension = Dimension::Unit())
    {
        DataElement element;
        element.setName(name);
        element.setDimension(dimension);
        element.setExtractFunc(detail::makeExtractFunc<T, Ret, Getter>());
        addElement(element);
    }
    
private:
    std::vector<DataElement> elements_;                             ///< 数据元素列表
    std::vector<std::pair<std::string, std::string>> aliasMap_;     ///< 别名映射
};

const int i = sizeof(DataElements);

/*! @} */

AST_NAMESPACE_END
