#include "ComInit.hpp"

AST_NAMESPACE_BEGIN

#ifdef _WIN32

HRESULT aEnsureCoInitialized()
{
    static A_THREAD_LOCAL CoInitializeGuard guard(COINIT_APARTMENTTHREADED);
    if (guard.hr_ == RPC_E_CHANGED_MODE) return S_FALSE;
    return guard.hr_;
}

#endif

AST_NAMESPACE_END
