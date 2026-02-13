#include <AstGlobal.h>
#include <iostream>
#include <string>
#include <vector>

#ifndef AST_NO_CSPICE
#include "SpiceUsr.h"
#endif

int main() {
    setlocale(LC_ALL, ".UTF-8");

#ifndef AST_NO_CSPICE
    std::cout << "=== SPICE 计算火星相对于地球位置示例 ===" << std::endl;
    
    
    // 2. 定义需要的内核文件
    // 注意：实际使用时需要下载这些内核文件到本地目录
    // 可以从NASA的NAIF网站下载：https://naif.jpl.nasa.gov/pub/naif/generic_kernels/
    std::vector<std::string> kernels = {
        // 基础SPICE内核
        "data/Test/kernels/spk/de430.bsp",           // 行星历表（DE430）
        "data/Test/kernels/lsk/naif0012.tls"         // 时间系统定义
    };
    
    // 3. 尝试加载内核文件
    std::cout << "\n1. 加载SPICE内核文件：" << std::endl;
    
    for (const auto& kernel : kernels) {
        std::cout << "   尝试加载: " << kernel << "...";
        // 加载内核文件
        furnsh_c(kernel.c_str());
        std::cout << " 成功" << std::endl;
    }
    
    // 4. 设置观测时间
    std::string utc_time = "2023-01-01T12:00:00.000";
    SpiceDouble et;
    
    // 转换UTC时间到Ephemeris Time (ET)
    str2et_c(utc_time.c_str(), &et);
    
    std::cout << "\n2. 观测时间设置：" << std::endl;
    std::cout << "   UTC时间: " << utc_time << std::endl;
    std::cout << "   Ephemeris Time (ET): " << et << " 秒" << std::endl;
    
    // 5. 计算火星相对于地球的位置
    const char* target = "Moon";
    const char* observer = "Earth";
    const char* ref_frame = "J2000";
    const char* aberration_correction = "LT+S";
    
    SpiceDouble position[3];   // 位置向量（公里）
    SpiceDouble light_time;    // 光行时（秒）
    
    // 使用spkpos_c计算位置
    spkpos_c(target, et, ref_frame, aberration_correction, observer, position, &light_time);
    
    std::cout << "\n3. 火星相对于地球的位置（" << ref_frame << "坐标系）：" << std::endl;
    std::cout << "   目标天体: " << target << std::endl;
    std::cout << "   观测者: " << observer << std::endl;
    std::cout << "   光行时修正: " << aberration_correction << std::endl;
    std::cout << "   位置向量 (公里): " << std::endl;
    std::cout << "      X: " << position[0] << std::endl;
    std::cout << "      Y: " << position[1] << std::endl;
    std::cout << "      Z: " << position[2] << std::endl;
    
    // 计算距离
    SpiceDouble distance = vnorm_c(position);
    std::cout << "   距离地球: " << distance << " 公里" << std::endl;
    std::cout << "   光行时: " << light_time << " 秒" << std::endl;
    
    // 6. 计算不同时间的位置变化（可选）
    std::cout << "\n4. 不同时间点的位置变化：" << std::endl;
    std::vector<double> time_offsets = {0.0, 86400.0, 172800.0, 259200.0}; // 0天, 1天, 2天, 3天
    
    for (double offset : time_offsets) {
        SpiceDouble current_et = et + offset;
        SpiceDouble current_pos[3];
        SpiceDouble current_lt;
        
        spkpos_c(target, current_et, ref_frame, aberration_correction, observer, current_pos, &current_lt);
        
        SpiceDouble current_dist = vnorm_c(current_pos);
        
        // 转换ET到UTC时间用于显示
        SpiceChar current_utc[32];
        et2utc_c(current_et, "C", 0, sizeof(current_utc), current_utc);
        
        std::cout << "   " << current_utc << ": 距离 = " << current_dist << " 公里" << std::endl;
    }
        
    
    
    // 7. 清理：卸载所有内核文件
    kclear_c();
    
    std::cout << "\n=== 示例结束 ===" << std::endl;
    
#else
    std::cout << "错误：未启用CSPICE支持" << std::endl;
    std::cout << "请确保在xmake.lua中配置了cspice库" << std::endl;
#endif
    
    return 0;
}