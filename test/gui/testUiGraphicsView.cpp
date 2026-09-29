#ifdef AST_WITH_QT
#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QList>

class PortItem : public QGraphicsItem
{
public:
    explicit PortItem(QGraphicsItem *parent = nullptr)
        : QGraphicsItem(parent)
    {
        setFlags(ItemIsSelectable);
    }

    QRectF boundingRect() const override
    {
        return QRectF(-5, -5, 10, 10);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);

        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(QBrush(Qt::white));
        painter->drawEllipse(boundingRect());
    }

    void setBlockItem(QGraphicsItem *block)
    {
        m_blockItem = block;
    }

    QGraphicsItem *blockItem() const
    {
        return m_blockItem;
    }

private:
    QGraphicsItem *m_blockItem = nullptr;
};

class BlockItem : public QGraphicsItem
{
public:
    explicit BlockItem(const QString &name, QGraphicsItem *parent = nullptr)
        : QGraphicsItem(parent), m_name(name)
    {
        setFlags(ItemIsMovable | ItemIsSelectable);
        
        // 创建输入端口
        m_inputPort = new PortItem(this);
        m_inputPort->setPos(-20, 0);
        m_inputPort->setBlockItem(this);
        
        // 创建输出端口
        m_outputPort = new PortItem(this);
        m_outputPort->setPos(80, 0);
        m_outputPort->setBlockItem(this);
    }

    QRectF boundingRect() const override
    {
        return QRectF(-30, -20, 110, 40);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);

        // 绘制模块矩形
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(QBrush(Qt::lightGray));
        painter->drawRect(boundingRect());
        
        // 绘制模块名称
        painter->setPen(QPen(Qt::black));
        painter->drawText(boundingRect(), Qt::AlignCenter, m_name);
    }

    PortItem *inputPort() const
    {
        return m_inputPort;
    }

    PortItem *outputPort() const
    {
        return m_outputPort;
    }

private:
    QString m_name;
    PortItem *m_inputPort = nullptr;
    PortItem *m_outputPort = nullptr;
};

class ConnectionItem : public QGraphicsItem
{
public:
    ConnectionItem(PortItem *startPort, PortItem *endPort, QGraphicsItem *parent = nullptr)
        : QGraphicsItem(parent), m_startPort(startPort), m_endPort(endPort)
    {
        updatePosition();
    }

    QRectF boundingRect() const override
    {
        qreal left = qMin(m_startPos.x(), m_endPos.x()) - 2;
        qreal top = qMin(m_startPos.y(), m_endPos.y()) - 2;
        qreal width = qAbs(m_endPos.x() - m_startPos.x()) + 4;
        qreal height = qAbs(m_endPos.y() - m_startPos.y()) + 4;
        return QRectF(left, top, width, height);
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override
    {
        Q_UNUSED(option);
        Q_UNUSED(widget);

        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(m_startPos, m_endPos);
    }

    void updatePosition()
    {
        if (m_startPort)
            m_startPos = m_startPort->scenePos();
        if (m_endPort)
            m_endPos = m_endPort->scenePos();
        update();
    }

    PortItem *startPort() const
    {
        return m_startPort;
    }

    PortItem *endPort() const
    {
        return m_endPort;
    }

private:
    PortItem *m_startPort = nullptr;
    PortItem *m_endPort = nullptr;
    QPointF m_startPos;
    QPointF m_endPos;
};

class SimulinkEditor : public QGraphicsView
{
public:
    explicit SimulinkEditor(QWidget *parent = nullptr)
        : QGraphicsView(parent)
    {
        m_scene = new QGraphicsScene(this);
        setScene(m_scene);
        setRenderHint(QPainter::Antialiasing);
        
        // 创建初始模块
        createBlock("Input", 100, 100);
        createBlock("Gain", 250, 100);
        createBlock("Output", 400, 100);
        
        // 连接模块
        connectBlocks(0, 1);
        connectBlocks(1, 2);
    }

    BlockItem *createBlock(const QString &name, qreal x, qreal y)
    {
        BlockItem *block = new BlockItem(name);
        block->setPos(x, y);
        m_scene->addItem(block);
        m_blocks.append(block);
        return block;
    }

    void connectBlocks(int startIndex, int endIndex)
    {
        if (startIndex >= 0 && startIndex < m_blocks.size() &&
            endIndex >= 0 && endIndex < m_blocks.size()) {
            
            BlockItem *startBlock = m_blocks[startIndex];
            BlockItem *endBlock = m_blocks[endIndex];
            
            ConnectionItem *connection = new ConnectionItem(
                startBlock->outputPort(),
                endBlock->inputPort()
            );
            m_scene->addItem(connection);
            m_connections.append(connection);
        }
    }

protected:
    void mousePressEvent(QMouseEvent *event) override
    {
        QGraphicsView::mousePressEvent(event);
    }

    void mouseMoveEvent(QMouseEvent *event) override
    {
        QGraphicsView::mouseMoveEvent(event);
        
        // 更新所有连接的位置
        for (ConnectionItem *connection : m_connections) {
            connection->updatePosition();
        }
    }

private:
    QGraphicsScene *m_scene = nullptr;
    QList<BlockItem *> m_blocks;
    QList<ConnectionItem *> m_connections;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    SimulinkEditor editor;
    editor.setWindowTitle("Simulink Editor Demo");
    editor.resize(800, 600);
    editor.show();
    
    return app.exec();
}
#else
#include <stdio.h>
int main(){
    puts("没有启用Qt库");
    return 0;
}
#endif
