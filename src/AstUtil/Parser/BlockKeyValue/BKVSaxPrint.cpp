///
/// @file      BKVSaxPrint.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-08
/// @copyright 版权所有 (C) 2026-present, ast项目.
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

#include "BKVSaxPrint.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/StringView.hpp"
#include "AstUtil/GenericValue.hpp"

AST_NAMESPACE_BEGIN

BKVSaxPrint::BKVSaxPrint()
    : file_(stdout)
    , indent_(4)
    , depth_(0)
{
}

BKVSaxPrint::BKVSaxPrint(StringView filepath)
{
    file_ = fopen(filepath.data(), "w");
    if (file_ == nullptr)
    {
        aError("failed to open file: %s", filepath.data());
    }
}

BKVSaxPrint::~BKVSaxPrint()
{
    if (file_ != stdout && file_ != nullptr && file_ != stderr)
    {
        fclose(file_);
    }
}

void BKVSaxPrint::begin(StringView name)
{
    fprintf(file_, "%*s", depth_ * indent_, "");
    depth_++;
    fprintf(file_, "BEGIN %.*s\n", name.size(), name.data());
}

void BKVSaxPrint::end(StringView name)
{
    depth_--;
    fprintf(file_, "%*s", depth_ * indent_, "");
    fprintf(file_, "END %.*s\n", name.size(), name.data());
}

void BKVSaxPrint::keyValue(StringView key, const GenericValue &value)
{
    fprintf(file_, "%*s", depth_ * indent_, "");
    fprintf(file_, "%.*s %s\n", key.size(), key.data(), value.c_str());
}

AST_NAMESPACE_END


