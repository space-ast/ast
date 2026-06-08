///
/// @file      SpiceAPI.cpp
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

#include "SpiceAPI.hpp"
#include <cstdlib>              // for atexit
#include <type_traits>          // for aligned_storage
#include "AstUtil/StringView.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstCore/RunTimeConfig.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/LocaleGuard.hpp"
#include "AstUtil/Encode.hpp"


AST_NAMESPACE_BEGIN

// 声明spice函数原型，参考SpiceZpr.h
namespace spiceproto{
    
#if defined(__linux__)
#   define CSPICE_PC_LINUX_64BIT_GCC
#endif

#if (   defined(CSPICE_ALPHA_DIGITAL_UNIX    )      \
     || defined(CSPICE_SUN_SOLARIS_64BIT_NATIVE)    \
     || defined(CSPICE_SUN_SOLARIS_64BIT_GCC )      \
     || defined(CSPICE_MAC_OSX_INTEL_64BIT_GCC )    \
     || defined(CSPICE_MAC_OSX_M1_64BIT_CLANG )     \
     || defined(CSPICE_SUN_SOLARIS_INTEL_64BIT_CC ) \
     || defined(CSPICE_PC_CYGWIN_64BIT_GCC )        \
     || defined(CSPICE_PC_LINUX_64BIT_GCC    )  )
   
    typedef int         SpiceInt;
#else
    typedef long        SpiceInt;
#endif

    typedef const char     ConstSpiceChar;
    typedef double         SpiceDouble;
    typedef char           SpiceChar;
    typedef int            SpiceBoolean;

    
    void              furnsh_c ( ConstSpiceChar    * file );
    
    void              spkgeo_c ( SpiceInt            targ,
                                SpiceDouble         et,
                                ConstSpiceChar    * ref,
                                SpiceInt            obs,
                                SpiceDouble         state[6],
                                SpiceDouble       * lt       );
                                
    void              spklef_c ( ConstSpiceChar    * filename,
                                SpiceInt          * handle   );

    void              spkuef_c ( SpiceInt             handle );

    void              ktotal_c ( ConstSpiceChar   * kind,
                                SpiceInt         * count );

    void              bodc2n_c ( SpiceInt             code,
                                SpiceInt             namelen,
                                SpiceChar          * name,
                                SpiceBoolean       * found   );

    void              erract_c ( ConstSpiceChar    * operation,
                                SpiceInt            lenout,
                                SpiceChar         * action    );

    SpiceBoolean      failed_c ( void );

    void              reset_c  ( void );

    void              kclear_c ( void );
    

    // 根据SpiceZdf.h的说明，SpiceInt的大小是SpiceDouble的一半
    // 在这里进行编译期检查
    static_assert(2 * sizeof(SpiceInt) == sizeof(SpiceDouble), "SpiceInt and SpiceDouble size must be 2:1");
}

int funcarray_loadedfunc(const SpiceAPI::funcarray& funcs)
{
    int count = 0;
    for(auto& func : funcs)
        if(func)
            count++;
    return count;
}

bool funcarray_isfull(const SpiceAPI::funcarray& funcs)
{
    for(auto& func : funcs)
        if(!func)
            return false;
    return true;
}

namespace{
std::aligned_storage<sizeof(SpiceAPI), alignof(SpiceAPI)>::type buf;  // 单例存储区
bool singletonDestroyed = false;                                       // 单例是否已销毁
struct SingletonGuard {
    SingletonGuard(){
        new(&buf) SpiceAPI(true);
        singletonDestroyed = false;
    }
    ~SingletonGuard() {
        reinterpret_cast<SpiceAPI*>(&buf)->~SpiceAPI();
        // 注意要在析构后再将标志位设置为 true，顺序不能弄反，否则会导致在没有析构时就再次构造单例，产生未定义行为
        singletonDestroyed = true;
    }
};
}

SpiceAPI* SpiceAPI::Instance()
{
    static SingletonGuard guard;

    // 采用凤凰单例模式安全承接调用，避免 use-after-destroy。
    if (A_UNLIKELY(singletonDestroyed))
    {
        new(&buf) SpiceAPI(false);
        singletonDestroyed = false;
        atexit([]() {
            if(!singletonDestroyed)
            {
                reinterpret_cast<SpiceAPI*>(&buf)->~SpiceAPI();
                // 注意要在析构后再将标志位设置为 true，顺序不能弄反，否则会导致在没有析构时就再次构造单例，产生未定义行为
                singletonDestroyed = true;
            }
        });
    }
    return reinterpret_cast<SpiceAPI*>(&buf);
}

SpiceAPI::SpiceAPI(bool shouldLoadDynamicLib)
{
    if(shouldLoadDynamicLib)
    {
        // 遍历所配置的动态库路径
        auto lispaths = aGetConfigStringVector("CSPICE_LIBS");
        if(!lispaths.empty()){
            tryload(lispaths);
        }else{
            // 尝试加载默认动态库
            load("cspice");
        }
    }
}

SpiceAPI::~SpiceAPI()
{
    kclear();   // 清除所有已加载的内核
    unload();   // 卸载库
}

errc_t SpiceAPI::load(StringView dirpath)
{
    void* lib = aLoadLibrary(std::string(dirpath).c_str());
    if(!lib)
        return eErrorInvalidFile;
    // auto spkpos_ = aGetProcAddress(lib, "spkpos_c");
    funcarray funcs{};
    funcs[ifurnsh] = aGetProcAddress(lib, "furnsh_c");
    funcs[ispkgeo] = aGetProcAddress(lib, "spkgeo_c");
    funcs[ispklef] = aGetProcAddress(lib, "spklef_c");
    funcs[ispkuef] = aGetProcAddress(lib, "spkuef_c");
    funcs[iktotal] = aGetProcAddress(lib, "ktotal_c");
    funcs[ibodc2n] = aGetProcAddress(lib, "bodc2n_c");
    funcs[ierract] = aGetProcAddress(lib, "erract_c");
    funcs[ifailed] = aGetProcAddress(lib, "failed_c");
    funcs[ireset]  = aGetProcAddress(lib, "reset_c");
    funcs[ikclear] = aGetProcAddress(lib, "kclear_c");
    
    int numloaded = funcarray_loadedfunc(funcs);
    if(numloaded < numfunctions)
    {
        aError("failed to load all %d functions, only %d loaded", numfunctions, numloaded);
        aFreeLibrary(lib);
        return eErrorInvalidFile;
    }
    library_ = lib;
    functions_ = funcs;
    // 防止 cspice 直接让程序退出(默认是ABORT Mode)
    char action[] = "RETURN";
    erract("SET", 0, action);
    return eNoError;
}

errc_t SpiceAPI::tryload(const std::vector<std::string>& libpaths)
{
    for(auto& libpath : libpaths)
        if(load(libpath) == eNoError)
            return eNoError;
    return eErrorInvalidFile;
}

errc_t SpiceAPI::unload()
{
    if(library_)
    {
        errc_t rc = aFreeLibrary(library_);
        library_ = nullptr;
        functions_ = funcarray{};
        return rc;
    }
    return eNoError;
}

const char* kSpiceUnloadError = "spice library not loaded, call SpiceAPI::load first";

errc_t SpiceAPI::furnsh(const char* file)
{
    using functype = decltype(&spiceproto::furnsh_c);
    functype furnsh_c = reinterpret_cast<functype>(functions_[ifurnsh]);
    if(!furnsh_c)
    {
        aError(kSpiceUnloadError);
        return eErrorNullPtr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    std::string crt;
    aUTF8ToCRT(file, crt);
    furnsh_c(crt.c_str());
    return checkerror();
}

errc_t SpiceAPI::spkgeo(int targ, double et, const char * ref, int obs, double state[6], double * lt)
{
    using functype = decltype(&spiceproto::spkgeo_c);
    functype spkgeo_c = reinterpret_cast<functype>(functions_[ispkgeo]);
    if(!spkgeo_c)
    {
        aError(kSpiceUnloadError);
        return eErrorNullPtr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    spkgeo_c(targ, et, ref, obs, state, lt);
    return checkerror();
}

errc_t SpiceAPI::spklef(const char *filename, int *handle)
{
    using functype = decltype(&spiceproto::spklef_c);
    functype spklef_c = reinterpret_cast<functype>(functions_[ispklef]);
    if(!spklef_c)
    {
        aError(kSpiceUnloadError);
        return eErrorNullPtr;
    }
    spiceproto::SpiceInt h=0;
    std::lock_guard<std::mutex> lock(mutex_);
    std::string crt;
    aUTF8ToCRT(filename, crt);
    spklef_c(crt.c_str(), &h);
    if(h >= (spiceproto::SpiceInt)spk_handles_.size())
    {
        spk_handles_.resize(h + 1);
    }
    if(h>=0){
        spk_handles_[h] ++;
    }
    if(handle)
        *handle = static_cast<int>(h);
    return checkerror();
}

errc_t SpiceAPI::spkuef(int handle)
{
    using functype = decltype(&spiceproto::spkuef_c);
    functype spkuef_c = reinterpret_cast<functype>(functions_[ispkuef]);
    if(!spkuef_c)
    {
        aError(kSpiceUnloadError);
        return eErrorNullPtr;
    }
    if(handle >= (spiceproto::SpiceInt)spk_handles_.size() || handle < 0)
        return eErrorInvalidParam;
    std::lock_guard<std::mutex> lock(mutex_);
    spk_handles_[handle] --;
    if(spk_handles_[handle] == 0){
        spkuef_c(handle);
    }
    return checkerror();
}

void SpiceAPI::bodc2n(int code, int namlen, char *name, bool *found)
{
    using functype = decltype(&spiceproto::bodc2n_c);
    functype bodc2n_c = reinterpret_cast<functype>(functions_[ibodc2n]);
    if(!bodc2n_c)
        return;
    spiceproto::SpiceBoolean f;
    bodc2n_c(code, namlen, name, &f);
    if(found)
        *found = (f != 0);
    checkerror();
}

errc_t SpiceAPI::bodc2n(int code, std::string &name)
{
    char n[128];
    bool found = false;
    bodc2n(code, sizeof(n), n, &found);
    if(found){
        name = n;
        return checkerror();
    }
    return eErrorNotFound;
}

bool SpiceAPI::failed()
{
    using functype = decltype(&spiceproto::failed_c);
    functype failed_c = reinterpret_cast<functype>(functions_[ifailed]);
    if(!failed_c)
        return true;
    return failed_c() != 0;
}

void SpiceAPI::reset()
{
    using functype = decltype(&spiceproto::reset_c);
    functype reset_c = reinterpret_cast<functype>(functions_[ireset]);
    if(!reset_c)
        return;
    reset_c();
}

void SpiceAPI::erract(const char * operation, int lenout, char * action)
{
    using functype = decltype(&spiceproto::erract_c);
    functype erract_c = reinterpret_cast<functype>(functions_[ierract]);
    if(!erract_c)
        return;
    erract_c(operation, lenout, action);
}

void SpiceAPI::kclear()
{
    using functype = decltype(&spiceproto::kclear_c);
    functype kclear_c = reinterpret_cast<functype>(functions_[ikclear]);
    if(!kclear_c)
        return;
    kclear_c();
}


errc_t SpiceAPI::ktotal(const char *kind, int *count)
{
    using functype = decltype(&spiceproto::ktotal_c);
    functype ktotal_c = reinterpret_cast<functype>(functions_[iktotal]);
    if(!ktotal_c)
    {
        aError(kSpiceUnloadError);
        return eErrorNullPtr;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    spiceproto::SpiceInt c;
    ktotal_c(kind, &c);
    if(count)
        *count = static_cast<int>(c);
    return checkerror();
}

errc_t SpiceAPI::checkerror()
{
    if(failed())
    {
        // 这里必须重置一下，否则后续调用会一直报错...
        reset();
        return eError;
    }
    return eNoError;
}



AST_NAMESPACE_END


