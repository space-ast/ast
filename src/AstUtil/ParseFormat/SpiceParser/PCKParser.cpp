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

#include "PCKParser.hpp"
#include "AstUtil/StringView.hpp"
#include <algorithm>

AST_NAMESPACE_BEGIN

static_assert(sizeof(std::vector<double>) == sizeof(std::vector<char>), "std::vector<double> and std::vector<char> must have the same size");
static_assert(sizeof(std::vector<double>) == sizeof(std::vector<int>), "std::vector<double> and std::vector<int> must have the same size");

PCKParser::PCKParser()
    : BaseParser()
    , keyBuffer_(1024)
    , valueBuffer_()
    , inDataBlock_(false)
{

}

PCKParser::PCKParser(StringView filepath)
    : BaseParser(filepath)
    , keyBuffer_(1024)
    , valueBuffer_()
    , inDataBlock_(false)
{

}

errc_t PCKParser::getNext(BKVItemView &item)
{
    auto file = getFile();
    if(!file)
        return eErrorInvalidFile;
    while(fgets(keyBuffer_.data(), (int)keyBuffer_.size(), file))
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
            if(!inDataBlock_)
                continue;
            StringView line = keyBuffer_.data();
            // 解析数据块中的键值对项
            auto pos = line.find('=');
            if(pos == StringView::npos)
                continue;
            item.key() = aStripAsciiWhitespace(line.substr(0, pos));
            StringView value = line.substr(pos + 1);
            StringView valueStrip = aStripAsciiWhitespace(value);
            item.value() = valueStrip;
            if(valueStrip.empty())
            {
                aWarning("value is whitespace for key: %.*s", (int)item.key().size(), item.key().data());
            }else if(valueStrip[0] == '(' && valueStrip.back() != ')'){
                valueBuffer_.assign(value.data(), value.size());
                // 继续读取，直到找到完整的括号表达式
                StringView lineStrip;
                constexpr size_t maxmem = 20 * 1024 * 1024; // 20 MB
                const size_t maxsize = (std::min)(valueBuffer_.max_size(), maxmem);
                do{
                    StringView line = getLineWithNewline();
                    if(valueBuffer_.size() + line.size() >= maxsize)
                    {
                        aError("value buffer overflow (exceeds %zu bytes) for key: %.*s",
                               maxsize, (int)item.key().size(), item.key().data());
                        break;
                    }
                    valueBuffer_.append(line.data(), line.size());
                    lineStrip = aStripTrailingAsciiWhitespace(line);
                }while(lineStrip.empty() || lineStrip.back() != ')');
                item.value() = valueBuffer_;
            }
            // {
            //     printf("key: '%.*s'\n", (int)item.key().size(), item.key().data());
            //     printf("value: '%.*s'\n", (int)item.value().size(), item.value().data());
            // }
            return eNoError;
        }
    }
    static_assert(EOF != 0, "EOF must be non-zero");
    return EOF;
}

errc_t PCKParser::readData(KernelPool &kernelPool)
{
    if(!isOpen())
    {
        aError("file is not open");
        return eErrorInvalidFile;
    }
    BKVItemView item;
    while(1){
        errc_t rc = getNext(item);
        if(rc == EOF)
            break;
        if(rc != eNoError)
            return rc;
        auto key = item.key();
        if(key.empty())
        {
            continue;
        }
        auto value = item.value();
        {
            std::vector<double> d;
            if(value.toFortranDoubleVector(d) == eNoError){
                kernelPool.setData(key, d);
            }else{
                aError("value is not double for key: %.*s", (int)key.size(), key.data());
            }
        }
    };
    return eNoError;
}

AST_NAMESPACE_END


