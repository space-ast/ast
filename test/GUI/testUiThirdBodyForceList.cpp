#include "ast/UiThirdBodyForceList.hpp"
#include "ast/HPOPForceModel.hpp"
#include <QApplication>


AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建HPOP力模型
    HPOPForceModel hpop;
    
    // 创建UiThirdBodyForceList
    UiThirdBodyForceList ui(&hpop);
    ui.show();
    
    return app.exec();
}
