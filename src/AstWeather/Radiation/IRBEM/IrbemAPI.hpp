///
/// @file      IrbemAPI.hpp
/// @brief     IRBEM 动态库加载与包装
/// @details   参考 SpiceAPI 的单例模式，封装 gfortran 编译的 IRBEM 共享库。
/// @author    axel
/// @date      2026-07-30
/// @copyright 版权所有 (C) 2026-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
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
#include "AstUtil/StringView.hpp"
#include <mutex>
#include <array>
#include <string>
#include <vector>

AST_NAMESPACE_BEGIN

/*!
    @addtogroup Magnetosphere
    @{
*/

/// @brief IRBEM API 包装类
/// @details 处理 IRBEM 共享库的动态加载、卸载与函数调用。
/// 采用凤凰单例模式，确保在静态析构后仍可安全重建。
/// @warning IRBEM 库内部使用 Fortran COMMON 块（全局可变状态），
///          涉及 COMMON 块的方法内部加锁保护，不可重入。
class AST_WEATHER_API IrbemAPI
{
public:
    enum FuncIndex {
        kIrbemFortranVersion, kGetIrbemNtimeMax,
        kMakeLstar, kGetField,
        kFlyInAfrlCrres, kGetCrresFlux, kNumFunctions
    };
    using FuncArray = std::array<void*, kNumFunctions>;

    /// @brief 获取单例实例
    static IrbemAPI* Instance();

    IrbemAPI() = default;

    /// @param shouldLoad 是否在构造时自动加载默认库
    explicit IrbemAPI(bool shouldLoad);

    ~IrbemAPI();

    /// @brief 加载指定路径的 IRBEM 共享库
    /// @param libPath 库路径（不含后缀）
    /// @return 错误码
    errc_t load(StringView libPath);

    /// @brief 尝试从多个路径加载，直到成功
    /// @param libPaths 候选库路径列表
    /// @return 错误码
    errc_t tryload(const std::vector<std::string>& libPaths);

    /// @brief 卸载已加载的库
    errc_t unload();

    /// @brief 检查库是否已加载
    bool isLoaded() const { return library_ != nullptr; }

public:
    // ---- 纯常量查询，不加锁 ----

    /// @brief 获取 IRBEM 库的 Fortran 源码版本号
    /// @details 返回 Makefile 构建时由 git rev-list --count 生成的版本号。
    /// @param[out] version 版本号
    errc_t irbem_fortran_version1(int* version);

    /// @brief 获取数组的最大时间维度
    /// @details 返回 IRBEM 库内部所有时间数组的最大长度 (ntime_max)，
    ///          由 ntime_max.inc 在编译时确定。
    /// @param[out] ntime_max 最大时间点数
    errc_t get_irbem_ntime_max1(int* ntime_max);

    // ---- 涉及 COMMON 块共享状态，内部加锁 ----

    /// @brief 计算 L* 及相关磁壳参数
    /// @details 根据给定的位置和时间，使用指定的内外磁场模型计算磁壳参数。
    ///          内部调用 IGRF 和外部场模型，共享 COMMON 块状态。
    /// @param[in]  ntime   时间点数量
    /// @param[in]  kext    外部磁场模型选择（0-14，=0 仅 IGRF 内源场）
    /// @param[in]  options 计算选项数组（长度 5）
    ///                      options(1): L* 计算方法
    ///                      options(2): 磁力线追踪步长控制
    ///                      options(3): 积分步数上限
    ///                      options(4): 收敛判据
    ///                      options(5): 保留
    /// @param[in]  sysaxes 输入坐标系统（1=GEO, 2=GSE, 3=GSM, 4=SM, 5=GEI）
    /// @param[in]  iyear   年份数组（公元纪年），长度 ntime
    /// @param[in]  idoy    年积日数组（1-366），长度 ntime
    /// @param[in]  UT      世界时数组 [小时]，长度 ntime
    /// @param[in]  x1, x2, x3 输入位置坐标数组，长度 ntime
    /// @param[in]  maginput 磁场模型输入参数数组（长度 25 × ntime），包含太阳风动压、
    ///                      Dst 指数、IMF By/Bz 等外部场模型所需参数
    /// @param[out] Lm      McIlwain L-shell，长度 ntime
    /// @param[out] Lstar   Roederer L* 参数，长度 ntime
    /// @param[out] Blocal  当地磁场强度 [nT]，长度 ntime
    /// @param[out] Bmin    磁赤道最小磁场强度 [nT]，长度 ntime
    /// @param[out] J       第二绝热不变量 [Re² nT⁰·⁵]，长度 ntime
    /// @param[out] MLT     磁地方时 [小时]，长度 ntime
    errc_t make_lstar1(int ntime, int kext, const std::array<int, 5>& options, int sysaxes,
                       const int* iyear, const int* idoy, const double* UT,
                       const double* x1, const double* x2, const double* x3,
                       const double* maginput,
                       double* Lm, double* Lstar, double* Blocal,
                       double* Bmin, double* J, double* MLT);

    /// @brief 计算空间中某点的地磁场矢量与强度（单点版本）
    /// @details 使用 IGRF 内源场和所选外部场模型，计算指定位置的地磁场。
    ///          内部共享 IGRF COMMON 块状态。
    /// @param[in]  kext    外部磁场模型选择（=0 仅 IGRF 内源场）
    /// @param[in]  options 计算选项数组（长度 5）
    /// @param[in]  sysaxes 输入坐标系统（1=GEO, 2=GSE, 3=GSM, 4=SM, 5=GEI）
    /// @param[in]  iyear   年份（公元纪年）
    /// @param[in]  idoy    年积日 (1-366)
    /// @param[in]  UT      世界时 [小时]
    /// @param[in]  x1, x2, x3 输入位置坐标
    /// @param[in]  maginput 磁场模型输入参数数组（长度 25）
    /// @param[out] Bgeo    GEO 坐标系下的磁场三分量 [nT]（长度为 3）
    /// @param[out] B       磁场总强度 [nT]
    errc_t get_field1(int kext, const std::array<int, 5>& options, int sysaxes,
                      int iyear, int idoy, double UT,
                      double x1, double x2, double x3,
                      const double* maginput,
                      double Bgeo[3], double& B);

    /// @brief 沿轨道在 AFRL CRRES 辐射带模型中飞行计算通量
    /// @details 内部根据位置自动计算 L 和 B/B₀（调用 calcul_Lstar_opt），
    ///          然后调用 get_crres_flux 完成通量插值。共享 CRRES COMMON 块。
    /// @param[in]  ntime   要计算的时间点数量
    /// @param[in]  sysaxes 输入坐标系统:
    ///                      1 = GEO（地理）
    ///                      2 = GSE（地心太阳黄道）
    ///                      3 = GSM（地心太阳磁层）
    ///                      4 = SM （太阳磁层）
    ///                      5 = GEI（地心赤道惯性）
    /// @param[in]  whichm  模型选择:
    ///                      1 = CRRESPRO Quiet（质子，宁静期）
    ///                      2 = CRRESPRO Active（质子，活跃期）
    ///                      3 = CRRESELE Average（电子，平均）
    ///                      4 = CRRESELE Worst Case（电子，最劣情况）
    ///                      5 = CRRESELE + Ap15 分段（电子，按地磁活动分 6 档）
    /// @param[in]  whatf   通量类型: 1 = 微分通量, 2 = 能段通量, 3 = 积分通量
    /// @param[in]  nene    要计算的能量通道数
    /// @param[in]  energy  能量数组 (MeV)，维度 [2, nene]:
    ///                     energy[0][i] = 通道 i 的下界能量
    ///                     energy[1][i] = 通道 i 的上界能量（whatf=1 时与下界相同）
    /// @param[in]  iyear   年份数组（公元纪年），长度 ntime
    /// @param[in]  idoy    年积日数组（1-366），长度 ntime
    /// @param[in]  UT      世界时数组 [小时]，长度 ntime
    /// @param[in]  x1, x2, x3 输入坐标数组，长度 ntime
    /// @param[in]  Ap15    前 15 天 Ap 指数平均值数组，长度 ntime
    /// @param[in]  ascii_path 数据文件目录路径
    /// @param[out] flux    输出通量数组，维度 [ntime, 25]
    ///                     微分通量 (whatf=1) 单位: MeV⁻¹ cm⁻² s⁻¹
    ///                     能段/积分通量 (whatf=2,3) 单位: cm⁻² s⁻¹
    ///                     超出有效范围的点赋值为 baddata
    errc_t fly_in_afrl_crres1(int ntime, int sysaxes,
                              int whichm, int whatf, int nene,
                              const double* energy,
                              const int* iyear, const int* idoy, const double* UT,
                              const double* x1, const double* x2, const double* x3,
                              const double* Ap15, double* flux,
                              StringView ascii_path);

    /// @brief 根据 B/B₀ 和 L 值直接计算 CRRES 辐射带通量
    /// @details 不进行位置→L 转换，直接由 B/B₀ 和 L 在对数空间内插值计算通量。
    ///          内部按需调用 Init_CRRES 从数据文件加载模型系数。
    /// @param[in]  ntime   要计算的时间点数量
    /// @param[in]  whichm  模型选择:
    ///                      1 = CRRESPRO Quiet（质子，宁静期）
    ///                      2 = CRRESPRO Active（质子，活跃期）
    ///                      3 = CRRESELE Average（电子，平均）
    ///                      4 = CRRESELE Worst Case（电子，最劣情况）
    ///                      5 = CRRESELE + Ap15 分段（电子，按地磁活动分 6 档）
    /// @param[in]  whatf   通量类型: 1 = 微分通量, 2 = 能段通量, 3 = 积分通量
    /// @param[in]  nene    要计算的能量通道数
    /// @param[in]  energy  能量数组 (MeV)，维度 [2, nene]
    /// @param[in]  BBo     B_local/B_equator 数组，长度 ntime
    /// @param[in]  L       McIlwain L-shell 数组，长度 ntime
    /// @param[in]  Ap15    前 15 天 Ap 指数平均值数组，长度 ntime
    /// @param[in]  ascii_path 数据文件目录路径
    /// @param[out] flux    输出通量数组，维度 [ntime, 25]
    ///                     微分通量 (whatf=1) 单位: MeV⁻¹ cm⁻² s⁻¹
    ///                     能段/积分通量 (whatf=2,3) 单位: cm⁻² s⁻¹
    ///                     超出有效范围的点赋值为 baddata
    errc_t get_crres_flux(int ntime, int whichm, int whatf,
                          int nene, const double* energy,
                          const double* BBo, const double* L, const double* Ap15,
                          double* flux, StringView ascii_path);

protected:
    A_DISABLE_COPY(IrbemAPI);

    void*       library_{nullptr};
    FuncArray   functions_{};
    std::mutex  mutex_{};
};

/*! @} */

AST_NAMESPACE_END
