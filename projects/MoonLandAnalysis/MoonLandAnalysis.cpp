///
/// @file      MoonLandAnalysis.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-05-14
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

#include "MoonLandAnalysis.hpp"
#include "AstCore/HPOP.hpp"
#include "AstAnalyzer/TraverseSearchAnalyzer.hpp"
#include "AstCore/Command.hpp"
#include "AstUtil/Literals.hpp"

/*
PRL: 近月制动

*/

AST_USING_NAMESPACE;
using namespace literals;

errc_t MoonLandCommand::execute()
{
    
    return eNoError;
}

/*
搜索参数: 
- prlTime: 近月制动的时间点
- landTime: 着陆时间点

约束参数:
- 太阳方位角	
- 太阳高度角
- 月固系赤纬
- 月固系RAAN	
- 可见约束

*/

int main()
{

    return 0;
}
