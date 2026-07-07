///
/// @file      GUIInterfaceImpl.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-02
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

#include "GUIInterfaceImpl.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/Logger.hpp"
#include "AstGUI/ObjectEditRegistry.hpp"
#include "AstGUI/AstGUIAPI.hpp"
#include <QWidget>
#include <QEvent>
#include <QEventLoop>
#include <QApplication>

AST_NAMESPACE_BEGIN

/// @brief 用于监听窗口关闭事件的助手类，将窗口关闭事件转换为信号
class CloseEventDetecter: public QObject
{
public:
    explicit CloseEventDetecter(QObject *parent) 
        : QObject(parent)
    {
        parent->installEventFilter(this);
    }
    bool eventFilter(QObject *watched, QEvent *event) override
    {
        if(event->type() == QEvent::Close)
        {
            if(watched == parent())
            {
                this->deleteLater();
                return true;
            }
        }
        return false;
    }
};

void aUiExecWidget(QWidget* widget)
{
    if(widget == nullptr)
        return;
    QObject* detecter = new CloseEventDetecter(widget);              /// 监听窗口关闭事件
    QEventLoop loop;
    QObject::connect(detecter, &QObject::destroyed, &loop, &QEventLoop::quit);
    widget->show();
    loop.exec();
}

GUIInterfaceImpl* GUIInterfaceImpl::Instance()
{
    static GUIInterfaceImpl instance;
    return &instance;
}

GUIInterfaceImpl::GUIInterfaceImpl()
{
    if(!qApp)
    {
        aGUIInit();
    }
}

errc_t GUIInterfaceImpl::editObject(Object *object)
{
    QWidget* editWidget = ObjectEditRegistry::Instance().newEditWidget(object);
    if(editWidget == nullptr)
    {
        return eErrorNullPtr;
    }
    editWidget->setAttribute(Qt::WA_DeleteOnClose);                      /// 关闭窗口时自动删除对象
    editWidget->setWindowModality(Qt::ApplicationModal);                 /// 应用程序模态，其他窗口不可交互
    editWidget->setWindowFlags(editWidget->windowFlags() | Qt::Dialog);  /// 设置为对话框样式
    aUiExecWidget(editWidget);
    return eNoError;
}

Object* GUIInterfaceImpl::selectObject(StringView typeName)
{
    aError("selectObject not implemented");
    return nullptr;
}

// @todo 这里需要使用凤凰单例模式避免悬挂指针问题

A_THREAD_LOCAL std::string translateCache_;

const char* GUIInterfaceImpl::translate(const char* msgctxt, const char* msgid)
{
    auto app = qApp;
    if(app){
        translateCache_ = app->translate(msgctxt, msgid).toStdString();
        return translateCache_.c_str();
    }
    return msgid;
}


GUIInterface* aGUIInterfaceImpl()
{
    return GUIInterfaceImpl::Instance();
}

AST_NAMESPACE_END
