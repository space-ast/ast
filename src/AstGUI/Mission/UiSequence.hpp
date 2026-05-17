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
