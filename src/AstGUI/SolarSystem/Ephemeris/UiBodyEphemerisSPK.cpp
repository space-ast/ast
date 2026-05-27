///
/// @file      UiBodyEphemerisSPK.cpp
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

#include "AstGUI/UiBodyEphemerisSPK.hpp"
#include "AstCore/BodyEphemerisSPK.hpp"
#include "AstGUI/UiInteger.hpp"
#include "AstGUI/UiFilePath.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>

AST_NAMESPACE_BEGIN

UiBodyEphemerisSPK::UiBodyEphemerisSPK(Object *object, QWidget *parent)
    : UiBodyEphemerisSPK(parent)
{
    setBodyEphemerisSPK(dynamic_cast<BodyEphemerisSPK*>(object));
}

UiBodyEphemerisSPK::UiBodyEphemerisSPK(QWidget *parent) : UiObject(parent)
{
    // 创建主布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // SPICE索引
    QHBoxLayout* spiceIndexLayout = new QHBoxLayout();
    QLabel* spiceIndexLabel = new QLabel(tr("SPICE索引"), this);
    spiceIndexEdit_ = new UiInteger(this);
    spiceIndexEdit_->setValue(0);
    spiceIndexLayout->addWidget(spiceIndexLabel);
    spiceIndexLayout->addWidget(spiceIndexEdit_);
    mainLayout->addLayout(spiceIndexLayout);
    
    // SPK文件路径输入
    QHBoxLayout* spkFileLayout = new QHBoxLayout();
    QLabel* spkFileLabel = new QLabel(tr("SPK文件"), this);
    filePathEdit_ = new UiFilePath(this);
    filePathEdit_->setFilter(tr("SPK文件 (*.bsp);;所有文件 (*.*)"));
    spkFileLayout->addWidget(spkFileLabel);
    spkFileLayout->addWidget(filePathEdit_);
    mainLayout->addLayout(spkFileLayout);
    
    setLayout(mainLayout);

    connect(spiceIndexEdit_, &UiInteger::valueChanged, this, &UiBodyEphemerisSPK::apply);
    connect(filePathEdit_, &UiFilePath::pathChanged, this, &UiBodyEphemerisSPK::onFilePathChanged);
}

void UiBodyEphemerisSPK::refreshUi()
{
    if(auto ephemeris = getBodyEphemerisSPK()){
        // 设置SPICE索引值
        spiceIndexEdit_->setValue(ephemeris->getSpiceIndex());
        
        // 显示SPK文件路径
        const std::string& filePath = ephemeris->getFilePath();
        if (!filePath.empty())
        {
            filePathEdit_->setPath(QString::fromUtf8(filePath.c_str()));
        }
        else
        {
            filePathEdit_->setPath(QString());
        }
    }
}

void UiBodyEphemerisSPK::apply()
{
    if (auto ephemeris = getBodyEphemerisSPK())
    {
        applyTo(ephemeris);
        emit bodyEphemerisSPKChanged(ephemeris);
    }
}

void UiBodyEphemerisSPK::applyTo(BodyEphemerisSPK *ephemeris)
{
    // 从界面上获取SPICE索引值
    ephemeris->setSpiceIndex(spiceIndexEdit_->value());
}

void UiBodyEphemerisSPK::setBodyEphemerisSPK(BodyEphemerisSPK* ephemeris)
{
    if (ephemeris)
    {
        setObject(ephemeris);
        refreshUi();
    }
}

BodyEphemerisSPK* UiBodyEphemerisSPK::getBodyEphemerisSPK() const
{
    return dynamic_cast<BodyEphemerisSPK*>(getObject());
}

void UiBodyEphemerisSPK::onFilePathChanged(const QString& path)
{
    if (auto ephemeris = getBodyEphemerisSPK())
    {
        if (!path.isEmpty())
        {
            errc_t rc = ephemeris->openSPKFile(path.toUtf8().data());
            if (rc != 0)
            {
                // 显示加载失败提示
                QMessageBox::warning(
                    this,
                    tr("加载失败"),
                    tr("SPK文件加载失败，请检查文件是否存在且格式正确。"),
                    QMessageBox::Ok
                );
            }
        }
    }
}

AST_NAMESPACE_END