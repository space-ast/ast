#ifdef AST_WITH_QT
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
#else
#include <stdio.h>
int main(){
    puts("没有启用Qt库");
    return 0;
}
#endif
