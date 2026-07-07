///
/// @file      UiNewObjectDialog.cpp
/// @brief     新建对象对话框实现
/// @details   ~
/// @author    axel
/// @date      2026-06-05
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

#include "UiNewObjectDialog.hpp"
#include "AstGUI/UiCommon.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/Class.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <unordered_set>

AST_NAMESPACE_BEGIN

UiNewObjectDialog::UiNewObjectDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("新建对象"));
    setMinimumSize(420, 480);
    setupUi();

    // 初始加载全部类型
    populateTypeList();

    // 默认选中第一个
    if (typeList_->count() > 0)
        typeList_->setCurrentRow(0);
}

UiNewObjectDialog::~UiNewObjectDialog() = default;

QString UiNewObjectDialog::selectedTypeName() const
{
    auto* item = typeList_->currentItem();
    return item ? item->data(Qt::UserRole).toString() : QString();
}

QString UiNewObjectDialog::objectName() const
{
    // 延迟到使用时才生成唯一名称（避免打开时阻塞）
    if (generatedName_.isEmpty())
        generatedName_ = generateUniqueName(selectedTypeName());
    return generatedName_;
}

void UiNewObjectDialog::setupUi()
{
    auto* rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(12, 12, 12, 12);
    rootLayout->setSpacing(8);

    // 搜索框
    searchEdit_ = new QLineEdit(this);
    searchEdit_->setPlaceholderText(tr("搜索类型名或描述..."));
    searchEdit_->setClearButtonEnabled(true);
    rootLayout->addWidget(searchEdit_);

    // 类型列表
    typeList_ = new QListWidget(this);
    typeList_->setIconSize(QSize(24, 24));
    typeList_->setMinimumHeight(180);
    rootLayout->addWidget(typeList_, 1);

    // 类型描述标签
    descLabel_ = new QLabel(this);
    descLabel_->setObjectName(QStringLiteral("NewObjectDescLabel"));
    descLabel_->setWordWrap(true);
    descLabel_->setMinimumHeight(36);
    rootLayout->addWidget(descLabel_);

    // 按钮
    auto* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    okButton_ = buttonBox->button(QDialogButtonBox::Ok);
    cancelButton_ = buttonBox->button(QDialogButtonBox::Cancel);
    okButton_->setText(tr("创建"));
    cancelButton_->setText(tr("取消"));
    okButton_->setEnabled(false);
    rootLayout->addWidget(buttonBox);

    // 连接 — 搜索
    connect(searchEdit_, &QLineEdit::textChanged, this, [this](const QString& text) {
        populateTypeList(text);
        if (typeList_->count() > 0)
            typeList_->setCurrentRow(0);
    });

    // 连接 — 选中变化：仅更新描述，不立即生成名称
    connect(typeList_, &QListWidget::currentItemChanged, this, [this](QListWidgetItem* current, QListWidgetItem*) {
        okButton_->setEnabled(current != nullptr);
        updateDescription();
        generatedName_.clear(); // 标记名称需重新生成
    });

    // 连接 — 双击创建
    connect(typeList_, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem*) {
        if (okButton_->isEnabled())
            accept();
    });

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void UiNewObjectDialog::populateTypeList(const QString& filter)
{
    // 首次调用时，收集所有可创建类型
    if (allClasses_.empty())
    {
        const auto& allClasses = aGetAllClasses();
        allClasses_.reserve(allClasses.size());
        for (const auto& pair : allClasses)
        {
            Class* cls = pair.second;
            if (!cls || cls->isVirtual())
                continue;

            ClassInfo info;
            info.name = cls->getName();
            info.desc = cls->desc();
            info.cls  = cls;
            allClasses_.push_back(std::move(info));
        }

        // 按类型名排序
        std::sort(allClasses_.begin(), allClasses_.end(),
                  [](const ClassInfo& a, const ClassInfo& b) {
                      return a.name < b.name;
                  });
    }

    // 禁用更新以避免批量添加时反复重绘
    typeList_->setUpdatesEnabled(false);
    typeList_->clear();

    QString lowerFilter = filter.trimmed().toLower();
    for (const auto& info : allClasses_)
    {
        if (!lowerFilter.isEmpty())
        {
            QString typeName = QString::fromStdString(info.name);
            QString desc     = QString::fromStdString(info.desc);
            if (!typeName.toLower().contains(lowerFilter)
                && !desc.toLower().contains(lowerFilter))
                continue;
        }

        QString typeName = QString::fromStdString(info.name);
        QIcon icon = aUiClassIcon(typeName);

        auto* item = new QListWidgetItem(icon, typeName);
        item->setData(Qt::UserRole, typeName);

        // tooltip 中显示描述和父类
        QString tooltip = tr("类型: %1").arg(typeName);
        if (!info.desc.empty())
            tooltip += QStringLiteral("\n") + tr("描述: %1").arg(QString::fromStdString(info.desc));
        Class* parentCls = info.cls->getParent();
        if (parentCls)
            tooltip += QStringLiteral("\n") + tr("父类: %1").arg(QString::fromStdString(parentCls->getName()));
        item->setToolTip(tooltip);

        // 存储描述用于 tooltip/搜索
        item->setData(Qt::UserRole + 1, QString::fromStdString(info.desc));

        typeList_->addItem(item);
    }

    typeList_->setUpdatesEnabled(true);
}

void UiNewObjectDialog::updateDescription()
{
    auto* item = typeList_->currentItem();
    if (!item)
    {
        descLabel_->clear();
        return;
    }

    QString desc = item->data(Qt::UserRole + 1).toString();
    if (desc.isEmpty())
        descLabel_->setText(tr("（无描述）"));
    else
        descLabel_->setText(desc);
}

QString UiNewObjectDialog::generateUniqueName(const QString& typeName) const
{
    if (typeName.isEmpty())
        return QString();

    // 收集所有现有对象名称
    auto& mgr = ObjectManager::CurrentInstance();
    auto allObjs = mgr.getAllObjects();
    std::unordered_set<std::string> existingNames;
    existingNames.reserve(allObjs.size());
    for (auto* obj : allObjs)
    {
        if (obj)
            existingNames.insert(obj->getName());
    }

    std::string baseName = typeName.toStdString();

    // 若基名未被占用，直接使用
    if (existingNames.find(baseName) == existingNames.end())
        return typeName;

    // 否则递增数字后缀，直到找到未占用的名称
    for (int i = 1; i < 9999; ++i)
    {
        std::string candidate = baseName + std::to_string(i);
        if (existingNames.find(candidate) == existingNames.end())
            return QString::fromStdString(candidate);
    }

    // 极端回退
    return typeName + QStringLiteral("_new");
}

AST_NAMESPACE_END
