///
/// @file      SpiceAPI.hpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-22
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
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

#pragma once

#include "AstGlobal.h"
#include <mutex>
#include <vector>
#include <string>
#include <array>
#include <cstdint>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Data
    @{
*/

/// @brief Spice API
/// @details 处理CSPICE库的动态加载、卸载、函数调用等
/// 这里只对CSPICE库进行简单的包装：
/// - 在保证接口不变的情况下，确保线程安全
/// - 针对文件加载增加了引用计数机制
/// - 处理了文件路径的编码问题
/// @warning 注意CSPICE库的默认长度单位是**千米**
class AST_CORE_API SpiceAPI
{
public:
    // 函数索引
    enum{
        ifurnsh = 0,
        ispkgeo,
        ispklef,
        ispkuef,
        iktotal,
        ibodc2n,
        ierract,
        ifailed,
        ireset,
        ikclear,
        numfunctions,
    };
    using funcarray = std::array<void*, numfunctions>;

    /// @brief 获取单例实例
    /// @return 单例实例指针
    static SpiceAPI* Instance();

    /// @brief 默认构造函数
    SpiceAPI() = default;

    /// @param shouldLoadDynamicLib 是否自动加载动态库
    explicit SpiceAPI(bool shouldLoadDynamicLib);
    
    ~SpiceAPI();

    /// @brief 
    /// @param libpath 库路径
    /// @return 错误码
    errc_t load(StringView libpath);

    /// @brief 尝试加载库
    /// @details 尝试加载库，指定多个可能的库路径，直到成功加载为止。
    /// @param libpaths 库路径列表
    /// @return 错误码
    errc_t tryload(const std::vector<std::string>& libpaths);

    /// @brief 卸载库
    /// @return 错误码
    errc_t unload();

    /// @brief 检查是否加载了库
    /// @return 是否加载了库
    bool isLoaded() const { return library_ != nullptr; }
public: // 包装函数

    /// @brief 加载spice内核文件
    /// @param file 内核文件路径
    /// @return 错误码
    errc_t furnsh(const char* file);

    /// @brief 计算spice位置
    /// @param targ 目标体ID
    /// @param et 时间，相对于J2000.0 TDB的秒数，并以TDB为时间递进尺度
    /// @param ref 参考系
    /// @param obs 观察者ID
    /// @param state 状态向量
    /// @param lt 光行时
    /// @return 错误码
    errc_t spkgeo(
        int            targ,
        double         et,
        const char    * ref,
        int            obs,
        double         state[6],
        double       * lt      
    );

    /// @brief 加载SPK内核文件
    /// @details 在CSPICE的接口基础上增加了引用计数机制
    /// @param filename 内核文件路径
    /// @param handle 内核句柄
    /// @return 错误码
    errc_t spklef(const char* filename, int* handle);

    /// @brief 鞋子SPK内核文件
    /// @details 在CSPICE的接口基础上增加了引用计数机制
    /// @param handle 内核句柄
    /// @return 错误码
    errc_t spkuef(int handle);

        /// @brief 获取已加载的内核数量
    /// @param kind 内核类型
    /// @param count 内核数量
    /// @return 错误码
    errc_t ktotal(const char * kind, int* count);
    int ktotal(const char * kind){
        int count = 0;
        ktotal(kind, &count);
        return count;
    }

    /// @brief 通过天体ID获取天体名称
    void bodc2n(int            code,
                int            namlen,
                char           * name,
                bool           * found);
    errc_t bodc2n(int            code, std::string&    name);

    /// @brief 检查是否发生错误
    /// @return 是否发生错误
    bool failed();

    /// @brief 重置错误状态
    void reset();

    /// @brief 错误处理函数
    /// @param action 错误处理操作
    void erract(const char* operation, int lenout, char* action);

    /// @brief 清除所有已加载的内核
    void kclear();
protected:
    errc_t checkerror();
    A_DISABLE_COPY(SpiceAPI);
protected:
    void*  library_{nullptr};           ///< 库句柄
    std::vector<uint32_t> spk_handles_; ///< 已加载的SPK内核句柄列表
    funcarray functions_{};             ///< 函数指针
    std::mutex mutex_;                  ///< 互斥锁(CSPICE库的函数不是线程安全的，这里用于保护函数调用的线程安全问题)
};



/*! @} */

AST_NAMESPACE_END
