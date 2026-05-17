///
/// @file      UiSequence.hpp
/// @brief     Sequence / TargeterSequence 段编辑器
/// @details   重复次数、脚本工具、打靶配置
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
#include "AstGUI/UiObject.hpp"
#include <QSpinBox>
#include <QCheckBox>

AST_NAMESPACE_BEGIN

class Sequence;
class TargeterSequence;

/// @brief Sequence 段编辑器
class AST_GUI_API UiSequence : public UiObject
{
    Q_OBJECT
public:
    explicit UiSequence(Object* object, QWidget* parent = nullptr);
    explicit UiSequence(QWidget* parent = nullptr);
    ~UiSequence() override;

    void setSequence(Sequence* seq);
    Sequence* getSequence() const;

private slots:
    void onRepeatCountChanged(int value);

private:
    void setupUi();

    QSpinBox* repeatCountSpin_ = nullptr;
};

/// @brief TargeterSequence 段编辑器（在 Sequence 基础上增加打靶配置）
class AST_GUI_API UiTargeterSequence : public UiObject
{
    Q_OBJECT
public:
    explicit UiTargeterSequence(Object* object, QWidget* parent = nullptr);
    explicit UiTargeterSequence(QWidget* parent = nullptr);
    ~UiTargeterSequence() override;

    void setTargeterSequence(TargeterSequence* seq);
    TargeterSequence* getTargeterSequence() const;

private:
    void setupUi();

    QSpinBox* repeatCountSpin_ = nullptr;
    QSpinBox* maxIterSpin_ = nullptr;
};

AST_NAMESPACE_END
