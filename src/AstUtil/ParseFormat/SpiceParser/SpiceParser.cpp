///
/// @file      SpiceParser.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-03-05
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

#include "SpiceParser.hpp"
#include "AstUtil/StringView.hpp"

AST_NAMESPACE_BEGIN

SpiceParser::SpiceParser()
    : BaseParser()
    , keyBuffer_(1024)
    //, valueBuffer_(2048)
    , inDataBlock_(false)
{

}

SpiceParser::SpiceParser(StringView filepath)
    : BaseParser(filepath)
    , keyBuffer_(1024)
    // , valueBuffer_(2048)
    , inDataBlock_(false)
{

}

err_t SpiceParser::getNext(BKVItemView &item)
{
    while(fgets(keyBuffer_.data(), (int)keyBuffer_.size(), file_))
    {
        if(keyBuffer_[0] == '\\')
        {
            if(strncmp(keyBuffer_.data(), "\\begindata", 10) == 0)
            {
                inDataBlock_ = true;
            }else if(strncmp(keyBuffer_.data(), "\\begintext", 10) == 0)
            {
                inDataBlock_ = false;
            }
        }else{
            if(inDataBlock_)
            {
                StringView line = keyBuffer_.data();
                // 解析数据块中的键值对项
                auto pos = line.find('=');
                if(pos != StringView::npos)
                {
                    item.key() = aStripAsciiWhitespace(line.substr(0, pos));
                    StringView value = line.substr(pos + 1);
                    StringView valueStrip = aStripAsciiWhitespace(value);
                    item.value() = valueStrip;
                    if(valueStrip.empty())
                    {
                        aWarning("value is whitespace for key: %.*s", (int)item.key().size(), item.key().data());
                    }else if(valueStrip[0] == '(' && valueStrip.back() != ')'){
                        valueBuffer_.assign(value.begin(), value.size());
                        // 继续读取，直到找到完整的括号表达式
                        StringView lineStrip;
                        const size_t maxsize = valueBuffer_.max_size();
                        do{
                            StringView line = getLineWithNewline();
                            if(valueBuffer_.size() + line.size() >= maxsize)
                            {
                                aError("value buffer overflow for key: %.*s", (int)item.key().size(), item.key().data());
                                break;
                            }
                            valueBuffer_.append(line.begin(), line.size());
                            lineStrip = aStripTrailingAsciiWhitespace(line);
                        }while(lineStrip.back() != ')');
                        item.value() = valueBuffer_;
                    }
                    // {
                    //     printf("key: '%.*s'\n", (int)item.key().size(), item.key().data());
                    //     printf("value: '%.*s'\n", (int)item.value().size(), item.value().data());
                    // }
                    return eNoError;
                }else{
                    continue;
                }
            }
        }
    }
    static_assert(EOF != 0, "EOF must be non-zero");
    return EOF;
}

AST_NAMESPACE_END


