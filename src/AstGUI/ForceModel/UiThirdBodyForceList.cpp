#include "UiThirdBodyForceList.hpp"
#include "AstCore/ThirdBodyForce.hpp"
#include "AstCore/HPOPForceModel.hpp"
#include "AstCore/CelestialBody.hpp"
#include <QMessageBox>

AST_NAMESPACE_BEGIN

UiThirdBodyForceList::UiThirdBodyForceList(Object* object, QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiThirdBodyForceList");
    setupUi();
    if (object) {
        setObject(object);
        setHPOPForceModel(dynamic_cast<HPOPForceModel*>(object));
    }
}

UiThirdBodyForceList::UiThirdBodyForceList(QWidget *parent)
    : UiObject(parent)
{
    setObjectName("UiThirdBodyForceList");
    setupUi();
}

void UiThirdBodyForceList::setupUi()
{
    // 主布局
    mainLayout_ = new QVBoxLayout(this);
    mainLayout_->setContentsMargins(0, 0, 0, 0);
    
    // 顶部布局
    topLayout_ = new QHBoxLayout();
    
    // 三体列表
    thirdBodyList_ = new QListWidget(this);
    thirdBodyList_->setMinimumWidth(200);
    topLayout_->addWidget(thirdBodyList_);
    
    // 按钮布局
    buttonLayout_ = new QVBoxLayout();
    addButton_ = new QPushButton("添加", this);
    removeButton_ = new QPushButton("移除", this);
    buttonLayout_->addWidget(addButton_);
    buttonLayout_->addWidget(removeButton_);
    buttonLayout_->addStretch();
    topLayout_->addLayout(buttonLayout_);
    
    // 详情组
    detailGroup_ = new QGroupBox("三体详情", this);
    detailLayout_ = new QVBoxLayout(detailGroup_);
    thirdBodyWidget_ = new UiThirdBodyForce(detailGroup_);
    detailLayout_->addWidget(thirdBodyWidget_);
    
    // 添加到主布局
    mainLayout_->addLayout(topLayout_);
    mainLayout_->addWidget(detailGroup_);
    
    // 连接信号槽
    connect(addButton_, &QPushButton::clicked, this, &UiThirdBodyForceList::onAddThirdBody);
    connect(removeButton_, &QPushButton::clicked, this, &UiThirdBodyForceList::onRemoveThirdBody);
    connect(thirdBodyList_, &QListWidget::currentRowChanged, this, &UiThirdBodyForceList::onThirdBodySelected);
    
    // 初始化
    refreshUi();
}

HPOPForceModel* UiThirdBodyForceList::getHPOPForceModel() const
{
    return dynamic_cast<HPOPForceModel*>(getObject());
}

void UiThirdBodyForceList::setHPOPForceModel(HPOPForceModel* hpop)
{
    if (hpop) {
        setObject(hpop);
        hpopForceModel_ = hpop;
        refreshUi();
    }
}

void UiThirdBodyForceList::refreshUi()
{
    auto hpop = getHPOPForceModel();
    if (!hpop) return;
    
    // 清空列表
    thirdBodyList_->clear();
    
    // 添加三体到列表
    const auto& thirdBodies = hpop->getThirdBodies();
    for (const auto& thirdBody : thirdBodies) {
        auto body = thirdBody.body();
        if (body) {
            thirdBodyList_->addItem(body->name().c_str());
        }
    }
    
    // 选中第一个项目
    if (thirdBodyList_->count() > 0) {
        thirdBodyList_->setCurrentRow(0);
    } else {
        thirdBodyWidget_->setThirdBodyForce(nullptr);
    }
}

void UiThirdBodyForceList::apply()
{
    // 应用当前选中的三体配置
    thirdBodyWidget_->apply();
}

void UiThirdBodyForceList::onAddThirdBody()
{
    auto hpop = getHPOPForceModel();
    if (!hpop) return;
    
    // 这里可以添加一个对话框让用户选择要添加的天体
    // 暂时添加一个默认的月球
    // @todo: 实现天体选择对话框
    CelestialBody* moon = nullptr; // 这里需要从天体数据库中获取月球
    if (moon) {
        auto thirdBody = hpop->addThirdBody(moon);
        if (thirdBody) {
            refreshUi();
        }
    } else {
        QMessageBox::information(this, "提示", "暂不支持添加新天体，请在代码中手动添加");
    }
}

void UiThirdBodyForceList::onRemoveThirdBody()
{
    auto hpop = getHPOPForceModel();
    if (!hpop) return;
    
    int currentRow = thirdBodyList_->currentRow();
    if (currentRow >= 0) {
        // 这里需要从HPOPForceModel中移除对应的三体
        // @todo: 实现从HPOPForceModel中移除三体的方法
        refreshUi();
    }
}

void UiThirdBodyForceList::onThirdBodySelected(int index)
{
    auto hpop = getHPOPForceModel();
    if (!hpop) return;
    
    const auto& thirdBodies = hpop->getThirdBodies();
    if (index >= 0 && index < static_cast<int>(thirdBodies.size())) {
        // 注意：这里需要传递一个指向ThirdBodyForce的指针
        // 由于thirdBodies是std::vector<ThirdBodyForce>，我们需要获取地址
        thirdBodyWidget_->setThirdBodyForce(const_cast<ThirdBodyForce*>(&thirdBodies[index]));
        thirdBodyWidget_->refreshUi();
    } else {
        thirdBodyWidget_->setThirdBodyForce(nullptr);
    }
}

AST_NAMESPACE_END