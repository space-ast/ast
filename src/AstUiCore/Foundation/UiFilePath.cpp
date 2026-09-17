///
/// @file      UiFilePath.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-04
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

#include "UiFilePath.hpp"
#include "AstUtil/StringView.hpp"
#include <QHBoxLayout>
#include <QPushButton>
#include <QFileDialog>

AST_NAMESPACE_BEGIN

UiFilePath::UiFilePath(QWidget* parent)
    : QWidget(parent)
{
    // 创建主布局
    QHBoxLayout* layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    
    // 创建输入框
    lineEdit_ = new UiValueEdit(this);
    
    // 创建浏览按钮
    QPushButton* browseButton = new QPushButton(tr("浏览"), this);
    browseButton->setFixedWidth(60);
    
    // 将输入框和按钮添加到布局
    layout->addWidget(lineEdit_, 1);
    layout->addWidget(browseButton);
    
    // 设置布局
    setLayout(layout);
    
    // 连接信号
    connect(browseButton, &QPushButton::clicked, this, &UiFilePath::onBrowse);
    connect(lineEdit_, &QLineEdit::editingFinished, this, &UiFilePath::onEditingFinished);
}

void UiFilePath::setPath(const QString& path)
{
    currentPath_ = path;
    lineEdit_->setText(path);
    lineEdit_->setNormal();
}

void UiFilePath::setPath(StringView path)
{
    setPath(QString::fromUtf8(path.data(), (int)path.size()));
}

QString UiFilePath::path() const
{
    return currentPath_;
}

void UiFilePath::setFilter(const QString& filter)
{
    filter_ = filter;
}

QString UiFilePath::filter() const
{
    return filter_;
}

void UiFilePath::onBrowse()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        tr("选择文件"),
        currentPath_,
        filter_.isEmpty() ? tr("所有文件 (*.*)") : filter_
    );
    
    if (!fileName.isEmpty())
    {
        setPath(fileName);
        emit pathChanged(fileName);
    }
}

void UiFilePath::onEditingFinished()
{
    QString text = lineEdit_->text();
    if (text != currentPath_)
    {
        currentPath_ = text;
        emit pathChanged(text);
    }
    lineEdit_->setNormal();
}

AST_NAMESPACE_END