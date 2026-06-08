///
/// @file      UiNewObjectQuickDialog.cpp
/// @brief     快速新建对象对话框实现
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

#include "UiNewObjectQuickDialog.hpp"
#include "UiNewObjectDialog.hpp"

#include "AstGUI/UiCommon.hpp"
#include "AstUtil/Object.hpp"
#include "AstUtil/ObjectManager.hpp"
#include "AstUtil/ObjectNode.hpp"
#include "AstUtil/RTTIAPI.hpp"
#include "AstUtil/StringView.hpp"

#include <QDialogButtonBox>
#include <QEvent>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include <unordered_set>

AST_NAMESPACE_BEGIN

// ============================================================================
// 可快速创建的类型配置（按需增删改）
// ============================================================================

std::vector<QuickTypeEntry> UiNewObjectQuickDialog::quickTypes()
{
    return {
        // 实体对象
        { "Spacecraft",      u8"实体对象",   nullptr },
        { "Facility",        u8"实体对象",   nullptr },
        { "CelestialBody",   u8"实体对象",   nullptr },
        { "Sensor",          u8"实体对象",   "Point" },

        { "MainSequence",    u8"分析工具",   nullptr },

        // 分析
        { "SweepStudy",      u8"分析工具",   "StudyWorkbench" },
        { "StudyWorkbench",  u8"分析工具",   nullptr },
    };
}

// ============================================================================
// 构造 / 析构
// ============================================================================

UiNewObjectQuickDialog::UiNewObjectQuickDialog(QWidget* parent)
    : QDialog(parent)
{
    setWindowTitle(tr("新建对象"));
    resize(640, 520);
    setupUi();
}

UiNewObjectQuickDialog::~UiNewObjectQuickDialog() = default;

// ============================================================================
// 界面搭建
// ============================================================================

void UiNewObjectQuickDialog::setupUi()
{
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(12, 12, 12, 12);
    root->setSpacing(8);

    // ---- 顶部：高级按钮 ----
    auto* topBar = new QHBoxLayout();
    topBar->addStretch();
    advancedBtn_ = new QPushButton(tr("高级 >>"), this);
    advancedBtn_->setToolTip(tr("打开完整类型列表"));
    connect(advancedBtn_, &QPushButton::clicked, this, &UiNewObjectQuickDialog::onAdvanced);
    topBar->addWidget(advancedBtn_);
    root->addLayout(topBar);

    // ---- 中部：类型表格 ----
    typeTable_ = new QTableWidget(this);
    typeTable_->setColumnCount(4);
    typeTable_->horizontalHeader()->setVisible(false);
    typeTable_->horizontalHeader()->setStretchLastSection(true);
    typeTable_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    typeTable_->verticalHeader()->setVisible(false);
    typeTable_->setSelectionMode(QAbstractItemView::SingleSelection);
    typeTable_->setSelectionBehavior(QAbstractItemView::SelectItems);
    typeTable_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    typeTable_->setShowGrid(false);
    typeTable_->setFocusPolicy(Qt::NoFocus);

    buildTypeGrid();

    root->addWidget(typeTable_, 1);

    // ---- 类型描述 ----
    typeDescLabel_ = new QLabel(this);
    typeDescLabel_->setWordWrap(true);
    typeDescLabel_->setStyleSheet("color: #666; padding: 4px 8px;");
    root->addWidget(typeDescLabel_);

    // ---- 底部：名称 + 父对象（同一行） ----
    auto* infoLayout = new QHBoxLayout();
    infoLayout->addWidget(new QLabel(tr("名称:"), this));
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setPlaceholderText(tr("自动生成唯一名称"));
    infoLayout->addWidget(nameEdit_, 1);

    parentRow_ = new QWidget(this);
    parentRow_->setVisible(false);
    auto* parentLayout = new QHBoxLayout(parentRow_);
    parentLayout->setContentsMargins(0, 0, 0, 0);
    parentLayout->addWidget(new QLabel(tr("父对象:"), parentRow_));
    parentEdit_ = new QLineEdit(parentRow_);
    parentEdit_->setReadOnly(true);
    parentEdit_->setPlaceholderText(tr("点击选择父对象..."));
    parentEdit_->installEventFilter(this);
    parentLayout->addWidget(parentEdit_, 1);
    infoLayout->addWidget(parentRow_);

    root->addLayout(infoLayout);

    // ---- 底部：按钮 ----
    auto* btnBar = new QHBoxLayout();
    btnBar->addStretch();

    auto* cancelBtn = new QPushButton(tr("取消"), this);
    connect(cancelBtn, &QPushButton::clicked, this, &QDialog::reject);
    btnBar->addWidget(cancelBtn);

    createBtn_ = new QPushButton(tr("创建"), this);
    createBtn_->setDefault(true);
    createBtn_->setEnabled(false);
    connect(createBtn_, &QPushButton::clicked, this, &UiNewObjectQuickDialog::onCreate);
    btnBar->addWidget(createBtn_);

    root->addLayout(btnBar);
}

// ============================================================================
// 类型表格
// ============================================================================

static const int kTableColumns = 4;

void UiNewObjectQuickDialog::buildTypeGrid()
{
    auto types = quickTypes();

    // 先计算需要多少行
    QString currentCategory;
    int row = 0;
    int col = 0;

    for (const auto& entry : types)
    {
        QString category = QString::fromUtf8(entry.category);
        if (category != currentCategory)
        {
            if (col > 0) row++;
            currentCategory = category;
            row++; // 分类标题行
            col = 0;
        }
        col++;
        if (col >= kTableColumns) { col = 0; row++; }
    }
    typeTable_->setRowCount(row + 1);

    // 填充表格
    currentCategory.clear();
    row = 0;
    col = 0;

    for (size_t i = 0; i < types.size(); ++i)
    {
        const auto& entry = types[i];
        QString category = QString::fromUtf8(entry.category);

        // 新分类：添加标题行
        if (category != currentCategory)
        {
            if (col > 0) row++;
            currentCategory = category;

            auto* header = new QTableWidgetItem(category);
            header->setFlags(Qt::NoItemFlags);
            header->setData(Qt::ForegroundRole, QColor(100, 100, 100));
            QFont f = header->font();
            f.setBold(true);
            header->setFont(f);
            typeTable_->setItem(row, 0, header);
            typeTable_->setSpan(row, 0, 1, kTableColumns);
            row++;
            col = 0;
        }

        // 类型条目（图标 + 名称）
        auto cls = aGetClass(entry.typeName);
        QString typeName = QString::fromStdString(cls?cls->displayName():entry.typeName);
        auto* item = new QTableWidgetItem(aUiClassIcon(entry.typeName), typeName);
        item->setData(Qt::UserRole, QVariant::fromValue<int>((int)i));
        item->setToolTip(typeName);
        typeTable_->setItem(row, col, item);

        col++;
        if (col >= kTableColumns) { col = 0; row++; }
    }

    connect(typeTable_, &QTableWidget::currentItemChanged,
            this, [this](QTableWidgetItem* current, QTableWidgetItem*)
    {
        if (!current || current->flags() == Qt::NoItemFlags)
            return;
        int idx = current->data(Qt::UserRole).value<int>();
        onTypeCardClicked(idx);
    });

    // 双击：创建对象但保持对话框打开，以便连续创建
    connect(typeTable_, &QTableWidget::cellDoubleClicked,
            this, [this](int /*row*/, int /*col*/)
    {
        auto types = quickTypes();
        if (selectedEntry_ < 0 || selectedEntry_ >= (int)types.size())
            return;

        const auto& entry = types[selectedEntry_];
        if (entry.parentType)
        {
            Object* parent = showParentDialog(entry.parentType);
            if (!parent)
                return;
            selectedParent_ = parent;
            parentEdit_->setText(aUiObjectDisplayName(parent));
        }

        if (tryCreate())
        {
            // 刷新名称以供下次创建，保持选中状态
            QString typeName = QString::fromStdString(entry.typeName);
            nameEdit_->setText(generateUniqueName(typeName));
            if (entry.parentType)
            {
                selectedParent_ = nullptr;
                parentEdit_->clear();
                createBtn_->setEnabled(false);
            }
        }
    });
}

// ============================================================================
// 交互逻辑
// ============================================================================

void UiNewObjectQuickDialog::onTypeCardClicked(int entryIndex)
{
    auto types = quickTypes();
    if (entryIndex < 0 || entryIndex >= (int)types.size())
        return;

    selectedEntry_ = entryIndex;
    selectedParent_ = nullptr;

    const auto& entry = types[entryIndex];

    // 自动生成名称
    QString typeName = QString::fromStdString(entry.typeName);
    nameEdit_->setText(generateUniqueName(typeName));

    // 父对象选择
    bool needsParent = (entry.parentType != nullptr);
    parentRow_->setVisible(needsParent);
    if (needsParent)
        parentEdit_->clear();
    createBtn_->setEnabled(!nameEdit_->text().isEmpty() && !needsParent);

    // 类型描述
    auto cls = aGetClass(entry.typeName);
    if (cls && !cls->desc().empty())
        typeDescLabel_->setText(QString::fromStdString(cls->desc()));
    else
        typeDescLabel_->clear();
}

void UiNewObjectQuickDialog::onChooseParent()
{
    auto types = quickTypes();
    if (selectedEntry_ < 0 || selectedEntry_ >= (int)types.size())
        return;

    const char* parentType = types[selectedEntry_].parentType;
    Object* parent = showParentDialog(parentType);
    if (parent)
    {
        selectedParent_ = parent;
        parentEdit_->setText(aUiObjectDisplayName(parent));
        createBtn_->setEnabled(!nameEdit_->text().isEmpty());
    }
}

Object* UiNewObjectQuickDialog::showParentDialog(const char* parentType)
{
    auto* dlg = new QDialog(this);
    dlg->setWindowTitle(tr("选择父对象"));
    dlg->resize(400, 300);
    auto* layout = new QVBoxLayout(dlg);
    layout->setContentsMargins(8, 8, 8, 8);

    auto* tree = new QTreeWidget(dlg);
    tree->setHeaderHidden(true);
    tree->setSelectionMode(QAbstractItemView::SingleSelection);

    auto& mgr = ObjectManager::CurrentInstance();
    auto rootObjects = mgr.getRootObjects();
    for (auto* obj : rootObjects)
    {
        if (!obj) continue;
        populateParentTree(tree, tree->invisibleRootItem(), obj, parentType);
    }
    tree->expandAll();
    layout->addWidget(tree);

    auto* btnBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dlg);
    btnBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    connect(tree, &QTreeWidget::currentItemChanged,
            this, [&](QTreeWidgetItem* current, QTreeWidgetItem*)
    {
        btnBox->button(QDialogButtonBox::Ok)->setEnabled(
            current && (current->flags() & Qt::ItemIsSelectable));
    });
    connect(tree, &QTreeWidget::itemDoubleClicked,
            this, [dlg](QTreeWidgetItem* item, int)
    {
        if (item && (item->flags() & Qt::ItemIsSelectable))
            dlg->accept();
    });
    connect(btnBox, &QDialogButtonBox::accepted, dlg, &QDialog::accept);
    connect(btnBox, &QDialogButtonBox::rejected, dlg, &QDialog::reject);
    layout->addWidget(btnBox);

    if (dlg->exec() != QDialog::Accepted)
        return nullptr;

    auto* current = tree->currentItem();
    return current
        ? reinterpret_cast<Object*>(current->data(0, Qt::UserRole).value<quintptr>())
        : nullptr;
}

void UiNewObjectQuickDialog::populateParentTree(
    QTreeWidget* tree, QTreeWidgetItem* parentItem, Object* obj, const char* parentType)
{
    if (!obj)
        return;

    bool matches = obj->isOfType(StringView(parentType));
    auto& mgr = ObjectManager::CurrentInstance();
    auto* node = mgr.getObjectNode(obj);

    if (matches)
    {
        // 匹配的对象：创建可选择节点
        auto* item = new QTreeWidgetItem();
        item->setText(0, aUiObjectDisplayName(obj));
        item->setIcon(0, aUiClassIcon(QString::fromStdString(obj->typeName())));
        item->setData(0, Qt::UserRole,
                      QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(obj)));

        if (parentItem)
            parentItem->addChild(item);
        else
            tree->addTopLevelItem(item);

        // 递归处理子节点
        if (node)
        {
            for (auto* childNode : node->getChildren())
            {
                if (auto* childObj = childNode->getObject())
                    populateParentTree(tree, item, childObj, parentType);
            }
        }
    }
    else
    {
        // 不匹配的对象直接隐藏，但继续递归子节点以查找嵌套的匹配项
        if (node)
        {
            for (auto* childNode : node->getChildren())
            {
                if (auto* childObj = childNode->getObject())
                    populateParentTree(tree, parentItem, childObj, parentType);
            }
        }
    }
}

void UiNewObjectQuickDialog::onAdvanced()
{
    UiNewObjectDialog dlg(this);
    if (dlg.exec() != QDialog::Accepted)
        return;

    QString typeName = dlg.selectedTypeName();
    QString objName = dlg.objectName();
    if (typeName.isEmpty())
        return;

    Object* obj = aNewObject(StringView(typeName.toStdString()));
    if (!obj)
    {
        QMessageBox::warning(this, tr("创建失败"),
                             tr("无法创建类型为 \"%1\" 的对象。").arg(typeName));
        return;
    }

    obj->setName(StringView(objName.toStdString()));
    aAddObject(obj);
    createdObjectName_ = QString::fromStdString(obj->displayName());
    accept();
}

bool UiNewObjectQuickDialog::tryCreate()
{
    auto types = quickTypes();
    if (selectedEntry_ < 0 || selectedEntry_ >= (int)types.size())
        return false;

    const auto& entry = types[selectedEntry_];
    QString typeName = QString::fromStdString(entry.typeName);
    QString objName = nameEdit_->text().trimmed();
    if (objName.isEmpty())
    {
        QMessageBox::warning(this, tr("名称不能为空"), tr("请输入对象名称。"));
        return false;
    }

    Object* parentScope = nullptr;
    if (entry.parentType && selectedParent_)
        parentScope = selectedParent_;

    Object* obj = aNewObject(StringView(typeName.toStdString()), parentScope);
    if (!obj)
    {
        QMessageBox::warning(this, tr("创建失败"),
                             tr("无法创建类型为 \"%1\" 的对象。").arg(typeName));
        return false;
    }

    obj->setName(StringView(objName.toStdString()));
    aAddObject(obj);
    createdObjectName_ = QString::fromStdString(obj->displayName());
    emit objectCreated();
    return true;
}

void UiNewObjectQuickDialog::onCreate()
{
    if (tryCreate())
        accept();
}

// ============================================================================
// 工具函数
// ============================================================================

bool UiNewObjectQuickDialog::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == parentEdit_ && event->type() == QEvent::MouseButtonPress)
    {
        onChooseParent();
        return true;
    }
    return QDialog::eventFilter(obj, event);
}

QString UiNewObjectQuickDialog::createdObjectName() const
{
    return createdObjectName_;
}

QString UiNewObjectQuickDialog::generateUniqueName(const QString& typeName) const
{
    auto& mgr = ObjectManager::CurrentInstance();
    auto allObjs = mgr.getAllObjects();

    std::unordered_set<std::string> existingNames;
    for (auto* obj : allObjs)
    {
        if (obj)
            existingNames.insert(obj->getName());
    }

    std::string base = typeName.toStdString();
    if (existingNames.find(base) == existingNames.end())
        return typeName;

    for (int i = 1; ; ++i)
    {
        std::string candidate = base + std::to_string(i);
        if (existingNames.find(candidate) == existingNames.end())
            return QString::fromStdString(candidate);
    }
}

AST_NAMESPACE_END
