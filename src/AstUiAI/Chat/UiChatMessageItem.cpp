///
/// @file      UiChatMessageItem.cpp
/// @brief     单条聊天消息气泡部件实现
/// @author    axel
/// @date      2026-06-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiChatMessageItem.hpp"
#include "AstAI/ChatMessage.hpp"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QScrollBar>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QResizeEvent>

AST_NAMESPACE_BEGIN

UiChatMessageItem::UiChatMessageItem(EChatRole role, const QString& content, QWidget* parent)
    : QWidget(parent)
    , role_(role)
{
    setupUi();
    applyRoleStyle();
    if (!content.isEmpty())
        setContent(content);
}

UiChatMessageItem::~UiChatMessageItem() = default;

void UiChatMessageItem::setupUi()
{
    auto* outerLayout = new QHBoxLayout(this);
    outerLayout->setContentsMargins(8, 4, 8, 4);

    // 角色标签
    roleLabel_ = new QLabel(this);
    roleLabel_->setFixedWidth(42);
    roleLabel_->setAlignment(Qt::Alignment(Qt::AlignTop | Qt::AlignHCenter));
    roleLabel_->setStyleSheet("font-weight: bold; padding-top: 6px;");

    // 消息内容 — 初始高度由文档内容决定
    contentEdit_ = new QTextEdit(this);
    contentEdit_->setReadOnly(true);
    contentEdit_->setFrameShape(QFrame::NoFrame);
    contentEdit_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    contentEdit_->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    contentEdit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    contentEdit_->setMinimumHeight(28);

    // 文档内容变化时自动调整高度
    connect(contentEdit_->document()->documentLayout(),
            &QAbstractTextDocumentLayout::documentSizeChanged,
            this, &UiChatMessageItem::updateContentHeight);
    connect(contentEdit_->document(), &QTextDocument::contentsChanged,
            this, &UiChatMessageItem::updateContentHeight);

    outerLayout->addWidget(roleLabel_);
    outerLayout->addWidget(contentEdit_, 4);
    outerLayout->addStretch(1);
}

void UiChatMessageItem::applyRoleStyle()
{
    QString bubbleStyle;
    QString textColor;

    switch (role_)
    {
    case EChatRole::eUser:
        roleLabel_->setText(QString::fromUtf8("你"));
        roleLabel_->setStyleSheet("font-weight: bold; color: #1976D2; padding-top: 6px;");
        bubbleStyle =
            "QTextEdit {"
            "  background-color: #E3F2FD;"
            "  border: 1px solid #90CAF9;"
            "  border-radius: 12px;"
            "  padding: 8px 14px;"
            "  font-size: 13px;"
            "}";
        textColor = "#0D47A1";
        break;

    case EChatRole::eAssistant:
        roleLabel_->setText(QString::fromUtf8("助手"));
        roleLabel_->setStyleSheet("font-weight: bold; color: #388E3C; padding-top: 6px;");
        bubbleStyle =
            "QTextEdit {"
            "  background-color: #F5F5F5;"
            "  border: 1px solid #E0E0E0;"
            "  border-radius: 12px;"
            "  padding: 8px 14px;"
            "  font-size: 13px;"
            "}";
        textColor = "#212121";
        break;

    case EChatRole::eSystem:
        roleLabel_->setText(QString::fromUtf8("系统"));
        roleLabel_->setStyleSheet("font-weight: bold; color: #9E9E9E; padding-top: 6px;");
        bubbleStyle =
            "QTextEdit {"
            "  background-color: #FAFAFA;"
            "  border: 1px dashed #BDBDBD;"
            "  border-radius: 8px;"
            "  padding: 6px 10px;"
            "  font-size: 12px;"
            "  font-style: italic;"
            "}";
        textColor = "#757575";
        break;

    case EChatRole::eTool:
        roleLabel_->setText(QString::fromUtf8("工具"));
        roleLabel_->setStyleSheet("font-weight: bold; color: #FF8F00; padding-top: 6px;");
        bubbleStyle =
            "QTextEdit {"
            "  background-color: #FFF8E1;"
            "  border: 1px solid #FFE082;"
            "  border-radius: 8px;"
            "  padding: 6px 10px;"
            "  font-size: 12px;"
            "  font-family: 'Consolas, Monaco, monospace';"
            "}";
        textColor = "#E65100";
        break;
    }

    contentEdit_->setStyleSheet(bubbleStyle + QString(" color: %1;").arg(textColor));

    // 根据角色调整布局方向
    auto* layout = qobject_cast<QHBoxLayout*>(this->layout());
    if (!layout) return;

    // 清空布局
    while (layout->count() > 0)
    {
        QLayoutItem* item = layout->takeAt(0);
        if (item->widget())
            layout->removeWidget(item->widget());
        delete item;
    }

    if (role_ == EChatRole::eUser)
    {
        // 用户消息：stretch(1) | 气泡(stretch=2) | 角色标签(42)
        layout->addStretch(1);
        layout->addWidget(contentEdit_, 4);
        layout->addWidget(roleLabel_);
        contentEdit_->setAlignment(Qt::Alignment(Qt::AlignRight | Qt::AlignVCenter));
    }
    else
    {
        // 其他消息：角色标签(42) | 气泡(stretch=2) | stretch(1)
        layout->addWidget(roleLabel_);
        layout->addWidget(contentEdit_, 4);
        layout->addStretch(1);
        contentEdit_->setAlignment(Qt::Alignment(Qt::AlignLeft | Qt::AlignVCenter));
    }
}

void UiChatMessageItem::setContent(const QString& content)
{
    accumulatedHtml_.clear();
    contentEdit_->setHtml(content);
    updateContentHeight();
}

void UiChatMessageItem::appendHtml(const QString& html)
{
    // 累积 HTML 片段，用完整内容 setHtml（避免 insertHtml 造成的增量布局问题）
    accumulatedHtml_ += html;
    contentEdit_->setHtml(accumulatedHtml_);

    updateContentHeight();

    // 自动滚动到底部
    QScrollBar* sb = contentEdit_->verticalScrollBar();
    if (sb)
        sb->setValue(sb->maximum());
}

QString UiChatMessageItem::content() const
{
    return contentEdit_->toHtml();
}

QString UiChatMessageItem::roleDisplayName(EChatRole role)
{
    switch (role)
    {
    case EChatRole::eUser:      return QString::fromUtf8("你");
    case EChatRole::eAssistant: return QString::fromUtf8("助手");
    case EChatRole::eSystem:    return QString::fromUtf8("系统");
    case EChatRole::eTool:      return QString::fromUtf8("工具");
    default:                    return QString::fromUtf8("未知");
    }
}

// ============================================================
//  自适应高度 — 根据文档内容动态调整 QTextEdit 高度
// ============================================================

void UiChatMessageItem::updateContentHeight()
{
    QTextDocument* doc = contentEdit_->document();
    if (!doc) return;

    // 将文档宽度约束为 viewport 宽度，让文档可以正确计算换行后的高度
    int viewportWidth = contentEdit_->viewport()->width();
    if (viewportWidth > 0)
        doc->setTextWidth(viewportWidth);

    // 获取文档理想高度（包含 padding）
    int docHeight = static_cast<int>(doc->size().height());

    // 加上 QTextEdit 内部的 frame/padding 余量
    int frameExtra = contentEdit_->contentsMargins().top()
                   + contentEdit_->contentsMargins().bottom()
                   + static_cast<int>(contentEdit_->document()->documentMargin() * 2)
                   + 6; // 额外 buffer

    int totalHeight = docHeight + frameExtra;

    // 避免相同高度时循环触发 layout
    if (totalHeight == cachedDocHeight_) return;
    cachedDocHeight_ = totalHeight;

    contentEdit_->setMinimumHeight(totalHeight);
    contentEdit_->setMaximumHeight(totalHeight);

    // 通知布局系统重新计算
    updateGeometry();
}

void UiChatMessageItem::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    // 宽度变化时（如拖拽 dock），需要重新计算文档换行高度
    if (event->size().width() != event->oldSize().width())
        updateContentHeight();
}

void UiChatMessageItem::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
}

AST_NAMESPACE_END
