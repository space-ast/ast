///
/// @file      ValXMLLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-04-16
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

#include "ValXMLLoader.hpp"
#include "AstScript/ValDict.hpp"
#include "AstScript/Value.hpp"
#include "AstScript/ScriptAPI.hpp"
#include "AstUtil/XMLSax.hpp"
#include "AstUtil/XMLParser.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/Escape.hpp"
#include "AstUtil/Quantity.hpp"
#include "AstUtil/QuantityParser.hpp"
#include "AstUtil/UnitParser.hpp"
#include <stack>
#include <vector>

AST_NAMESPACE_BEGIN

namespace{

struct ParseContext{
    std::string name_;
    std::string class_;
    std::string text_;
    SharedPtr<Value> value_;
};

class ValXMLSaxBase: public XMLSax
{
public:
    Value* getValue()
    {
        return value_.get();
    }
protected:
    ParseContext& getCurrentContext()
    {
        if(currentDepth_ >= stack_.size())
        {
            stack_.resize(currentDepth_ + 1);
        }
        return stack_[currentDepth_];
    }
    ParseContext* getParentContext()
    {
        if(currentDepth_ == 0)
            return nullptr;
        size_t parentDepth = currentDepth_ - 1;
        if(parentDepth >= stack_.size())
        {
            stack_.resize(parentDepth + 1);
        }
        return &stack_[parentDepth];
    }
    /// @brief 将当前值插入到父上下文的字典中
    void insertCurrentValueToParentDict()
    {
        ParseContext& context = getCurrentContext();
        ParseContext* parentContextPtr = getParentContext();
        if(!parentContextPtr)
        {
            // aError("parent context is not found");
            return;
        }
        ParseContext& parentContext = *parentContextPtr;
        ValDict* parentDict{nullptr};
        if(!parentContext.value_)
        {
            parentDict = ValDict::New();
            parentContext.value_ = parentDict;
        }else{
            parentDict = parentContext.value_->toValDict();
        }
        if(parentDict)
        {
            if(context.name_.empty()){
                parentDict->insert(context.text_, context.value_.get());
            }else{
                parentDict->insert(context.name_, context.value_.get());
            }
        }else{
            aError("parent element is not a object or container");
        }
    }
protected:
    size_t currentDepth_{0};                ///< 当前解析深度，1表示根元素
    std::vector<ParseContext> stack_;       ///< 解析上下文栈
    SharedPtr<Value> value_;                ///< 当前解析到的值
};

class ValXMLSax: public ValXMLSaxBase
{
public:
    void startDocument() override
    {

    }
    void endDocument() override
    {

    }
    void startElement(StringView element, const AttributeList& attrs) override
    {
        currentDepth_ ++;  // 这里应该先增加深度，再处理元素，保证处理元素内部text时的深度与元素深度一致
        if(inSTKObjectScope_){
            if(element == "OBJECT")
            {
                ParseContext& context = getCurrentContext();
                context = {};
                context.name_ = attrs.get("Name").toString();
                context.class_ = attrs.get("Class").toString();
            }
            else if(element == "STRING")
            {
                ParseContext& context = getCurrentContext();
                context = {};
            }
            else{
                aError("unknown element '%.*s', expect 'OBJECT'", element.size(), element.data());
            }
        }
        else if(element == "STKOBJECT")
        {
            inSTKObjectScope_ = true;
        }else {
            aError("root element 'STKOBJECT' is not found");
        }
    }
    void endElement(StringView element) override
    {
        if(element == "STKOBJECT")
        {
            inSTKObjectScope_ = false;
            value_ = stack_[currentDepth_ + 1].value_;
        }
        else if(element == "OBJECT")
        {
            ParseContext& context = getCurrentContext();
            if(context.class_ == "string")
            {
                context.value_ = aNewValueString(aUnquote(context.text_));
            }
            else if(context.class_ == "link")
            {
                // pass, do nothing here
            }
            else if(context.class_ == "bool")
            {
                context.value_ = aNewValueBool(aParseBool(context.text_));
            }
            else if(context.class_ == "int")
            {
                context.value_ = aNewValueInt(aParseInt(context.text_));
            }
            else if(context.class_ == "date")
            {
                std::string datestr = aUnquote(context.text_);
                context.value_ = aNewValueString(datestr);
            }
            else if(context.class_ == "double")
            {
                Quantity quantity;
                errc_t rc = aQuantityParse(context.text_, quantity);
                // 很奇怪，有些数量值会以""结尾，导致解析数量值时失败，例如： 2.2 ""
                if(rc){
                    double d = 0.0;
                    StringView text = aStripAsciiWhitespace(context.text_);
                    rc = aParseDouble(text, d);
                    if(!text.ends_with("\"\"") || rc != eNoError)
                    {
                        aError("failed to parse quantity '%.*s'", text.size(), text.data());
                    }
                    context.value_ = aNewValueDouble(d);
                }else{
                    context.value_ = aNewValueQuantity(quantity);
                }
            }
            else if(!context.value_)
            {
                context.value_ = ValDict::New();
            }
            ParseContext* parentContextPtr = getParentContext();
            if(parentContextPtr)
            {
                ParseContext& parentContext = *parentContextPtr;
                if(parentContext.class_ == "link")
                {
                    parentContext.value_ = context.value_;
                }
                else
                {
                    insertCurrentValueToParentDict();
                }
            }
        }
        else if(element == "STRING")
        {
            ParseContext& context = getCurrentContext();
            context.value_ = aNewValueString(aUnquote(context.text_));
            insertCurrentValueToParentDict();
        }
        else
        {
            aError("unknown element '%.*s', expect 'OBJECT'", element.size(), element.data());
        }
        currentDepth_ --;
    }
    void characters(StringView text) override
    {
        ParseContext& context = getCurrentContext();
        context.text_ += std::string(text);
    }
    void comment(StringView text) override
    {
        // pass
    }
    void error(StringView msg) override
    {
        // pass
    }

private:
    bool inSTKObjectScope_{false};          ///< 是否在STKOBJECT元素范围内
};


class ValXMLSax2: public ValXMLSaxBase
{
public:
    void startDocument() override
    {

    }
    void endDocument() override
    {
        if(stack_.size() > 1)
        {
            value_ = stack_[1].value_;
        }
    }
    void startElement(StringView element, const AttributeList& attributes) override
    {
        currentDepth_ ++; // 这里应该先增加深度，再处理元素，保证处理元素内部text时的深度与元素深度一致
        ParseContext& context = getCurrentContext();
        context = {};
        context.name_ = attributes.get("name").toString();
        context.class_ = std::string(element);
        if(element == "QUANTITY")
        {
            // 针对QUANTITY元素，需要特殊处理
            context.name_ = attributes.get("Unit").toString();
        }
    }
    void endElement(StringView element) override
    {
        ParseContext& context = getCurrentContext();
        ParseContext* parentContextPtr = getParentContext();
        if(context.class_ == "VAR")
        {
            insertCurrentValueToParentDict();
        }
        else if(context.class_ == "INT")
        {
            context.value_ = aNewValueInt(aParseInt(context.text_));
        }
        else if(context.class_ == "REAL")
        {
            if(parentContextPtr && parentContextPtr->class_ == "QUANTITY"){
                parentContextPtr->text_ = context.text_;
            }else{
                context.value_ = aNewValueDouble(aParseDouble(context.text_));
            }
        }
        else if(context.class_ == "QUANTITY")
        {
            Unit unit;
            errc_t rc = aUnitParse(context.name_, unit);
            if(rc){
                aError("failed to parse unit '%s'", context.name_.c_str());
            }
            double magnitude = aParseDouble(context.text_);
            context.value_ = aNewValueQuantity(Quantity(magnitude, unit));
        }
        else if(context.class_ == "BOOL")
        {
            context.value_ = aNewValueBool(aParseBool(context.text_));
        }
        else if(context.class_ == "STRING")
        {
            context.value_ = aNewValueString(aUnquote(context.text_));
        }
        else if(context.class_ == "DATE")
        {
            StringView text = aStripAsciiWhitespace(context.text_);
            if(!text.empty() && !std::isalpha(text.back()))
            {
                // 如果没有指定时间系统，则默认是TAI
                std::string dateString = std::string(text) + " TAI";
                context.value_ = aNewValueString(dateString);
            }else{
                context.value_ = aNewValueString(text);
            }
        }
        else if(!context.value_)
        {
            context.value_ = ValDict::New();
        }
        
        if(parentContextPtr)
        {
            if(parentContextPtr->class_ == "VAR" || parentContextPtr->class_ == "LINKTOOBJ")
            {
                parentContextPtr->value_ = context.value_;
            }
        }
        currentDepth_ --;
    }
    void characters(StringView text) override
    {
        ParseContext& context = getCurrentContext();
        context.text_ += std::string(text);
    }
private:
    
};

}

errc_t aLoadValue(XMLParser& parser, SharedPtr<Value>& value)
{
    errc_t rc;
    auto token = parser.getNext();
    if(token == XMLParser::eComment)
    {
        ValXMLSax sax;
        rc = parser.parse(sax);
        value = sax.getValue();
    }else if(token == XMLParser::eProcessingInstruction)
    {
        ValXMLSax2 sax;
        rc = parser.parse(sax);
        value = sax.getValue();
    }else{
        aError("invalid XML format: expected comment or processing instruction at the beginning");
        return eErrorInvalidFile;
    }
#if 0
    if(auto dict = value->toValDict()){
        std::string json = dict->toJsonString(2);
        printf("json: %s\n", json.c_str());
    }
#endif
    return rc;
}



errc_t aLoadValue(StringView filepath, SharedPtr<Value>& value)
{
    XMLParser parser(filepath);
    return aLoadValue(parser, value);
}


errc_t aLoadValue(FILE* file, SharedPtr<Value>& value)
{
    if(!file)
        return eErrorNullInput;
    XMLParser parser;
    parser.setBorrowedFile(file);
    return aLoadValue(parser, value);
}

AST_NAMESPACE_END

