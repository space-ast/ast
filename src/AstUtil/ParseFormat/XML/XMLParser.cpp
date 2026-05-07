#include "AstUtil/XMLParser.hpp"
#include "AstUtil/XMLSax.hpp"
#include "AstUtil/XMLNode.hpp"
#include "AstUtil/XMLDomSax.hpp"
#include "AstUtil/Escape.hpp"
#include <string>
#include <sstream>
#include "XMLParser.hpp"

AST_NAMESPACE_BEGIN

#define _AST_XML_DEFAULT_BUFFER_SIZE 4096 


XMLParser::XMLParser() 
    : BaseParser()
    , pos_(0) 
{

}

XMLParser::XMLParser(StringView filepath) 
    : BaseParser(filepath)
    , pos_(0) 
{
    
}

XMLParser::~XMLParser()
{
    if(isOpen())
    {
        seekFileToCurrent();
    }
}

errc_t XMLParser::parse(XMLSax& sax) 
{
    // pos_ = 0;
    // buffer_.clear();
    int depth = 0;
    
    if(!isOpen()) {
        aError("failed to open file");
        return eErrorInvalidFile;
    }
    
    sax.startDocument();
    
    while (1) {
        EToken token = getNext();
        switch (token)
        {
        case eError:
            sax.error("failed to parse XML document");
            return eErrorInvalidFile;
        case eEndDocument:
            sax.endDocument();
            return eNoError;
            break;
        case eStartDocument:
            sax.startDocument();
            break;
        case eStartElement:
            depth++;
            sax.startElement(getName(), getAttributes());
            break;
        case eCharacters:
            sax.characters(getText());
            break;
        case eComment:
            sax.comment(getText());
            break;
        case eEndElement:
            depth--;
            sax.endElement(getName());
            break;
        default:
            // do nothing
            break;
        }
        if(depth == 0 && token == eEndElement)
        {
            this->seekFileToCurrent();  // 确保解析位置与文件指针位置一致，归还缓冲区剩余空间
            break;
        }
    }
    sax.endDocument();
    return eNoError;
}

errc_t XMLParser::parse(XMLNode &node)
{
    XMLDomSax sax(node);
    return parse(sax);
}

void XMLParser::advance(int n)
{
    pos_ += n;
}

// 辅助函数：跳过空白字符（空格、制表、换行、回车）
void XMLParser::skipWhitespace() 
{
    while (true) {
        int c = peek();
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            get();
        } else {
            break;
        }
    }
}


// 解析开始标签: <name attr="value" ... >
XMLParser::EToken XMLParser::parseStartElement() 
{
    advance(1);
    StringView content = parseUntil('>');
    unescapedText_.clear();
    aUnescapeXML(content, unescapedText_);
    content = unescapedText_;
    
    // 清空之前的属性
    attributes_.clear();
    
    size_t i = 0;
    // 跳过空白
    while (i < content.size() && (content[i] == ' ' || content[i] == '\t' || content[i] == '\n' || content[i] == '\r')) {
        i++;
    }
    
    // 解析元素名称
    size_t nameStart = i;
    while (i < content.size() && content[i] != ' ' && content[i] != '\t' && content[i] != '\n' && content[i] != '\r' && content[i] != '/' && content[i] != '>') {
        i++;
    }
    size_t nameLen = i - nameStart;
    if (nameLen == 0) {
        return eError;
    }
    setName(StringView(content.data() + nameStart, nameLen));
    
    // 解析属性
    while (i < content.size()) {
        // 跳过空白
        while (i < content.size() && (content[i] == ' ' || content[i] == '\t' || content[i] == '\n' || content[i] == '\r')) {
            i++;
        }
        
        // 检查是否到达标签末尾
        if (i >= content.size()) {
            break;
        }
        // 检查是否是自闭合标签
        else if (content[i] == '/')  
        {
            selfClosingTag_ = true;
            advance(-2);
            return eStartElement;
        }
        
        // 解析属性名
        size_t attrNameStart = i;
        while (i < content.size() && content[i] != '=' && content[i] != ' ' && content[i] != '\t' && content[i] != '\n' && content[i] != '\r') {
            i++;
        }
        size_t attrNameLen = i - attrNameStart;
        if (attrNameLen == 0) {
            return eError;
        }
        StringView attrName(content.data() + attrNameStart, attrNameLen);
        
        // 跳过空白
        while (i < content.size() && (content[i] == ' ' || content[i] == '\t' || content[i] == '\n' || content[i] == '\r')) {
            i++;
        }
        
        // 检查是否有等号
        if (i >= content.size() || content[i] != '=') {
            aError("expect '='");
            return eError;
        }
        i++;
        
        // 跳过空白
        while (i < content.size() && (content[i] == ' ' || content[i] == '\t' || content[i] == '\n' || content[i] == '\r')) {
            i++;
        }
        
        // 解析属性值（支持单引号和双引号）
        if (i >= content.size() || (content[i] != '"' && content[i] != '\'')) {
            return eError;
        }
        char quoteChar = content[i];
        i++;
        size_t attrValueStart = i;
        while (i < content.size() && content[i] != quoteChar) {
            i++;
        }
        if (i >= content.size()) {
            return eError;
        }
        size_t attrValueLen = i - attrValueStart;
        StringView attrValue(content.data() + attrValueStart, attrValueLen);
        i++;
        
        // 添加属性
        attributes_.emplace_back(attrName, attrValue);
    }
    
    
    
    return eStartElement;
}

// 解析结束标签: </name>
XMLParser::EToken XMLParser::parseEndElement() 
{
    advance(2);
    nameOrText_ = parseUntil('>');
    return eEndElement;
}

// 解析文本内容（直到 '<' 或 EOF）
XMLParser::EToken XMLParser::parseText() 
{
    StringView text = parseUntil('<');
    advance(-1);
    nameOrText_ = aStripTrailingAsciiWhitespace(text);
    unescapedText_.clear();
    aUnescapeXML(nameOrText_, unescapedText_);
    nameOrText_ = unescapedText_;
    return eCharacters;
}

// 解析注释 <!-- ... -->
XMLParser::EToken XMLParser::parseComment() 
{
    // 消耗 "<!--"
    advance(4);

    // 读取注释内容直到 "-->"
    StringView comment = parseUntil("-->");
    nameOrText_ = comment;
    return eComment;
}

// 解析 CDATA <![CDATA[ ... ]]>
XMLParser::EToken XMLParser::parseCDATA() 
{
    // 消耗 "<![CDATA["
    advance(9);

    StringView data = parseUntil("]]>");
    nameOrText_ = data;
    return eCharacters;
}

// 处理指令 <? ... ?>
XMLParser::EToken XMLParser::parsePI() 
{
    advance(2);
    parseUntil("?>");
    return eProcessingInstruction;
}

// DOCTYPE 声明
XMLParser::EToken XMLParser::parseDocTypeDecl() 
{
    // 消耗 "<!DOCTYPE"
    advance(9);
    
    // 简单跳过直到 '>'
    int depth = 1;
    while (depth > 0) {
        int c = get();
        if (c == EOF) return eError;
        if (c == '<') depth++;
        else if (c == '>') depth--;
    }
    return eDocTypeDecl;
}

StringView XMLParser::parseUntil(char stopChar) 
{
    size_t start = pos_;
    size_t current = pos_;
    while (true) {
        if (current >= buffer_.size()) {
            // 尝试从文件读取更多数据
            errc_t result = appendBuffer();
            if (result != eNoError || current >= buffer_.size()) {
                // 无法读取更多数据，返回当前内容
                pos_ = buffer_.size();
                return StringView(buffer_.data() + start, buffer_.size() - start);
            }
        }
        if (buffer_[current] == stopChar) {
            pos_ = current + 1; // 更新位置到停止字符之后
            return StringView(buffer_.data() + start, current - start);
        }
        current++;
    }
}

StringView XMLParser::parseUntil(StringView stopSeq) 
{
    if (stopSeq.empty()) {
        return StringView();
    }
    size_t start = pos_;
    size_t current = pos_;
    size_t matchIdx = 0;
    while (true) {
        if (current >= buffer_.size()) {
            // 尝试从文件读取更多数据
            errc_t result = appendBuffer();
            if (result != eNoError || current >= buffer_.size()) {
                // 无法读取更多数据，返回当前内容
                pos_ = buffer_.size();
                return StringView(buffer_.data() + start, buffer_.size() - start);
            }
        }
        if (buffer_[current] == stopSeq[matchIdx]) {
            matchIdx++;
            if (matchIdx == stopSeq.size()) {
                // 找到完整的停止序列
                pos_ = current + 1; // 更新位置到停止序列之后
                return StringView(buffer_.data() + start, current - start - matchIdx + 1);
            }
        } else {
            matchIdx = 0;
        }
        current++;
    }
}

// ========== getNext 主逻辑 ==========
XMLParser::EToken XMLParser::getNext() 
{   
    // 清空上一次的缓冲区
    if(pos_ > _AST_XML_DEFAULT_BUFFER_SIZE){
        buffer_.erase(buffer_.begin(), buffer_.begin() + pos_);
        pos_ = 0;
    }
    // 跳过可忽略的声明 / 处理指令，直到产生有效事件
    while (true) 
    {
        // 跳过空格
        skipWhitespace();
        StringView str = ensure(100);
        char c = str[0];
        
        if (c == '<') 
        {
            char c2 = str[1];
            if (c2 == '!') 
            {
                if (str.substr(2, 2) == "--") {
                    return parseComment();
                } else if (str.substr(2, 7) == "[CDATA[") {
                    return parseCDATA();
                } else if (str.substr(2, 9) == "DOCTYPE") {
                    return parseDocTypeDecl();
                } else {
                    return eError;
                }
            } else if (c2 == '?') {
                return parsePI();
            } else if (c2 == '/') {
                return parseEndElement();
            } else {
                return parseStartElement();
            }
        }
        else if(c == '/')
        {
            if (selfClosingTag_) {
                selfClosingTag_ = false;
                if(str[1] == '>'){
                    advance(2);
                    return eEndElement;
                }
            }
            return parseText(); 
        }
        else if(c == '\0')
        {
            return eEndDocument;
        }
        else {
            return parseText();
        }
    }
}

errc_t XMLParser::appendBuffer(size_t size)
{
    // 这里需要先确保缓冲区有足够的空间，然后再进行错误检查
    const size_t oldSize = buffer_.size();
    buffer_.resize(oldSize + size);
    if(!isOpen())
    {
        return eErrorInvalidFile;
    }
    if (eof()) {
        return eErrorInvalidFile;
    }
    size_t s = fread(buffer_.data() + oldSize, 1, size, getFile());
    if(s != size)
    {
        std::fill_n(buffer_.data() + oldSize + s, size - s, '\0');
        // buffer_.resize(oldSize + s);
    }
    return eNoError;
}

errc_t XMLParser::appendBuffer()
{
    return appendBuffer(_AST_XML_DEFAULT_BUFFER_SIZE);
}



int XMLParser::peek() 
{
    if(A_UNLIKELY(pos_ >= buffer_.size()))
    {
        appendBuffer();
        if(pos_ >= buffer_.size())
            return EOF;
    }
    return buffer_[pos_];
}


int XMLParser::get() {
    int c = peek();
    pos_++;
    return c;
}

StringView XMLParser::ensure(size_t n)
{
    size_t expectSize = pos_ + n;
    if(A_UNLIKELY(expectSize > buffer_.size()))
    {
        appendBuffer(expectSize - buffer_.size());
        if(expectSize > buffer_.size())
            return StringView(buffer_.data() + pos_, buffer_.size() - pos_);
    }
    return StringView(buffer_.data() + pos_, n);
}

void XMLParser::seekFileToCurrent()
{
    ptrdiff_t offset = pos_ - buffer_.size();
    if(offset != 0){
        buffer_.resize(pos_);
        this->seek(offset, std::ios_base::cur);
    }
}

AST_NAMESPACE_END
