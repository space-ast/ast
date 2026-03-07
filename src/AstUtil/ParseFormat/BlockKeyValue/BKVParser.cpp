///
/// @file      BlockKeyValueParser.cpp
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

#include "BKVParser.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/BKVSax.hpp"
#include "AstUtil/IO.hpp"
#include "AstUtil/BKVItemView.hpp"
#include <cstdio>

AST_NAMESPACE_BEGIN

// #define AST_DEBUG_PARSER 1




static bool isCommentLine(StringView line)
{
    for(char c : line)
    {
        if(!isspace(static_cast<unsigned char>(c)))
        {
            return c == '#';
        }
    }
    return true;
}

BKVParser::BKVParser()
    : BaseParser()
    , allowComment_(true)
    , keyBuffer_(1024)
    , valueBuffer_(1024)
{
}

BKVParser::BKVParser(StringView filepath)
    : BaseParser{filepath}
    , allowComment_(true)
    , keyBuffer_(1024)
    , valueBuffer_(1024)
{
}



BKVParser::~BKVParser()
{
}



BKVParser::EToken BKVParser::getNext(StringView &key, ValueView &value)
{
start:
    // #pragma warning(suppress: 4996)
    if(fscanf(file_, "%1023s", keyBuffer_.data()) != EOF)
    {
        if(keyBuffer_[0] == '#' && allowComment_)
        {
            // fscanf(file_, "%*[^\n]%*c");                           // 跳过注释行
            char* str = fgets(valueBuffer_.data(), (int)valueBuffer_.size(), file_);     // 跳过注释行
            A_UNUSED(str);
            // printf("comment left parts: %s", value);
            // continue;
            goto start;
        }
        key = StringView(keyBuffer_.data());
        if(strcasecmp(keyBuffer_.data(), "BEGIN") == 0)
        {

            char* line = fgetlinetrim(valueBuffer_.data(), (int)valueBuffer_.size(), file_);
            value = StringView(line);
            return eBlockBegin;
        }
        else if(strcasecmp(keyBuffer_.data(), "END") == 0)
        {
            char* line = fgetlinetrim(valueBuffer_.data(), (int)valueBuffer_.size(), file_);
            value = StringView(line);
            return eBlockEnd;
        }else{
            long pos = ftell(file_);        // 记录当前位置
            char* line = fgetlinetrim(valueBuffer_.data(), (int)valueBuffer_.size(), file_);
            if(line && strncasecmp(line, "END ", 4) == 0){
                fseek(file_, pos, SEEK_SET); // 回退到记录位置
                value = nullptr;
            }else{
                value = StringView(line);
            }
            return eKeyValue;
        }
    }
    return eEOF;
}

BKVParser::EToken BKVParser::getNext(BKVItemView &item)
{
    return getNext(item.key(), item.value());
}

StringView BKVParser::getLineSkipComment()
{
    StringView line;
    do{
        line = getLine();
    }while(isCommentLine(line));
    return line;
}

err_t BKVParser::parseFile(const StringView filepath, BKVSax &sax)
{
    open(filepath);
    return parse(sax);
}


err_t BKVParser::parse(BKVSax &sax)
{
    if(!isOpen())
    {
        return eErrorInvalidFile;
    }
    BKVItemView item;
    EToken token;
    do
    {
        token = getNext(item);
        if(token == eKeyValue)
        {
            sax.keyValue(item.key(), item.value().toValue());
        }else if(token == eBlockBegin)
        {
            sax.begin(item.value().toStringView());
        }else if(token == eBlockEnd)
        {
            sax.end(item.value().toStringView());
        }
    }while(token != eEOF);
    return eNoError;
}

AST_NAMESPACE_END
