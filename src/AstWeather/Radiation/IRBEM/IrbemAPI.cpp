///
/// @file      IrbemAPI.cpp
/// @brief     IRBEM 动态库加载与包装实现
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

#include "IrbemAPI.hpp"

extern "C" {
#include "libirbem.h"
}

#include <cstdlib>              // for atexit
#include <type_traits>          // for aligned_storage

#include "AstUtil/LibraryLoader.hpp"
#include "AstUtil/Logger.hpp"


AST_NAMESPACE_BEGIN

// ---- 辅助函数 ----

static int funcArrayLoadedCount(const IrbemAPI::FuncArray& funcs)
{
    int count = 0;
    for (auto& f : funcs)
        if (f)
            count++;
    return count;
}

namespace {

// ---- 凤凰单例存储 ----

std::aligned_storage<sizeof(IrbemAPI), alignof(IrbemAPI)>::type sIrbemBuf;
bool sIrbemDestroyed = false;

struct SingletonGuard {
    SingletonGuard() {
        new (&sIrbemBuf) IrbemAPI(true);
        sIrbemDestroyed = false;
    }
    ~SingletonGuard() {
        reinterpret_cast<IrbemAPI*>(&sIrbemBuf)->~IrbemAPI();
        sIrbemDestroyed = true;
    }
};

} // anonymous namespace

// ---- 单例访问 ----

IrbemAPI* IrbemAPI::Instance()
{
    static SingletonGuard guard;

    // 凤凰单例：析构后重建
    if (A_UNLIKELY(sIrbemDestroyed))
    {
        new (&sIrbemBuf) IrbemAPI(false);
        sIrbemDestroyed = false;
        std::atexit([]() {
            if (!sIrbemDestroyed)
            {
                reinterpret_cast<IrbemAPI*>(&sIrbemBuf)->~IrbemAPI();
                sIrbemDestroyed = true;
            }
        });
    }
    return reinterpret_cast<IrbemAPI*>(&sIrbemBuf);
}

// ---- 构造 / 析构 ----

IrbemAPI::IrbemAPI(bool shouldLoad)
    : library_(nullptr)
    , functions_{}
    , mutex_()
{
    if (shouldLoad)
    {
        // 尝试加载平台相关的默认库名
        tryload({
            "libirbem.Win64.gfortran64",
            "libirbem.linux64.gfortran64",
            "libirbem",
        });
    }
}

IrbemAPI::~IrbemAPI()
{
    unload();
}

// ---- 动态库加载 ----

errc_t IrbemAPI::load(StringView libPath)
{
    void* lib = aLoadLibrary(std::string(libPath).c_str());
    if (!lib)
    {
        aError("IrbemAPI: failed to load library '%.*s'",
               libPath.size(), libPath.data());
        return eErrorInvalidFile;
    }

    FuncArray funcs{};
    funcs[kIrbemFortranVersion] = aGetProcAddress(lib, "irbem_fortran_version1_");
    funcs[kGetIrbemNtimeMax]    = aGetProcAddress(lib, "get_irbem_ntime_max1_");
    funcs[kMakeLstar]           = aGetProcAddress(lib, "make_lstar1_");
    funcs[kGetField]            = aGetProcAddress(lib, "get_field1_");
    funcs[kFlyInAfrlCrres]      = aGetProcAddress(lib, "fly_in_afrl_crres1_");
    funcs[kGetCrresFlux]        = aGetProcAddress(lib, "get_crres_flux_");

    int numLoaded = funcArrayLoadedCount(funcs);
    if (numLoaded < kNumFunctions)
    {
        aError("IrbemAPI: failed to load all %d functions, only %d loaded",
               kNumFunctions, numLoaded);
        aFreeLibrary(lib);
        return eErrorInvalidFile;
    }

    // 加锁保护 library_/functions_ 的写入
    {
        std::lock_guard<std::mutex> lock(mutex_);
        library_   = lib;
        functions_ = funcs;
    }

    return eNoError;
}

errc_t IrbemAPI::tryload(const std::vector<std::string>& libPaths)
{
    for (const auto& p : libPaths)
    {
        if (load(p) == eNoError)
            return eNoError;
    }
    return eErrorInvalidFile;
}

errc_t IrbemAPI::unload()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (library_)
    {
        aFreeLibrary(library_);
        library_   = nullptr;
        functions_ = FuncArray{};
    }
    return eNoError;
}

// ---- 纯常量查询（不加锁） ----

errc_t IrbemAPI::irbem_fortran_version1(int* version)
{
    using FnType = decltype(&irbem_fortran_version1_);
    auto fn = reinterpret_cast<FnType>(functions_[kIrbemFortranVersion]);
    if (!fn)
    {
        aError("IrbemAPI: library not loaded");
        return eErrorNullPtr;
    }
    fn(version);
    return eNoError;
}

errc_t IrbemAPI::get_irbem_ntime_max1(int* ntime_max)
{
    using FnType = decltype(&get_irbem_ntime_max1_);
    auto fn = reinterpret_cast<FnType>(functions_[kGetIrbemNtimeMax]);
    if (!fn)
    {
        aError("IrbemAPI: library not loaded");
        return eErrorNullPtr;
    }
    fn(ntime_max);
    return eNoError;
}

// ---- 涉及 COMMON 块共享状态（加锁） ----

errc_t IrbemAPI::make_lstar1(
    int ntime, int kext, const std::array<int, 5>& options, int sysaxes,
    const int* iyear, const int* idoy, const double* UT,
    const double* x1, const double* x2, const double* x3,
    const double* maginput,
    double* Lm, double* Lstar, double* Blocal,
    double* Bmin, double* J, double* MLT)
{
    using FnType = decltype(&make_lstar1_);
    std::lock_guard<std::mutex> lock(mutex_);
    auto fn = reinterpret_cast<FnType>(functions_[kMakeLstar]);
    if (!fn)
    {
        aError("IrbemAPI: library not loaded");
        return eErrorNullPtr;
    }
    fn(&ntime, &kext, const_cast<int*>(options.data()), &sysaxes,
       const_cast<int*>(iyear), const_cast<int*>(idoy),
       const_cast<double*>(UT),
       const_cast<double*>(x1), const_cast<double*>(x2), const_cast<double*>(x3),
       const_cast<double*>(maginput),
       Lm, Lstar, Blocal, Bmin, J, MLT);
    return eNoError;
}

errc_t IrbemAPI::get_field1(
    int kext, const std::array<int, 5>& options, int sysaxes,
    int iyear, int idoy, double UT,
    double x1, double x2, double x3,
    const double* maginput,
    double Bgeo[3], double& B)
{
    using FnType = decltype(&get_field1_);
    std::lock_guard<std::mutex> lock(mutex_);
    auto fn = reinterpret_cast<FnType>(functions_[kGetField]);
    if (!fn)
    {
        aError("IrbemAPI: library not loaded");
        return eErrorNullPtr;
    }
    fn(&kext, const_cast<int*>(options.data()), &sysaxes,
       &iyear, &idoy, &UT,
       &x1, &x2, &x3,
       const_cast<double*>(maginput),
       Bgeo, &B);
    return eNoError;
}

errc_t IrbemAPI::fly_in_afrl_crres1(
    int ntime, int sysaxes,
    int whichm, int whatf, int nene,
    const double* energy,
    const int* iyear, const int* idoy, const double* UT,
    const double* x1, const double* x2, const double* x3,
    const double* Ap15, double* flux,
    StringView ascii_path)
{
    // libirbem.h 声明正确：ascii_path 在 Fortran 端是 BYTE 数组，
    // gfortran 不追加隐藏字符串长度
    using FnType = decltype(&fly_in_afrl_crres1_);
    int slen = static_cast<int>(ascii_path.size());
    std::lock_guard<std::mutex> lock(mutex_);
    auto fn = reinterpret_cast<FnType>(functions_[kFlyInAfrlCrres]);
    if (!fn)
    {
        aError("IrbemAPI: library not loaded");
        return eErrorNullPtr;
    }
    fn(&ntime, &sysaxes,
       &whichm, &whatf, &nene,
       const_cast<double*>(energy),
       const_cast<int*>(iyear), const_cast<int*>(idoy),
       const_cast<double*>(UT),
       const_cast<double*>(x1), const_cast<double*>(x2), const_cast<double*>(x3),
       const_cast<double*>(Ap15), flux,
       const_cast<char*>(ascii_path.data()), &slen);
    return eNoError;
}

errc_t IrbemAPI::get_crres_flux(
    int ntime, int whichm, int whatf,
    int nene, const double* energy,
    const double* BBo, const double* L, const double* Ap15,
    double* flux, StringView ascii_path)
{
    // libirbem.h 声明缺少 gfortran 的隐藏字符串长度参数。
    // Fortran 端 afrl_crres_path 是 CHARACTER(*)，
    // gfortran 在末尾追加一个 int（按值传递）作为字符串长度。
    using RealFn = void(*)(int*, int*, int*, int*, double*,
                           double*, double*, double*,
                           double*, char*, int*, int);
    int slen = static_cast<int>(ascii_path.size());
    std::lock_guard<std::mutex> lock(mutex_);
    auto fn = reinterpret_cast<RealFn>(functions_[kGetCrresFlux]);
    if (!fn)
    {
        aError("IrbemAPI: library not loaded");
        return eErrorNullPtr;
    }
    fn(&ntime, &whichm, &whatf, &nene,
       const_cast<double*>(energy),
       const_cast<double*>(BBo), const_cast<double*>(L),
       const_cast<double*>(Ap15), flux,
       const_cast<char*>(ascii_path.data()), &slen, slen);
    return eNoError;
}

AST_NAMESPACE_END
