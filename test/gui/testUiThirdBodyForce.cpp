#ifdef AST_WITH_QT
#include "AstGlobal.h"
#include "ast/ThirdBodyForce.hpp"
#include "ast/UiThirdBodyForce.hpp"
#include <QApplication>

AST_USING_NAMESPACE

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // 创建三体引力模型对象
    ThirdBodyForce* thirdBodyForce = new ThirdBodyForce();
    
    // 创建界面
    UiThirdBodyForce ui(thirdBodyForce);
    ui.show();
    
    int result = app.exec();
    
    // 清理内存
    thirdBodyForce->destruct();
    
    return result;
}
#else
#include <stdio.h>
int main(){
    puts("没有启用Qt库");
    return 0;
}
#endif
