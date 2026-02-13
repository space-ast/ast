#include <iostream>
#include <string>
#include <iomanip>

#ifndef AST_NO_CSPICE
#include "SpiceUsr.h"
#include "SpiceZfc.h"
#endif

int main() {
    setlocale(LC_ALL, ".UTF-8");

#ifndef AST_NO_CSPICE

    // 加载内核数据文件
    furnsh_c("data/Test/kernels/lsk/naif0012.tls");
    
    // 1. UTC字符串转换为ET(Ephemeris Time)
    std::string utc_str = "2023-01-01T12:00:00.000";
    SpiceDouble et;
    str2et_c(utc_str.c_str(), &et);
    std::cout << "Ephemeris Time (ET): " << et << " 秒" << std::endl;
    
    // 2. ET转换为UTC字符串
    SpiceChar utc_out[32];
    et2utc_c(et, "C", 3, sizeof(utc_out), utc_out);  // "C"表示日历格式，3表示毫秒精度
    std::cout << "UTC时间: " << utc_out << std::endl;

    // 3. TAI转换为TT时间
    double tai = 1000;              // 相对于J2000.0的秒数
    double tt = tai;
    ttrans_("TAI", "TDT", &tt, 3, 3);
    std::cout << std::setprecision(15) << "TT时间: " << tt << " 秒" << std::endl;
    
    // 4. TT时间转换为TDB时间
    double tai_out = tt;
    ttrans_("TDT", "TDB", &tai_out, 3, 3);
    std::cout << "TDB时间: " << tai_out << " 秒" << std::endl;

#else
    std::cout << "错误：未启用CSPICE支持，请在xmake中启用cspice库" << std::endl;
#endif
    
    return 0;
}