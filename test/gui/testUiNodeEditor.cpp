#ifdef AST_WITH_QT
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QLabel>

#ifdef AST_WITH_NODEEDITOR
#include <nodes/FlowScene>
#include <nodes/FlowView>
#include <nodes/DataModelRegistry>
#include <nodes/NodeData>
#include <nodes/NodeDelegateModel>

using QtNodes::DataModelRegistry;
using QtNodes::FlowScene;
using QtNodes::FlowView;
using QtNodes::NodeData;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeDataModel;

// 定义一个简单的整数数据类型
class IntegerData : public NodeData
{
public:
    explicit IntegerData(int value = 0) : _value(value) {}

    QString type() const override { return "Integer"; }
    int value() const { return _value; }

private:
    int _value;
};

// 定义一个加法节点模型
class AddModel : public NodeDelegateModel
{
public:
    AddModel() {}

    QString caption() const override { return "Add"; }
    QString name() const override { return "Add"; }
    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        if (portType == PortType::In)
        {
            if (portIndex == 0) return "A";
            if (portIndex == 1) return "B";
        }
        else if (portType == PortType::Out)
        {
            return "Result";
        }
        return "";
    }

    unsigned int nPorts(PortType portType) const override
    {
        if (portType == PortType::In) return 2;
        if (portType == PortType::Out) return 1;
        return 0;
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        if (portType == PortType::In)
        {
            return NodeDataType{"Integer", "int"};
        }
        else if (portType == PortType::Out)
        {
            return NodeDataType{"Integer", "int"};
        }
        return NodeDataType{};
    }

    std::shared_ptr<NodeData> outData(PortIndex port) override
    {
        if (port == 0)
        {
            int result = 0;
            if (_inData1 && _inData2)
            {
                auto data1 = std::dynamic_pointer_cast<IntegerData>(_inData1);
                auto data2 = std::dynamic_pointer_cast<IntegerData>(_inData2);
                if (data1 && data2)
                {
                    result = data1->value() + data2->value();
                }
            }
            return std::make_shared<IntegerData>(result);
        }
        return nullptr;
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override
    {
        if (portIndex == 0)
        {
            _inData1 = data;
        }
        else if (portIndex == 1)
        {
            _inData2 = data;
        }

        emit dataUpdated(0);
    }

private:
    std::shared_ptr<NodeData> _inData1;
    std::shared_ptr<NodeData> _inData2;
};

// 定义一个常量节点模型
class ConstantModel : public NodeDelegateModel
{
public:
    ConstantModel() : _value(0) {}

    QString caption() const override { return "Constant"; }
    QString name() const override { return "Constant"; }
    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        if (portType == PortType::Out)
        {
            return "Value";
        }
        return "";
    }

    unsigned int nPorts(PortType portType) const override
    {
        if (portType == PortType::Out) return 1;
        return 0;
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        if (portType == PortType::Out)
        {
            return NodeDataType{"Integer", "int"};
        }
        return NodeDataType{};
    }

    std::shared_ptr<NodeData> outData(PortIndex port) override
    {
        if (port == 0)
        {
            return std::make_shared<IntegerData>(_value);
        }
        return nullptr;
    }

    QWidget* embeddedWidget() override
    {
        return nullptr;
    }

private:
    int _value;
};

// 注册节点模型
static std::shared_ptr<DataModelRegistry> registerDataModels()
{
    auto registry = std::make_shared<DataModelRegistry>();
    registry->registerModel<AddModel>();
    registry->registerModel<ConstantModel>();
    return registry;
}

class NodeEditorWindow : public QMainWindow
{
public:
    NodeEditorWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setWindowTitle("QtNodes Editor Demo");
        setGeometry(100, 100, 800, 600);

        // 创建中央部件
        auto centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        // 创建布局
        auto layout = new QVBoxLayout(centralWidget);

        // 创建标签
        auto label = new QLabel("QtNodes Editor Demo", this);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);

        // 创建节点场景
        _scene = new FlowScene(registerDataModels(), this);

        // 创建节点视图
        _view = new FlowView(_scene, this);
        _view->setScene(_scene);
        layout->addWidget(_view);

        // 添加一些示例节点
        addExampleNodes();
    }

private:
    void addExampleNodes()
    {
        // 添加常量节点
        auto constant1 = _scene->createNode(std::make_unique<ConstantModel>());
        constant1->setPos(100, 100);

        auto constant2 = _scene->createNode(std::make_unique<ConstantModel>());
        constant2->setPos(100, 200);

        // 添加加法节点
        auto addNode = _scene->createNode(std::make_unique<AddModel>());
        addNode->setPos(300, 150);

        // 连接节点
        connectNodes(constant1, 0, addNode, 0);
        connectNodes(constant2, 0, addNode, 1);
    }

    void connectNodes(QtNodes::Node& sourceNode, PortIndex sourcePort, QtNodes::Node& targetNode, PortIndex targetPort)
    {
        auto connection = _scene->createConnection(
            sourceNode, sourcePort, targetNode, targetPort);
        if (connection)
        {
            _scene->addConnection(connection);
        }
    }

private:
    FlowScene* _scene = nullptr;
    FlowView* _view = nullptr;
};

#else

// 如果没有NodeEditor库，显示一个简单的提示窗口
class NodeEditorWindow : public QMainWindow
{
public:
    explicit NodeEditorWindow(QWidget *parent = nullptr)
        : QMainWindow(parent)
    {
        setWindowTitle("QtNodes Editor Demo");
        setGeometry(100, 100, 400, 300);

        auto centralWidget = new QWidget(this);
        setCentralWidget(centralWidget);

        auto layout = new QVBoxLayout(centralWidget);
        auto label = new QLabel("NodeEditor library is not available.", this);
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
    }
};

#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    NodeEditorWindow window;
    window.show();

    return app.exec();
}
#else
#include <stdio.h>
int main(){
    puts("没有启用Qt库");
    return 0;
}
#endif
