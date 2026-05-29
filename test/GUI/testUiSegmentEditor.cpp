///
/// @file      testUiSegmentEditor.cpp
/// @brief     测试 UiSegmentEditor — RTTI 类型切换、编辑页面匹配
/// @author    axel
/// @date      2026-05-17
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

#include "AstGUI/UiSegmentEditor.hpp"
#include "AstCore/InitialState.hpp"
#include "AstCore/Propagate.hpp"
#include "AstCore/Maneuver.hpp"
#include "AstCore/Sequence.hpp"
#include "AstCore/TargeterSequence.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include <QApplication>
#include <cstdio>

using namespace ast;

static int g_failures = 0;

#define CHECK(cond, msg) \
    do { \
        if (!(cond)) { \
            printf("  FAIL: %s\n", msg); \
            ++g_failures; \
        } else { \
            printf("  OK:   %s\n", msg); \
        } \
    } while(0)

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // ---- 占位页索引为 0 ----
    UiSegmentEditor editor;

    int placeholderIdx = 0;
    CHECK(editor.currentIndex() == placeholderIdx,
          "初始显示占位页面");

    // ---- 逐个测试 RTTI 类型匹配 ----

    // 1. InitialState
    {
        auto* obj = aNewObject<InitialState>();
        obj->setName("TestInit");
        editor.editCommand(obj);

        CHECK(editor.currentIndex() != placeholderIdx,
              "InitialState: 已切换到编辑器页面");
        CHECK(editor.currentIndex() > 0,
              "InitialState: 页面索引有效");
    }

    // 2. Propagate
    {
        auto* obj = aNewObject<Propagate>();
        obj->setName("TestProp");
        obj->setMaxPropTime(12345);
        editor.editCommand(obj);

        int idx = editor.currentIndex();
        CHECK(idx != placeholderIdx,
              "Propagate: 已切换到编辑器页面");
    }

    // 3. Maneuver
    {
        auto* obj = aNewObject<Maneuver>();
        obj->setName("TestManeuver");
        editor.editCommand(obj);

        int idx = editor.currentIndex();
        CHECK(idx != placeholderIdx,
              "Maneuver: 已切换到编辑器页面");
    }

    // 4. Sequence
    {
        auto* obj = aNewObject<Sequence>();
        obj->setName("TestSequence");
        obj->setRepeatCount(3);
        editor.editCommand(obj);

        int idx = editor.currentIndex();
        CHECK(idx != placeholderIdx,
              "Sequence: 已切换到编辑器页面");
    }

    // 5. TargeterSequence — IS-A Sequence，必须匹配子类
    {
        auto* obj = aNewObject<TargeterSequence>();
        obj->setName("TestTargeter");
        obj->setRepeatCount(5);
        editor.editCommand(obj);

        int tgtIdx = editor.currentIndex();
        int seqIdx = [&]() {
            auto* seq = aNewObject<Sequence>();
            editor.editCommand(seq);
            return editor.currentIndex();
        }();

        CHECK(tgtIdx != seqIdx,
              "TargeterSequence 与 Sequence 使用不同编辑器页面");
        CHECK(tgtIdx != placeholderIdx,
              "TargeterSequence: 已切换到编辑器页面");

        printf("  TargeterSequence index=%d  Sequence index=%d\n", tgtIdx, seqIdx);
    }

    // 6. editCommand(nullptr) → clear()
    {
        editor.editCommand(nullptr);
        CHECK(editor.currentIndex() == placeholderIdx,
              "nullptr: 回到占位页面");
    }

    // 7. clear() after editing
    {
        auto* obj = aNewObject<Propagate>();
        editor.editCommand(obj);
        CHECK(editor.currentIndex() != placeholderIdx,
              "clear(): 先切换到 Propagate");

        editor.clear();
        CHECK(editor.currentIndex() == placeholderIdx,
              "clear(): 回到占位页面");
    }

    // ---- 结果 ----
    printf("\n");
    if (g_failures == 0) {
        printf("All tests PASSED.\n");
    } else {
        printf("%d test(s) FAILED.\n", g_failures);
    }

    return g_failures;
}
