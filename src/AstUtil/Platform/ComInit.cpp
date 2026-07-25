#include "ComInit.hpp"

AST_NAMESPACE_BEGIN

#ifdef _WIN32

HRESULT aEnsureCoInitialized()
{
    static A_THREAD_LOCAL CoInitializeGuard guard(COINIT_APARTMENTTHREADED);
    return guard.hr_;
}

#endif

AST_NAMESPACE_END
