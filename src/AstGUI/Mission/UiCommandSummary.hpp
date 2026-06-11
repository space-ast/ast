///
/// @file      UiCommandSummary.hpp
/// @brief     任务命令概要显示控件，以格式化文本展示段状态、时间、轨道根数等信息
/// @author    axel
/// @date      2026-06-11
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

#pragma once

#include "AstGlobal.h"
#include "AstGUI/UiObject.hpp"
#include "AstCore/OrbitElement.hpp"
#include "AstCore/TimePoint.hpp"
#include <functional>
#include <vector>

class QTextEdit;

AST_NAMESPACE_BEGIN

class Segment;
class Command;
class SpacecraftState;
class State;
class Frame;

// ============================================================================
// 管道类型
// ============================================================================

/// @brief 段上下文，汇总所有区块需要的公共数据，避免每个区块各自解析
struct SegmentContext
{
    const Segment*           seg = nullptr;
    const SpacecraftState*   outputState = nullptr;
    const State*             orbitState = nullptr;
    Frame*                   frame = nullptr;
    CartState                cart;
    ModOrbElem               moe;
    TimePoint                stateEpoch;

    /// @brief 上下文数据是否完整可用
    bool valid() const { return orbitState != nullptr; }
};

/// @brief 概要区块：独立生成一段多行文本（标题 + 内容）
/// @details applicable 判断该区块是否适用于当前段；
///          generate   生成区块文本（含标题行和分隔线）。
///          区块彼此不感知，便于独立增删和排序。
struct SummarySection
{
    QString                title;       ///< 区块标题（仅用于标识）
    std::function<bool   (const SegmentContext&)> applicable;  ///< 是否适用
    std::function<QString(const SegmentContext&)> generate;    ///< 生成区块文本
};

/// @brief 区块组：1 个 = 独占整行，2 个 = 横向并排
using SectionGroup = std::vector<SummarySection>;

// ============================================================================
// UiCommandSummary
// ============================================================================

/// @brief 任务命令概要显示控件
/// @details 以只读格式化文本展示命令的概要信息，包括段元数据、时间系统、
///          直角坐标/球坐标/LLR/开普勒轨道根数等信息。
///          内部采用 Section 管道架构：新增区块只需定义 SummarySection 并
///          加入 sectionGroups_，不需要修改主逻辑。
class AST_GUI_API UiCommandSummary : public UiObject
{
    Q_OBJECT
public:
    explicit UiCommandSummary(Object* object, QWidget* parent = nullptr);
    explicit UiCommandSummary(QWidget* parent = nullptr);
    ~UiCommandSummary() override;

    /// @brief 设置要显示概要的命令
    void setCommand(Command* command);

    /// @brief 获取当前命令
    Command* getCommand() const;

    /// @brief 刷新概要显示
    void refresh();

protected:
    /// @brief 子类可重写此方法来定制区块列表（如增删、排序）
    virtual std::vector<SectionGroup> buildSectionGroups() const;

private:
    void setupUi();

    /// @brief 生成概要文本（管道入口）
    QString generateSummary() const;

    /// @brief 从当前命令构建段上下文
    SegmentContext buildContext() const;

    /// @brief 渲染单个区块（独占行）
    static QString renderSingle(const SummarySection& sec, const SegmentContext& ctx);

    /// @brief 渲染两个区块并排
    static QString renderSideBySide(const SummarySection& left,
                                    const SummarySection& right,
                                    const SegmentContext& ctx,
                                    int colWidth = 52);

    // ---- 区块生成函数（静态，仅依赖 SegmentContext） ----
    static QString formatTimeSystems(const SegmentContext& ctx);
    static QString formatCartesian(const SegmentContext& ctx);
    static QString formatSpherical(const SegmentContext& ctx);
    static QString formatLLR(const SegmentContext& ctx);
    static QString formatKeplerian(const SegmentContext& ctx);

    QTextEdit* textEdit_ = nullptr;
};

AST_NAMESPACE_END
