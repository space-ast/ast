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
#include <stdio.h>

AST_NAMESPACE_BEGIN

// #define AST_DEBUG_PARSER 1


/// @brief     从文件中读取一行文本，移除行尾的换行符。
/// @param     buffer  用于存储读取文本的缓冲区。
/// @param     size    缓冲区的大小。
/// @param     file    文件指针，指向要读取的文件。
/// @return    如果成功读取到一行文本，返回指向缓冲区的指针；否则返回 nullptr。
static char* fgetline(char* buffer, int size, FILE* file)
{
    char* ret = fgets(buffer, size, file);
    if(ret != nullptr)
    {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
    return ret;
}


/// @brief     从文件中读取一行文本，跳过行首的空白字符，移除行尾的换行符。
/// @param     buffer  用于存储读取文本的缓冲区。
/// @param     size    缓冲区的大小。
/// @param     file    文件指针，指向要读取的文件。
/// @return    如果成功读取到一行文本，返回指向缓冲区的指针；否则返回 nullptr。
static char* fgetlinetrim(char* buffer, int size, FILE* file)
{
    int c;
    while ((c = fgetc(file)) != EOF && isspace(static_cast<unsigned char>(c)) && c != '\n') {
        // 跳过空白
    }
    
    if (c == EOF || c == '\n') {
        // 没有更多内容，返回空字符串
        if(size > 0)
            buffer[0] = '\0';
        return nullptr;
    }else{
        buffer[0] = static_cast<char>(c);
        char* ret = fgets(buffer + 1, size - 1, file);
        if(ret != nullptr)
        {
            size_t len = strlen(ret);
            char* end = ret + len - 1;
            while(end > buffer && isspace(static_cast<unsigned char>(*end)))
            {
                *end = '\0';
                end--;
            }
        }
        return buffer;
    }
}


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
    : file_(nullptr)
    , allowComment_(true)
    , keyBuffer(1024)
    , valueBuffer(1024)
{
}

BKVParser::BKVParser(StringView filepath)
    : BKVParser{}
{
    open(filepath);
}



BKVParser::~BKVParser()
{
    close();
}


int BKVParser::getLineNumber()
{
    return aCurrentLineNumber(file_);
}

BKVParser::EToken BKVParser::getNext(StringView &key, ValueView &value)
{
start:
    if(fscanf(file_, "%1024s", keyBuffer.data()) != EOF)
    {
        if(keyBuffer[0] == '#' && allowComment_)
        {
            // fscanf(file_, "%*[^\n]%*c");                           // 跳过注释行
            fgets(valueBuffer.data(), valueBuffer.size(), file_);     // 跳过注释行
            // printf("comment left parts: %s", value);
            // continue;
            goto start;
        }
        key = StringView(keyBuffer.data());
        if(stricmp(keyBuffer.data(), "BEGIN") == 0)
        {

            char* line = fgetlinetrim(valueBuffer.data(), valueBuffer.size(), file_);
            value = StringView(line);
            return eBegin;
        }
        else if(stricmp(keyBuffer.data(), "END") == 0)
        {
            char* line = fgetlinetrim(valueBuffer.data(), valueBuffer.size(), file_);
            value = StringView(line);
            return eEnd;
        }else{
            long pos = ftell(file_);        // 记录当前位置
            char* line = fgetlinetrim(valueBuffer.data(), valueBuffer.size(), file_);
            if(line && strnicmp(line, "END ", 4) == 0){
                fseek(file_, pos, SEEK_SET); // 回退到记录位置
                value = nullptr;
            }else{
                value = StringView(line);
            }
            return eKeyValue;
        }
    }
    return eError;
}

BKVParser::EToken BKVParser::getNext(BKVItemView &item)
{
    return getNext(item.key(), item.value());
}

StringView BKVParser::getLine()
{
    // @todo: 这个是不是需要跳过空行和注释行
    char* line = fgetline(valueBuffer.data(), valueBuffer.size(), file_);
    return StringView(line);
}

StringView BKVParser::getLineTrim()
{
    // @todo: 这个是不是需要跳过空行和注释行
    char* line = fgetlinetrim(valueBuffer.data(), valueBuffer.size(), file_);
    return StringView(line);
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
        }else if(token == eBegin)
        {
            sax.begin(item.value().toStringView());
        }else if(token == eEnd)
        {
            sax.end(item.value().toStringView());
        }
    }while(token != eError);
    return eNoError;
}

void BKVParser::open(StringView filepath)
{
    if(isOpen())
    {
        close();
    }
    file_ = ast_fopen(filepath.data(), "rb");  // 注意：这里使用二进制模式读取，避免文本模式下的ftell和fseek行为不一致
}

void BKVParser::close()
{
    if (file_ != stdout && file_ != nullptr && file_ != stderr)
    {
        fclose(file_);
    }
    file_ = nullptr;
}

void BKVParser::seek(std::streamoff pos, std::ios::seekdir dir)
{
    if (file_ != nullptr)
    {
        fseek(file_, pos, dir);
    }
    static_assert(std::ios::beg == SEEK_SET);
    static_assert(std::ios::cur == SEEK_CUR);
    static_assert(std::ios::end == SEEK_END);
}

std::streamoff BKVParser::tell()
{
    return std::streamoff(ftell(file_));
}

AST_NAMESPACE_END
