///
/// @file      UiResultView.hpp
/// @brief     结果表格和进度条组件
/// @details   ~
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

#pragma once

#include "AstGlobal.h"
#include <QWidget>
#include <QTableWidget>
#include <QProgressBar>
#include <QLabel>
#include <QVBoxLayout>
#include <vector>
#include <QStringList>

AST_NAMESPACE_BEGIN

/// @brief 结果表格与进度条组件
class AST_GUI_API UiResultView : public QWidget
{
    Q_OBJECT
public:
    explicit UiResultView(QWidget* parent = nullptr);

    /// @brief 初始化表格列头
    /// @param variableNames 变量名列表
    /// @param responseNames 响应名列表
    void initColumns(const QStringList& variableNames, const QStringList& responseNames);

    /// @brief 追加一行结果
    /// @param runIndex   运行序号
    /// @param varValues  变量值列表
    /// @param respValues 响应值列表
    void appendRow(int runIndex, const std::vector<double>& varValues, const std::vector<double>& respValues);

    /// @brief 清空所有结果
    void clearResults();

    /// @brief 更新进度
    void setProgress(int current, int total);

    /// @brief 设置运行时间
    void setElapsedTime(double seconds);

private:
    void setupUi();

    QVBoxLayout*  layout_;
    QTableWidget* table_;
    QProgressBar* progressBar_;
    QLabel*       progressLabel_;
    QLabel*       timeLabel_;
};

AST_NAMESPACE_END
