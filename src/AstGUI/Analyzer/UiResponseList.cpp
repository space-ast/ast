///
/// @file      UiResponseList.cpp
/// @brief     响应列表面板实现
/// @details   ~
/// @author    axel
/// @date      2026-05-17
/// @copyright 版权所有 (C) 2026-present, SpaceAST项目.
///
/// SpaceAST项目（https://github.com/space-ast/ast）
/// 本软件基于 Apache 2.0 开源许可证分发。

#include "UiResponseList.hpp"
#include "AstAnalyzer/AnalyzerConstraint.hpp"

AST_NAMESPACE_BEGIN

UiResponseList::UiResponseList(QWidget* parent)
    : QWidget(parent)
{
    setupUi();
}

void UiResponseList::setupUi()
{
    layout_ = new QVBoxLayout(this);
    layout_->setContentsMargins(0, 0, 0, 0);

    listWidget_ = new QListWidget(this);
    listWidget_->setSelectionMode(QAbstractItemView::SingleSelection);
    layout_->addWidget(listWidget_);

    addButton_ = new QPushButton(tr("+ 添加响应"), this);
    layout_->addWidget(addButton_);

    connect(listWidget_, &QListWidget::currentItemChanged,
            this, &UiResponseList::onSelectionChanged);
    connect(addButton_, &QPushButton::clicked,
            this, &UiResponseList::addResponseRequested);
}

void UiResponseList::setResponses(const std::vector<SharedPtr<AnalyzerConstraint>>& responses)
{
    responses_ = responses;
    listWidget_->clear();
    for (auto& resp : responses)
    {
        QListWidgetItem* item = new QListWidgetItem(
            QString::fromStdString(resp->getName()), listWidget_);
        item->setData(Qt::UserRole, QString::fromStdString(resp->getName()));
    }
}

AnalyzerConstraint* UiResponseList::selectedResponse() const
{
    auto item = listWidget_->currentItem();
    if (!item) return nullptr;
    QString name = item->data(Qt::UserRole).toString();
    for (auto& resp : responses_)
    {
        if (QString::fromStdString(resp->getName()) == name)
            return resp.get();
    }
    return nullptr;
}

void UiResponseList::onSelectionChanged()
{
    emit responseSelected(selectedResponse());
}

AST_NAMESPACE_END
