///
/// @file      Dimension.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2025-12-12
/// @copyright 版权所有 (C) 2025-present, ast项目.
///
/// ast项目（https://github.com/space-ast/ast）
/// 本项目基于 Apache 2.0 开源许可证分发。
/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。
/// 许可证全文请见：
/// 
///    http://www.apache.org/licenses/LICENSE-2.0
/// 
/// 重要须知：
/// 软件按"现有状态"提供，无任何明示或暗示的担保条件。
/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。
/// 使用本软件所产生的风险，需由您自行承担。

#include "Dimension.hpp"
#include "AstUtil/Logger.hpp"

AST_NAMESPACE_BEGIN

#if defined AST_TEST_DIMENSION 

struct dimension_value
{
    int dim1 : 4;
    int dim2 : 4;
    int dim3 : 4;
    int dim4 : 4;
    int dim5 : 4;
    int dim6 : 4;
    int dim7 : 4;
    int dim8 : 4;
};


AST_UTIL_CAPI int dim_get_exponent_shift(int dimension)
{
    int shift = 3 * 4; //static_cast<int>(idx) * 4;
    int exponent = (dimension >> shift) & 0xF;
    // 将4位有符号数转换为实际有符号整数
    return exponent;
}
AST_UTIL_CAPI int dim_set_exponent_shift(int dimension, int exponent)
{

    int shift = 3 * 4;
    int mask = 0xF << shift;
    int newValue = dimension & ~mask; // 清除原值
    // 将指数转换为4位有符号表示
    int expBits = exponent & 0xF;
    newValue |= (expBits << shift); // 设置新值

    return newValue;
}

AST_UTIL_CAPI
int dim_get_exponent_bitfield(int dimension)
{
    return ((dimension_value*)&dimension)->dim3;
}

AST_UTIL_CAPI
int dim_set_exponent_bitfield(int dimension, int exponent)
{
    ((dimension_value*)&dimension)->dim3 = exponent;
    return dimension;
}

#endif


std::string aDimName(EDimension dimension)
{
    switch (dimension)
    {
    case EDimension::eUnit: return "Unit";
    case EDimension::eLength: return "Length";
    case EDimension::eMass: return "Mass";
    case EDimension::eTime: return "Time";
    case EDimension::eCurrent: return "Electric Current";
    case EDimension::eTemperature: return "Temperature";
    case EDimension::eAmount: return "Amount of Substance";
    case EDimension::eLuminous: return "Luminous Intensity";
    case EDimension::eAngle: return "Angle";
    case EDimension::eAngVel: return "Angular Velocity";
    case EDimension::eArea: return "Area";
    case EDimension::eVolume: return "Volume";
    case EDimension::eSpeed: return "Speed";
    case EDimension::eAcceleration: return "Acceleration";
    case EDimension::eForce: return "Force";
    case EDimension::ePressure: return "Pressure";
    case EDimension::eEnergy: return "Energy";
    case EDimension::ePower: return "Power";
    case EDimension::eFrequency: return "Frequency";
    }
    return aDimSymbol(dimension);
}


const char* aNumberToSuperscript(int number)
{
    static const char *number_to_superscript[] =
    {
        aText("\u2070"), aText("\u00b9"), aText("\u00b2"), aText("\u00b3"), aText("\u2074"), 
        aText("\u2075"), aText("\u2076"), aText("\u2077"), aText("\u2078"), aText("\u2079"),
        /*
        aText("⁰"), aText("¹"), aText("²"), aText("³"), aText("⁴"), 
        aText("⁵"), aText("⁶"), aText("⁷"), aText("⁸"), aText("⁹")
        */
    };

    if (number < 0 || number > 9)
    {
        aError("number out of range");
        return "";
    }
    return number_to_superscript[number];
}

#if 0
const char* aNumberToStr(int number)
{
    static const char *number_to_str[] =
    {
        "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"
    };

    if (number < 0 || number > 9)
    {
        aError("number out of range");
        return "";
    }
    return number_to_str[number];
}
#endif

// const char* symbol_separator = aText("·");
const char* symbol_separator = aText("\u00b7");

std::string aDimBasicSymbol(EDimension dimension)
{
    switch (dimension)
    {
    case EDimension::eUnit: return ""; // @todo 单位量纲的符号为空，合适吗？
    case EDimension::eLength: return "L";
    case EDimension::eMass: return "M";
    case EDimension::eTime: return "T";
    case EDimension::eCurrent: return "I";
    case EDimension::eTemperature: return aText("\u0398");  // Θ
    case EDimension::eAmount: return "N";
    case EDimension::eLuminous: return "J";
    case EDimension::eAngle:return "A";
    }
    return "";
}

std::string aDimSymbol(EDimension dimension)
{
    std::string symbol = aDimBasicSymbol(dimension);
    if(!symbol.empty())
    {
        return symbol;
    }

    for (int i = 0; i < 8; i++)
    {
        // 跳过指数为0的量纲
        int exponent = dim_get_exponent(dimension, i);
        if (exponent != 0)
        {
            if(!symbol.empty())
            {
                symbol += symbol_separator;
            }
            EDimension basicDim = dim_set_exponent(EDimension::eUnit, i, 1);
            auto basicSymbol = aDimBasicSymbol(basicDim);
            if(basicSymbol.empty())
            {
                aError("basic symbol is empty");
                continue;
            }
            symbol += basicSymbol;
            if (exponent != 1){
                if (exponent < 0)
                {
                    // symbol += aText("⁻");
                    symbol += aText("\u207b");
                }
                symbol += aNumberToSuperscript(abs(exponent));
            }
        }
    }
    return symbol;
}



AST_NAMESPACE_END

