#include "AstUtil/Literals.hpp"

int main(){
    AST_USING_NAMESPACE;
    // 时间转换示例
    double time1 = 24_h;           // 24小时转换为秒 (86400秒)
    double time2 = 30_min;         // 30分钟转换为秒 (1800秒)
    double time3 = 500_ms;         // 500毫秒转换为秒 (0.5秒)
    double shortDelay = 10_us;     // 10微秒转换为秒
}