///
/// @file      GUIInterface.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
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

#include "GUIInterface.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/LibraryLoader.hpp"
#include "AstGUI/GUIInterfaceImpl.hpp"

AST_NAMESPACE_BEGIN

static GUIInterface *g_guiInstance{nullptr};

GUIInterface *GUIInterface::NoopInstance()
{
    static GUIInterface noop;
    return &noop;
}

GUIInterface *GUIInterface::CurrentInstance()
{
    if(g_guiInstance == nullptr)
    {
        using functype = decltype(&aGUIInterfaceImpl);
        functype func = (functype)aResolveProcAddress(AST_LIB_LINKNAME("AstGUI"), A_STR(aGUIInterfaceImpl));
        if(func){
            g_guiInstance = func();
        }else{
            aError("failed to resolve function 'aGUIInterfaceImpl' from shared library 'AstGUI'");
        }
        if(g_guiInstance == nullptr){
            g_guiInstance = NoopInstance();
        }
    }
    return g_guiInstance;
}

void GUIInterface::SetCurrentInstance(GUIInterface *instance)
{
    if(instance){
        g_guiInstance = instance;
    }
}

errc_t GUIInterface::editObject(Object* object)
{
    aError("noop: editObject not implemented");
    return -1;
}

Object *GUIInterface::selectObject(StringView typeName)
{
    aError("noop: selectObject not implemented");
    return nullptr;
}

const char* GUIInterface::translate(const char* msgctxt, const char* msgid)
{
    return msgid;
}



AST_NAMESPACE_END

