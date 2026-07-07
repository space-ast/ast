///
/// @file      UiToolCallCard.cpp
/// @brief     工具调用卡片部件实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiToolCallCard.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

AST_NAMESPACE_BEGIN

UiToolCallCard::UiToolCallCard(const QString& toolCallId,
                               const QString& functionName,
                               const QString& arguments,
                               QWidget* parent)
    : QWidget(parent)
    , toolCallId_(toolCallId)
    , functionName_(functionName)
    , arguments_(arguments)
{
    setupUi();
    updateAppearance();
}

UiToolCallCard::~UiToolCallCard() = default;

void UiToolCallCard::setupUi()
{
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 标题栏按钮
    headerButton_ = new QPushButton(this);
    headerButton_->setCursor(Qt::PointingHandCursor);
    headerButton_->setStyleSheet(
        "QPushButton {"
        "  text-align: left;"
        "  padding: 6px 12px;"
        "  border: 1px solid #E0E0E0;"
        "  border-radius: 8px;"
        "  background-color: #FFFDE7;"
        "  font-size: 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #FFF9C4;"
        "}"
    );
    connect(headerButton_, &QPushButton::clicked,
            this, &UiToolCallCard::toggleExpanded);
    mainLayout->addWidget(headerButton_);

    // 详情区域
    detailWidget_ = new QWidget(this);
    detailWidget_->setVisible(false);
    auto* detailLayout = new QVBoxLayout(detailWidget_);
    detailLayout->setContentsMargins(12, 4, 12, 8);
    detailLayout->setSpacing(6);

    // 参数
    auto* argsLabel = new QLabel(QString::fromUtf8("参数:"), detailWidget_);
    argsLabel->setStyleSheet("font-weight: bold; font-size: 11px; color: #616161;");
    detailLayout->addWidget(argsLabel);

    argsView_ = new QTextEdit(detailWidget_);
    argsView_->setReadOnly(true);
    argsView_->setMaximumHeight(100);
    argsView_->setStyleSheet(
        "QTextEdit {"
        "  background-color: #F5F5F5;"
        "  border: 1px solid #E0E0E0;"
        "  border-radius: 4px;"
        "  font-family: 'Consolas, Monaco, monospace';"
        "  font-size: 11px;"
        "  padding: 4px 8px;"
        "}"
    );
    if (!arguments_.isEmpty())
        argsView_->setPlainText(arguments_);
    detailLayout->addWidget(argsView_);

    // 状态
    statusLabel_ = new QLabel(detailWidget_);
    statusLabel_->setStyleSheet("font-size: 11px;");
    detailLayout->addWidget(statusLabel_);

    // 结果
    resultView_ = new QTextEdit(detailWidget_);
    resultView_->setReadOnly(true);
    resultView_->setMaximumHeight(120);
    resultView_->setStyleSheet(
        "QTextEdit {"
        "  background-color: #F5F5F5;"
        "  border: 1px solid #E0E0E0;"
        "  border-radius: 4px;"
        "  font-family: 'Consolas, Monaco, monospace';"
        "  font-size: 11px;"
        "  padding: 4px 8px;"
        "}"
    );
    detailLayout->addWidget(resultView_);

    mainLayout->addWidget(detailWidget_);
}

void UiToolCallCard::setState(EToolCallState state)
{
    state_ = state;
    updateAppearance();
}

void UiToolCallCard::setResult(const QString& result)
{
    result_ = result;
    if (resultView_)
        resultView_->setPlainText(result);
}

void UiToolCallCard::toggleExpanded()
{
    expanded_ = !expanded_;
    if (detailWidget_)
        detailWidget_->setVisible(expanded_);
    updateAppearance();
}

void UiToolCallCard::updateAppearance()
{
    QString icon;
    QString borderColor;
    QString bgColor;

    switch (state_)
    {
    case EToolCallState::eRunning:
        icon = QString::fromUtf8("\xF0\x9F\x94\xA7"); // 🔧
        statusLabel_->setText(QString::fromUtf8("状态: 执行中..."));
        statusLabel_->setStyleSheet("font-size: 11px; color: #1976D2;");
        borderColor = "#90CAF9";
        bgColor = "#E3F2FD";
        break;

    case EToolCallState::eSuccess:
        icon = QString::fromUtf8("\xE2\x9C\x85"); // ✅
        statusLabel_->setText(QString::fromUtf8("状态: 执行成功"));
        statusLabel_->setStyleSheet("font-size: 11px; color: #388E3C;");
        borderColor = "#A5D6A7";
        bgColor = "#E8F5E9";
        break;

    case EToolCallState::eError:
        icon = QString::fromUtf8("\xE2\x9D\x8C"); // ❌
        statusLabel_->setText(QString::fromUtf8("状态: 执行失败"));
        statusLabel_->setStyleSheet("font-size: 11px; color: #D32F2F;");
        borderColor = "#EF9A9A";
        bgColor = "#FFEBEE";
        break;
    }

    QString expandIcon = expanded_ ? QString::fromUtf8("\xE2\x96\xBC")   // ▼
                                   : QString::fromUtf8("\xE2\x96\xB6");  // ▶

    QString headerText = QString::fromUtf8("%1 %2 %3(%4)")
        .arg(expandIcon, icon, functionName_,
             arguments_.left(60) + (arguments_.length() > 60 ? "..." : ""));

    headerButton_->setText(headerText);
    headerButton_->setStyleSheet(
        QString(
            "QPushButton {"
            "  text-align: left;"
            "  padding: 6px 12px;"
            "  border: 1px solid %1;"
            "  border-radius: 8px;"
            "  background-color: %2;"
            "  font-size: 12px;"
            "}"
            "QPushButton:hover {"
            "  background-color: %2;"
            "  opacity: 0.8;"
            "}"
        ).arg(borderColor, bgColor)
    );
}

AST_NAMESPACE_END
