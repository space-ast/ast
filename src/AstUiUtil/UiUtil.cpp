///
/// @file      UiUtil.cpp
/// @brief     UiUtil 工具函数实现
/// @author    axel
/// @date      2026-06-12
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

#include "UiUtil.hpp"
#include "AstUtil/Logger.hpp"
#include <QApplication>
#include <QThread>
#include <QSemaphore>

AST_NAMESPACE_BEGIN

/// @brief 可调用事件，用于在主线程中执行函数
class CallableEvent : public QEvent {
public:
    static const QEvent::Type EventType = QEvent::Type(QEvent::User + 1);

    explicit CallableEvent(std::function<void()> func, QSemaphore* sem = nullptr)
        : QEvent(EventType)
        , func_(std::move(func))
        , sem_(sem)
    {}

    void call() {
        func_();
        if (sem_) sem_->release();
    }

    CallableEvent(const CallableEvent&) = delete;
    CallableEvent& operator=(const CallableEvent&) = delete;

private:
    std::function<void()> func_;
    QSemaphore* sem_;
};


/// @brief 调用帮助类，用于在主线程中执行函数
class InvokeHelper : public QObject {
public:
    InvokeHelper()
    {
        if(qApp)
            this->moveToThread(qApp->thread());
    }
    void post(std::function<void()> func) {
        QCoreApplication::postEvent(this, new CallableEvent(std::move(func)));
    }
    void blockingPost(std::function<void()> func) {
        if (QThread::currentThread() == thread()) {
            aWarning("blocking post in same thread");
            func();
            return;
        }

        QSemaphore sem(0);
        QCoreApplication::postEvent(this, new CallableEvent(std::move(func), &sem));
        sem.acquire();
    }
protected:
    void customEvent(QEvent *event) override {
        if (event->type() == CallableEvent::EventType) {
            static_cast<CallableEvent*>(event)->call();
            return;
        }
        QObject::customEvent(event);
    }
};

void aAddQueued(std::function<void()> func, Qt::ConnectionType type)
{
    if(!qApp)
    {
        aError("failed to add queued function, qApp is null");
        return;
    }
    static InvokeHelper* helper = new InvokeHelper();

    QThread* currentThread = QThread::currentThread();
    QThread *objectThread = helper->thread();
    bool receiverInSameThread = false;
    if (objectThread)
        receiverInSameThread = currentThread == objectThread;

    if (type == Qt::AutoConnection)
        type = receiverInSameThread ? Qt::DirectConnection : Qt::QueuedConnection;;
    switch (type)
    {
        case Qt::AutoConnection:
        case Qt::DirectConnection:
        {
            func();
            break;
        }
        case Qt::QueuedConnection:
        {
            helper->post(std::move(func));
            break;
        }
        case Qt::BlockingQueuedConnection:
        {
            if (receiverInSameThread) {
                // 同一线程的阻塞调用 = 直接执行，否则死锁
                func();
            } else {
                helper->blockingPost(std::move(func));
            }
            break;
        }
        default:
            break;
    }
}

// ---- 辅助：通过 DisplayRole 文字匹配查找子行索引（要求文本唯一） ----
static int findRowByDisplayText(QAbstractItemModel* model, const std::string& text, const QModelIndex& parent)
{
    if (!model || text.empty())
        return -1;

    int matchedRow = -1;
    int count = model->rowCount(parent);
    for (int row = 0; row < count; ++row)
    {
        QModelIndex idx = model->index(row, 0, parent);
        if (idx.isValid())
        {
            QString displayText = model->data(idx, Qt::DisplayRole).toString();
            if (displayText.toStdString() == text)
            {
                if (matchedRow >= 0)
                    return -1;  // 文本不唯一，返回 -1 避免歧义
                matchedRow = row;
            }
        }
    }
    return matchedRow;
}

// ---- 主函数 ----
QModelIndex modelIndexFromItemPath(QAbstractItemModel* model, const JsonValue& itemPath)
{
    if (!model || itemPath.isNull())
        return QModelIndex();

    // ---- Case 1: number → ListItem ----
    if (itemPath.isNumber())
    {
        int row = itemPath.toInt();
        return model->index(row, 0);
    }

    // ---- Case 2: array ----
    if (itemPath.isArray())
    {
        const auto& arr = itemPath.getArray();
        if (arr.empty())
            return QModelIndex();

        // Case 2a: [number, number] → TableItem (row, col)
        if (arr.size() == 2 && arr[0].isNumber() && arr[1].isNumber())
        {
            int row = arr[0].toInt();
            int col = arr[1].toInt();
            return model->index(row, col);
        }

        // Case 2b: 树路径 — 逐级导航
        //           元素可为 number (行), string (文字匹配), 或 [row, col] (显式行列)
        QModelIndex parent;
        for (size_t i = 0; i < arr.size(); ++i)
        {
            const JsonValue& elem = arr[i];

            int row = -1;
            int col = 0;

            if (elem.isNumber())
            {
                row = elem.toInt();
            }
            else if (elem.isString())
            {
                row = findRowByDisplayText(model, elem.toString(), parent);
            }
            else if (elem.isArray())
            {
                const auto& cell = elem.getArray();
                if (cell.size() >= 2 && cell[0].isNumber() && cell[1].isNumber())
                {
                    row = cell[0].toInt();
                    col = cell[1].toInt();
                }
                else if (cell.size() == 1 && cell[0].isNumber())
                {
                    // 兼容 [[row]] → 仅指定行
                    row = cell[0].toInt();
                }
            }

            if (row < 0)
                return QModelIndex();

            parent = model->index(row, col, parent);
            if (!parent.isValid())
                return QModelIndex();
        }

        return parent;
    }

    return QModelIndex();
}

AST_NAMESPACE_END
