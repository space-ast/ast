///
/// @file      testVBScript.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-09
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
#include "AstTest/Test.hpp"

#if defined _WIN32 && !defined A_GCC && defined AST_WITH_COM
#include "AstScript/ScriptExecutor.hpp"
#include "AstUtil/ScopedPtr.hpp"
#include "AstCOM/COMAPI.hpp"
#include <comdef.h>
#include <comutil.h>

AST_USING_NAMESPACE


TEST(COMTest, ObjectRoot)
{
    auto root = aComObjectRoot();
    ASSERT_TRUE(root);
    BSTR memberName = SysAllocString(L"ExecuteCommand"); // 要查询的成员名
    DISPID dispid = DISPID_UNKNOWN;                      // 用于接收DISPID

    // 3. 调用 GetIDsOfNames
    HRESULT hr = root->GetIDsOfNames(
        IID_NULL,               // riid, 必须为 IID_NULL
        &memberName,            // rgszNames, 名称数组的地址
        1,                      // cNames, 名称数量为1
        LOCALE_SYSTEM_DEFAULT,  // lcid, 语言环境
        &dispid                 // rgDispId, 接收ID的数组
    );

    // 4. 检查结果并使用 DISPID
    EXPECT_TRUE(SUCCEEDED(hr));

    // 5. 释放资源
    SysFreeString(memberName);
}


TEST(COMTest, VBScript)
{
    #ifndef _WIN32
    GTEST_SKIP();
    #endif
    ScopedPtr<ScriptExecutor> executor = aNewScriptExecutor(EScriptLanguage::eVBScript);
    ASSERT_TRUE(executor);
    errc_t rc = executor->initialize();
    ASSERT_EQ(rc, eNoError);

    rc = executor->execute(R"(
        root.ExecuteCommand("echo hello world")
    )");
    ASSERT_EQ(rc, eNoError);

}

#endif

GTEST_MAIN();

