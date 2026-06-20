///
/// @file      snopt_quadratic.cxx
/// @brief     使用 SnoptCAPI 动态加载 SNOPT 求解带约束的二次规划问题
/// @details   求解以下优化问题：
///
///                min  (x1 - 1)^2 + (x2 - 2)^2
///                s.t. x1 + x2 - 2 ≤ 0
///
///            解析解: x* = (0.5, 1.5), f* = 0.5
///            (约束边界上，拉格朗日乘子 λ = 1.0)
///
///            本示例演示 SnoptCAPI 的完整用法：
///              1. aSnopt_Find() / aSnopt_Load() — 加载 SNOPT 动态库
///              2. api->sninit  — 初始化工作空间
///              3. api->snmema — 估计所需内存
///              4. api->snseti — 设置求解选项
///              5. api->snjac  — 确定雅可比稀疏结构
///              6. api->snopta — 求解 NLP 问题
///
///            编译要求：
///              - xmake 构建系统（已添加 AstOpt 依赖）
///              - 运行时需要 csnopt/snopt 动态库在可搜索路径中
///              - 测试时可用 AstMock 库: aSnopt_Load("AstMock")
///
/// @author    axel
/// @date      2026-06-15
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

#include <cstdio>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <clocale>

// ── SnoptCAPI 头文件 ────────────────────────────────────────────────
// SnoptCAPI 通过动态加载提供 SNOPT Fortran 函数的 C 封装
#include "AstOpt/SnoptAPI.hpp"

// f2c 兼容类型 (integer, doublereal, ftnlen 等)
#include "AstOpt/f2c.h"

using namespace ast;

// ═══════════════════════════════════════════════════════════════════
// 用户回调函数：计算目标函数值与约束函数值
// ═══════════════════════════════════════════════════════════════════
//
// 此函数签名必须与 SNOPT 的 My_fp 类型一致。
// F[0] 固定为目标函数值（当 ObjRow = 1 时）。
// F[1..neF-1] 为约束函数值。
//
// 参数说明：
//   Status  — 状态标志（通常忽略）
//   n       — 变量个数
//   x       — 当前变量值（数组长度 n）
//   needF   — 是否需要计算 F 值
//   neF     — F 数组长度（= 目标数 + 约束数）
//   F       — 输出：目标 + 约束值
//   needG   — 是否需要计算雅可比矩阵 G
//   neG     — G 中非零元素个数
//   G       — 输出：雅可比矩阵非零元素（按列压缩）
//   cu, lencu, iu, leniu, ru, lenru — 用户数据通道
// ═══════════════════════════════════════════════════════════════════
static int UsrFun(integer* Status, integer* n, doublereal x[],
                  integer* needF, integer* neF, doublereal F[],
                  integer* needG, integer* neG, doublereal G[],
                  char* cu, integer* lencu,
                  integer iu[], integer* leniu,
                  doublereal ru[], integer* lenru)
{
    // ── 目标函数: (x1 - 1)^2 + (x2 - 2)^2 ──────────────────────
    F[0] = (x[0] - 1.0) * (x[0] - 1.0) + (x[1] - 2.0) * (x[1] - 2.0);

    // ── 不等式约束: x1 + x2 ≤ 2 ────────────────────────────
    F[1] = x[0] + x[1];

    return 0;
}


// ═══════════════════════════════════════════════════════════════════
// 辅助函数
// ═══════════════════════════════════════════════════════════════════

static void printSeparator(const char* title = nullptr)
{
    if (title)
        printf("\n--- %s ---\n", title);
    else
        printf("------------------------------------------------------------\n");
}

static bool allocWorkspace(integer lencw, integer leniw, integer lenrw,
                           char*& cw, integer*& iw, doublereal*& rw)
{
    cw = new char[8 * lencw];
    iw = new integer[leniw];
    rw = new doublereal[lenrw];
    if (!cw || !iw || !rw)
    {
        printf("[错误] 工作空间内存分配失败\n");
        return false;
    }
    return true;
}


// ═══════════════════════════════════════════════════════════════════
// 主函数
// ═══════════════════════════════════════════════════════════════════
int main()
{
    std::setlocale(LC_ALL, ".UTF-8");
    printSeparator("SnoptCAPI 二次规划示例");
    printf("问题: min (x1-1)^2 + (x2-2)^2  s.t. x1+x2-2 <= 0\n");
    printf("解析解: x* = (0.5, 1.5), f* = 0.5\n");

    // ── 1. 加载 SNOPT 动态库 ────────────────────────────────────
    printSeparator("加载 SNOPT 动态库");

    // 方式 A: 自动搜索 (推荐)
    SnoptCAPI* api = aSnopt_Find();

    // 方式 B: 指定路径
    // SnoptCAPI* api = aSnopt_Load("csnopt");

    // 方式 C: 使用 Mock 库进行测试 (不需要真实 SNOPT 许可证)
    // SnoptCAPI* api = aSnopt_Load(AST_APPEND_DEBUG("AstMock"));

    if (!api)
    {
        printf("[错误] 无法加载 SNOPT 动态库\n");
        printf("请确保 csnopt.dll/so/dylib 在以下位置之一:\n");
        printf("  - 库安装目录 (aLibDir)\n");
        printf("  - 当前工作目录\n");
        printf("或测试时使用: aSnopt_Load(AST_APPEND_DEBUG(\"AstMock\"))\n");
        return 1;
    }
    printf("✓ SNOPT 动态库加载成功 (地址: %p)\n", (void*)api);

    // ── 2. 问题维度定义 ────────────────────────────────────────
    // 注意：不能加 const，因为 SNOPT Fortran 函数接受非 const 指针
    integer n           = 2;      // 变量个数: x1, x2
    integer neF         = 2;      // F 的长度: 1 目标 + 1 约束
    integer ObjRow      = 1;      // 目标函数在 F 中的行号 (1-based)
    doublereal ObjAdd   = 0.0;    // 目标函数附加值
    doublereal INFBND   = 1.0e20; // "无穷大"边界

    // ── 3. 初始化工作空间 ──────────────────────────────────────
    integer iprint = 0;   // 打印级别: 0 = 静默
    integer isumm  = 6;   // 摘要输出单元

    integer lencw = 500;
    integer leniw = 500;
    integer lenrw = 500;
    integer cw_len = 8 * lencw;

    char*        cw = nullptr;
    integer*     iw = nullptr;
    doublereal*  rw = nullptr;

    if (!allocWorkspace(lencw, leniw, lenrw, cw, iw, rw))
        return 1;

    api->sninit(&iprint, &isumm, cw, &lencw, iw, &leniw, rw, &lenrw, cw_len);
    printf("✓ sninit 完成 (lencw=%ld, leniw=%ld, lenrw=%ld)\n",
           (long)lencw, (long)leniw, (long)lenrw);

    // ── 4. 估计所需内存 ────────────────────────────────────────
    integer nxname = 1;    // 不提供变量名 (SNOPT会修改此值)
    integer nfname = 1;    // 不提供约束名 (SNOPT会修改此值)
    integer neA    = -1;   // 线性雅可比非零元数, SNOPT输出实际值
    integer neG    = -1;   // 非线性雅可比非零元数, SNOPT输出实际值
    integer info;
    integer mincw, miniw, minrw;

    api->snmema(&info, &neF, &n, &nxname, &nfname, &neA, &neG,
                &mincw, &miniw, &minrw,
                cw, &lencw, iw, &leniw, rw, &lenrw, 8 * 500);

    printf("✓ snmema 完成: 需要 mincw=%ld, miniw=%ld, minrw=%ld\n",
           (long)mincw, (long)miniw, (long)minrw);

    // 如果初始空间不足，重新分配并保留已初始化的内容
    if (mincw > lencw || miniw > leniw || minrw > lenrw)
    {
        printf("  工作空间不足，重新分配...\n");

        integer tlencw = lencw, tleniw = leniw, tlenrw = lenrw;
        char* tcw = cw; integer* tiw = iw; doublereal* trw = rw;

        lencw = mincw; leniw = miniw; lenrw = minrw;
        cw_len = 8 * lencw;

        if (!allocWorkspace(lencw, leniw, lenrw, cw, iw, rw))
        { delete[] tcw; delete[] tiw; delete[] trw; return 1; }

        integer mlencw = lencw < tlencw ? lencw : tlencw;
        integer mleniw = leniw < tleniw ? leniw : tleniw;
        integer mlenrw = lenrw < tlenrw ? lenrw : tlenrw;
        memcpy(cw, tcw, 8 * mlencw * sizeof(char));
        memcpy(iw, tiw, mleniw * sizeof(integer));
        memcpy(rw, trw, mlenrw * sizeof(doublereal));

        // 通知 SNOPT 新的工作空间大小
        integer iPrt = 0, iSum = 0;
        integer stropt_len;

        stropt_len = (integer)sizeof("Total real workspace   ") - 1;
        api->snseti(const_cast<char*>("Total real workspace   "), &lenrw,
                    &iPrt, &iSum, &info, cw, &lencw, iw, &leniw, rw, &lenrw,
                    stropt_len, 8 * 500);

        stropt_len = (integer)sizeof("Total integer workspace") - 1;
        api->snseti(const_cast<char*>("Total integer workspace"), &leniw,
                    &iPrt, &iSum, &info, cw, &lencw, iw, &leniw, rw, &lenrw,
                    stropt_len, 8 * 500);

        delete[] tcw; delete[] tiw; delete[] trw;

        // 重新初始化
        api->sninit(&iprint, &isumm, cw, &lencw, iw, &leniw, rw, &lenrw, cw_len);
    }

    // ── 5. 设置求解选项 ────────────────────────────────────────
    {
        integer iPrt = 0, iSum = 0;
        integer majorIter = 500;
        integer stropt_len = (integer)sizeof("Major Iterations Limit") - 1;
        api->snseti(const_cast<char*>("Major Iterations Limit"), &majorIter,
                    &iPrt, &iSum, &info,
                    cw, &lencw, iw, &leniw, rw, &lenrw,
                    stropt_len, 8 * 500);
        printf("✓ Major Iterations Limit = %ld\n", (long)majorIter);
    }

    // ── 6. 分配变量与约束数组 ──────────────────────────────────
    doublereal* xlow   = new doublereal[n];
    doublereal* xupp   = new doublereal[n];
    doublereal* x      = new doublereal[n];
    integer*    xstate = new integer[n];
    doublereal* xmul   = new doublereal[n];

    doublereal* Flow   = new doublereal[neF];
    doublereal* Fupp   = new doublereal[neF];
    doublereal* F      = new doublereal[neF];
    integer*    Fstate = new integer[neF];
    doublereal* Fmul   = new doublereal[neF];

    // 变量边界: -∞ ≤ x1,x2 ≤ +∞ (无界)
    for (integer i = 0; i < n; i++)
    {
        xlow[i]   = -INFBND;
        xupp[i]   = +INFBND;
        x[i]      = 0.0;    // 初始猜测: (0, 0)
        xstate[i] = 0;      // 0 = 初值可忽略
        xmul[i]   = 0.0;
    }

    // 目标函数边界: 无约束
    Flow[0] = -INFBND;
    Fupp[0] = +INFBND;

    // 约束边界: -∞ ≤ x1+x2 ≤ 2, 即 x1+x2-2 ≤ 0
    Flow[1] = -INFBND;
    Fupp[1] = 2;

    for (integer i = 0; i < neF; i++)
    {
        F[i]      = 0.0;
        Fstate[i] = 0;
        Fmul[i]   = 0.0;
    }

    printf("✓ 边界: 变量无界, 目标无界, 约束 c(x) ≤ 0, 初值 (0,0)\n");

    // ── 7. 确定雅可比稀疏结构 ──────────────────────────────────
    // 雅可比矩阵 (neF × n = 2 × 2):
    //   [ ∂obj/∂x1    ∂obj/∂x2   ]   row 1 (目标)
    //   [ ∂con/∂x1    ∂con/∂x2   ]   row 2 (约束)
    // 目标行总是稠密的 (SNOPT 特殊处理)
    // 约束行: ∂c/∂x1=1, ∂c/∂x2=1 → 2 个非零元

    integer lenA = neF * n;
    integer* iAfun = new integer[lenA];
    integer* jAvar = new integer[lenA];
    doublereal* A  = new doublereal[lenA];

    integer lenG = neF * n;
    integer* iGfun = new integer[lenG];
    integer* jGvar = new integer[lenG];

    char* cu = cw;
    integer lencu = lencw;
    integer* iu = iw;
    integer leniu = leniw;
    doublereal* ru = rw;
    integer lenru = lenrw;
    short cu_len = (short)cw_len;

    api->snjac(&info, &neF, &n, UsrFun,
               iAfun, jAvar, &lenA, &neA, A,
               iGfun, jGvar, &lenG, &neG,
               x, xlow, xupp,
               &mincw, &miniw, &minrw,
               cu, &lencu, iu, &leniu, ru, &lenru,
               cw, &lencw, iw, &leniw, rw, &lenrw,
               cu_len, cw_len);

    printf(" snjac 完成: neA=%ld (线性), neG=%ld (非线性)\n",
           (long)neA, (long)neG);

    // snjac 已经计算了常数雅可比 A 的值，直接使用
    printf("  A (线性雅可比, snjac自动计算):\n");
    for (integer k = 0; k < neA; k++)
        printf("    A[%ld]: 行=%ld 列=%ld 值=%g\n",
               (long)k, (long)iAfun[k], (long)jAvar[k], A[k]);
    printf("  G (非线性雅可比, 值由有限差分计算):\n");
    for (integer k = 0; k < neG; k++)
        printf("    G[%ld]: 行=%ld 列=%ld\n",
               (long)k, (long)iGfun[k], (long)jGvar[k]);

    // ── 8. 调用 SNOPT 求解 ─────────────────────────────────────
    printSeparator("开始求解");

    integer start = 0;  // 冷启动
    char probName[] = "二次规划示例";
    integer npname = (integer)sizeof(probName) - 1;
    integer nS, nInf;
    doublereal sInf;

    api->snopta(&start, &neF, &n, &nxname, &nfname, &ObjAdd, &ObjRow,
                probName, UsrFun,
                iAfun, jAvar, &lenA, &neA, A,
                iGfun, jGvar, &lenG, &neG,
                xlow, xupp, nullptr,     // xnames
                Flow, Fupp, nullptr,     // fnames
                x, xstate, xmul,
                F, Fstate, Fmul,
                &info, &mincw, &miniw, &minrw,
                &nS, &nInf, &sInf,
                cu, &lencu, iu, &leniu, ru, &lenru,
                cw, &lencw, iw, &leniw, rw, &lenrw,
                npname, nxname * 8, nfname * 8, cu_len, cw_len);

    // ── 9. 输出结果 ─────────────────────────────────────────────
    printSeparator("求解结果");

    const char* infoMsg;
    switch (info)
    {
    case 1:  infoMsg = "满足最优性条件 (KKT)";          break;
    case 2:  infoMsg = "找到可行解, 未满足最优性";       break;
    case 3:  infoMsg = "无法达到指定精度";              break;
    case 10: infoMsg = "问题不可行";                    break;
    default: infoMsg = "异常终止, 请检查返回码含义";     break;
    }
    printf("info = %ld (%s)\n", (long)info, infoMsg);
    printf("nS = %ld, nInf = %ld, sInf = %g\n",
           (long)nS, (long)nInf, sInf);

    printf("\n--- 最优解 ---\n");
    printf("  x1*   = %16.8f  (解析解: 0.5)\n", x[0]);
    printf("  x2*   = %16.8f  (解析解: 1.5)\n", x[1]);
    printf("  f(x*) = %16.8f  (解析解: 0.5)\n", F[0]);
    printf("  c(x*) = %16.8f  (应 ≈ 2, 约束活跃在 Fupp[1]=2)\n", F[1]);

    printf("\n--- 拉格朗日乘子 ---\n");
    printf("  xmul[0] = %g (x1)\n", xmul[0]);
    printf("  xmul[1] = %g (x2)\n", xmul[1]);
    printf("  Fmul[0] = %g (目标)\n", Fmul[0]);
    printf("  Fmul[1] = %g (约束, 解析: 1.0)\n", Fmul[1]);

    printf("\n--- 验证 ---\n");
    printf("  x1+x2 = %g (应 ≈ 2)\n", x[0] + x[1]);

    // ── 10. 清理 ────────────────────────────────────────────────
    delete[] cw;    delete[] iw;    delete[] rw;
    delete[] iAfun; delete[] jAvar; delete[] A;
    delete[] iGfun; delete[] jGvar;
    delete[] x;     delete[] xlow;   delete[] xupp;
    delete[] xmul;  delete[] xstate;
    delete[] F;     delete[] Flow;   delete[] Fupp;
    delete[] Fmul;  delete[] Fstate;

    // 卸载动态库
    aSnopt_Unload();

    printSeparator();
    if (info == 1)
        printf("求解成功!\n");
    else
        printf("求解结束 (info=%ld)\n", (long)info);

    return (info == 1) ? 0 : (int)info;
}
