///
/// @file      UiScenarioWizard.cpp
/// @brief     新建场景向导对话框实现
/// @details   ~
/// @author    axel
/// @date      2026-09-07
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

#include "UiScenarioWizard.hpp"
#include "UiCentralBodies.hpp"
#include "AstGUI/UiTimePoint.hpp"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

AST_NAMESPACE_BEGIN

UiScenarioWizard::UiScenarioWizard(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("新建场景向导"));

    setupUi();
    applyTimeDefaults();
    setupConnections();
}

UiScenarioWizard::~UiScenarioWizard() = default;

// ============================================================================
// 公共接口
// ============================================================================

QString UiScenarioWizard::scenarioName() const
{
    return nameEdit_->text().trimmed();
}

QString UiScenarioWizard::scenarioDescription() const
{
    return descEdit_->toPlainText();
}

QString UiScenarioWizard::scenarioLocation() const
{
    return locationEdit_->text().trimmed();
}

TimePoint UiScenarioWizard::startTime() const
{
    return startEdit_->getTimePoint();
}

TimePoint UiScenarioWizard::stopTime() const
{
    return stopEdit_->getTimePoint();
}

QString UiScenarioWizard::centralBody() const
{
    return centralBodyCombo_->currentData().toString();
}

// ============================================================================
// 界面搭建
// ============================================================================

void UiScenarioWizard::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(14, 14, 14, 14);
    rootLayout->setSpacing(8);

    // 通用表单布局：左列标签，右列控件
    auto* grid = new QGridLayout();
    grid->setHorizontalSpacing(10);
    grid->setVerticalSpacing(10);
    grid->setColumnStretch(1, 1);

    int row = 0;

    // ——— 名称 ———
    grid->addWidget(new QLabel(tr("名称:"), this), row, 0, Qt::AlignRight);
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setText(tr("场景 1"));
    grid->addWidget(nameEdit_, row, 1, 1, 2);
    ++row;

    // ——— 描述 ———
    grid->addWidget(new QLabel(tr("描述:"), this), row, 0, Qt::AlignRight);
    descEdit_ = new QTextEdit(this);
    descEdit_->setPlaceholderText(tr("<输入场景描述>"));
    descEdit_->setMinimumHeight(40);
    descEdit_->setMaximumHeight(80);
    grid->addWidget(descEdit_, row, 1, 1, 2);
    ++row;

    // ——— 存储位置 ———
    grid->addWidget(new QLabel(tr("位置:"), this), row, 0, Qt::AlignRight);
    locationEdit_ = new QLineEdit(this);
    locationEdit_->setText(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation)
                           + QStringLiteral("/Scenarios"));
    auto* browseButton = new QPushButton(QStringLiteral("..."), this);
    browseButton->setObjectName(QStringLiteral("browseLocation"));
    browseButton->setFixedWidth(36);
    grid->addWidget(locationEdit_, row, 1);
    grid->addWidget(browseButton, row, 2);
    ++row;

    // ——— 起止时间———
    grid->addWidget(new QLabel(tr("起始:"), this), row, 0, Qt::AlignRight);
    startEdit_ = new UiTimePoint(this);
    startEdit_->setMinimumWidth(250);
    grid->addWidget(startEdit_, row, 1);
    ++row;

    grid->addWidget(new QLabel(tr("终止:"), this), row, 0, Qt::AlignRight);
    stopEdit_ = new UiTimePoint(this);
    stopEdit_->setMinimumWidth(250);
    grid->addWidget(stopEdit_, row, 1);
    ++row;

    // ——— 中心天体 ———
    grid->addWidget(new QLabel(tr("中心天体:"), this), row, 0, Qt::AlignRight);
    centralBodyCombo_ = new QComboBox(this);
    for (const auto& item : kCentralBodies)
        centralBodyCombo_->addItem(QString::fromUtf8(item.cn), QString::fromUtf8(item.id));
    int earthIdx = centralBodyCombo_->findData(QStringLiteral("Earth"));
    if (earthIdx >= 0)
        centralBodyCombo_->setCurrentIndex(earthIdx);
    grid->addWidget(centralBodyCombo_, row, 1);
    ++row;

    rootLayout->addLayout(grid);

    // ——— 按钮 ———
    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    okButton_ = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton_ = buttonBox->button(QDialogButtonBox::Cancel);
    okButton_->setText(tr("确定"));
    cancelButton_->setText(tr("取消"));
    okButton_->setDefault(true);
    rootLayout->addWidget(buttonBox);
}

void UiScenarioWizard::setupConnections()
{
    connect(cancelButton_, &QPushButton::clicked, this, &QDialog::reject);

    // OK：校验必填项后接受
    connect(okButton_, &QPushButton::clicked, this, [this]() {
        if (scenarioName().isEmpty())
        {
            QMessageBox::warning(this, tr("提示"), tr("请输入场景名称。"));
            nameEdit_->setFocus();
            return;
        }
        if (stopEdit_->getTimePoint() <= startEdit_->getTimePoint())
        {
            QMessageBox::warning(this, tr("提示"), tr("终止时间必须晚于起始时间。"));
            return;
        }
        accept();
    });

    // 位置浏览按钮
    if (auto* btn = findChild<QPushButton*>(QStringLiteral("browseLocation")))
        connect(btn, &QPushButton::clicked, this, &UiScenarioWizard::browseLocation);
}

// ============================================================================
// 默认时间
// ============================================================================

void UiScenarioWizard::applyTimeDefaults()
{
    TimePoint start = TimePoint::TodayUTC();
    TimePoint stop  = start + 86400.0;
    startEdit_->setTimePoint(start);
    stopEdit_->setTimePoint(stop);
}

// ============================================================================
// 其他槽
// ============================================================================

void UiScenarioWizard::browseLocation()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择场景目录"),
                                                    locationEdit_->text());
    if (!dir.isEmpty())
        locationEdit_->setText(dir);
}

AST_NAMESPACE_END
