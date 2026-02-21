///
/// @file      GravityField.cpp
/// @brief     ~
/// @details   ~
/// @author    axel
/// @date      2026-01-05
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

#include "GravityField.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstCore/RunTime.hpp"
#include <cmath>



AST_NAMESPACE_BEGIN

void aGravityFieldNormalize(GravityField &gf)
{
    gf.normalize();
}

void aGravityFieldUnnormalize(GravityField &gf)
{
    gf.unnormalize();
}

/// @brief 从GMAT格式文件(.cof文件)加载重力场
/// @param parser 解析器
/// @return 错误码
static err_t loadGravityFieldGMAT(BKVParser &parser, GravityFieldHead *head, GravityField *coeff);

/// @brief 从STK格式文件(.grv文件)加载重力场
/// @param parser 解析器
/// @return 错误码
static err_t loadGravityFieldSTK(BKVParser &parser, GravityFieldHead *head, GravityField *coeff);

/// @brief 从GFC格式文件(.gfc文件)加载重力场
/// @param parser 解析器
/// @return 错误码
static err_t loadGravityFieldGFC(BKVParser &parser, GravityFieldHead *head, GravityField *coeff);

/// @brief 从文件加载重力场
/// @param filepath 文件路径
/// @return 错误码
static err_t loadGravityField(StringView filepath, GravityFieldHead *head, GravityField *coeff);


/// @brief 打开重力场文件
/// @param parser 解析器
/// @param model 模型名称或文件路径
/// @param filepath 输出文件路径
/// @return 错误码
static err_t openGravityFile(BKVParser &parser, StringView model, std::string& filepath)
{
    parser.open(model);
    if(!parser.isOpen()){
        // 判断是不是模型名称
        bool has_dot = model.rfind('.') != StringView::npos;
	    bool has_dir_sep = model.rfind('/') != StringView::npos;
        if(!has_dot && !has_dir_sep)
        {
            // 模型名称，添加默认路径
            static const char* suffixes[]{ ".grv", ".cof", ".gfc" };
            std::string datadir = aDataDirGet() + "/SolarSystem/Earth/"; // @fixme: 非地球如何处理？
            // bool found = false;
            for(const char* suffix : suffixes)
            {
                std::string newfilepath = datadir + "/" + model.to_string() + suffix;
                if(fs::exists(newfilepath))
                {
                    parser.open(newfilepath);
                    if(parser.isOpen())
                    {
                        filepath = std::move(newfilepath);
                        return 0;
                    }
                }
            }
        }
        return eErrorInvalidFile;
    }
    filepath = model.to_string();
    return 0;
}


err_t GravityFieldHead::load(StringView filepath)
{
    return loadGravityField(filepath, this, nullptr);
}


GravityField::GravityField()
    : GravityFieldHead()
{

}

err_t loadGravityField(StringView model, GravityFieldHead *head, GravityField *coeff)
{
    BKVParser parser;
    std::string filepath;
    if(err_t err = openGravityFile(parser, model, filepath))
    {
        aError("failed to find gravity model %.*s", (int)model.size(), model.data());
        return err;
    }
    model = filepath;
    StringView firstline = parser.getLine();
    if(firstline.starts_with("COMMENT") || model.ends_with(".cof"))
    {
        return loadGravityFieldGMAT(parser, head, coeff);
    }
    else if(firstline.starts_with("stk") || model.ends_with(".grv"))
    {
        return loadGravityFieldSTK(parser, head, coeff);
    }
    else if(model.ends_with(".gfc"))
    {
        return loadGravityFieldGFC(parser, head, coeff);
    }
    aError(
        "unsupported gravity field format, checking by first line: %s and filepath: %s", 
        firstline.data(), model.data()
    );
    return eErrorParse;
}

err_t GravityField::load(StringView model)
{
    return loadGravityField(model, nullptr, this);
}

/// @brief 计算重力场系数的归一化因子
/// @param n 阶数 degree
/// @param m 次数 order
/// @return 归一化因子
static double gfNormalizeFactor(int n, int m)
{
    /*
    参考：航天器轨道力学理论与方法 附录C 公式C-1-3
    */
    int delta;
    if(m == 0)
        delta = 0;
    else
        delta = 1;

    double factor = 1.0;
    // if(m > 0){
        for(int i = n - m + 1; i <= n + m; i++)
            factor *= i;
    // }
    factor = sqrt(factor / ((1 + delta)*(2 * n + 1)) );
    return factor;
}

void GravityField::normalize()
{
    if(normalized_)
        return;
    normalized_ = true;
    for(int n = 2; n <= maxDegree_; n++)
    {
        for(int m = 0; m <= n; m++)
        {
            double factor = gfNormalizeFactor(n, m);
            snm(n, m) *= factor;
            cnm(n, m) *= factor;
        }
    }
}

void GravityField::unnormalize() 
{
    if(!normalized_)
        return;
    normalized_ = false;
    for(int n = 0; n <= maxDegree_; n++)
    {
        for(int m = 0; m <= n; m++)
        {
            double factor = gfNormalizeFactor(n, m);
            snm(n, m) /= factor;
            cnm(n, m) /= factor;
        }
    }
}

GravityField GravityField::normalized() const
{
    GravityField gf_normalized = *this;
    aGravityFieldNormalize(gf_normalized);
    return gf_normalized;
}

GravityField GravityField::unnormalized() const
{
    GravityField gf_unnormalized = *this;
    aGravityFieldUnnormalize(gf_unnormalized);
    return gf_unnormalized;
}

err_t loadGravityFieldSTK(BKVParser &parser, GravityFieldHead *head, GravityField *coeff)
{
    BKVParser::EToken token;
    BKVItemView item;
    GravityField gf;
    bool loadCoeff = coeff != nullptr;
    do{
        token = parser.getNext(item);
        if(token == BKVParser::eBlockBegin)
        {
            if(aEqualsIgnoreCase(item.value(), "Coefficients") && loadCoeff)
            {
                gf.initCoeffMatrices();
                while(1)
                {
                    StringView line = parser.getLineSkipComment();
                    int degree, order;
                    double cnm, snm;
                    // #pragma warning(suppress: 4996)
                    int status = sscanf(
                        line.data(), 
                        "%d %d %lf %lf", 
                        &degree, &order, &cnm, &snm
                    );
                    if(status == 4){
                        if(gf.isValidDegreeOrder(degree, order))
                        {
                            gf.snm(degree, order) = snm;
                            gf.cnm(degree, order) = cnm;
                        }else{
                            aError(
                                "Invalid degree or order: %d %d, with max degree %d and max order %d", 
                                degree, order, gf.getMaxDegree(), gf.getMaxOrder()
                            );
                            return eErrorParse;
                        }
                    }else{
                        break;
                    }
                }
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            // pass
        }else if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "Model"))
            {
                gf.model_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "CentralBody"))
            {
                gf.centralBody_ = item.value().toString();
            }else if(aEqualsIgnoreCase(item.key(), "Degree"))
            {
                gf.maxDegree_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "Order"))
            {
                gf.maxOrder_ = item.value().toInt();
            }else if(aEqualsIgnoreCase(item.key(), "GM"))
            {
                gf.gm_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "RefDistance"))
            {
                gf.refDistance_ = item.value().toDouble();
            }else if(aEqualsIgnoreCase(item.key(), "Normalized"))
            {
                gf.normalized_ = item.value().toBool();
            }else if(aEqualsIgnoreCase(item.key(), "IncludesPermTide"))
            {
                gf.includesPermTide_ = item.value().toBool();
            }
        }
    }while(token != BKVParser::EToken::eEOF);
    if(head)
        *head = gf.getHead();
    if(loadCoeff)
        *coeff = std::move(gf);
    return eNoError;
}

err_t loadGravityFieldGFC(BKVParser &parser, GravityFieldHead *head, GravityField *coeff)
{
    // 1. 查找关键词'product_type'
    while (true)
    {
        StringView line = parser.getLineWithNewline();
        if(line.empty()){
            aError("missing 'product_type' keyword in .gfc file.");
            return eErrorParse;
        }
        if(line.starts_with("product_type")){
            StringView productType = aStripAsciiWhitespace(line.substr(12));
            if(productType != "gravity_field"){
                aError("unsupported product type: '%.*s', expected 'gravity_field'", productType.size(), productType.data());
                return eErrorParse;
            }
            break;
        }
    }
    GravityField gf;
    gf.normalized_ = true;  // gfc文件系数默认归一化

    bool hasErrors = false;
    bool loadCoeff = coeff != nullptr;

    // 2. 读取头部信息
    {
        BKVItemView item;
        BKVParser::EToken token;
        bool findMaxOrder = false;
        do{
            token = parser.getNext(item);
            if(token == BKVParser::eKeyValue){
                if(aEqualsIgnoreCase(item.key(), "modelname"))
                {
                    gf.model_ = item.value().toString();
                }
                else if(aEqualsIgnoreCase(item.key(), "earth_gravity_constant"))
                {
                    gf.centralBody_ = "Earth";
                    gf.gm_ = item.value().toDouble();
                }
                else if(aEqualsIgnoreCase(item.key(), "radius"))
                {
                    gf.refDistance_ = item.value().toDouble();
                }
                else if(aEqualsIgnoreCase(item.key(), "max_degree"))
                {
                    gf.maxDegree_ = item.value().toInt();
                    if(!findMaxOrder)
                        gf.maxOrder_ = gf.maxDegree_;
                }
                else if(aEqualsIgnoreCase(item.key(), "max_order"))
                {
                    gf.maxOrder_ = item.value().toInt();
                    findMaxOrder = true;
                }
                else if(aEqualsIgnoreCase(item.key(), "errors"))
                {
                    if(aEqualsIgnoreCase(item.value(), "no"))
                        hasErrors = false;
                    else if(aEqualsIgnoreCase(item.value(), "formal"))
                        hasErrors = true;
                }
                else if(aEqualsIgnoreCase(item.key(), "norm")){
                    if(aEqualsIgnoreCase(item.value(), "fully_normalized"))
                        gf.normalized_ = true;
                    else{
                        // todo
                    }
                }
                else if(aEqualsIgnoreCase(item.key(), "tide_system"))
                {
                    if(aEqualsIgnoreCase(item.value(), "tide_free")){
                        // todo
                    }else if(aEqualsIgnoreCase(item.value(), "zero tide")){
                        // todo
                    }
                }
                else if(aEqualsIgnoreCase(item.key(), "J2-DOT"))
                {
                    // todo
                }
                else if(aEqualsIgnoreCase(item.key(), "end_of_head"))
                {
                    break;
                }
            }
        }while(token != BKVParser::EToken::eEOF);
    }
    // 3. 读取系数
    if(loadCoeff){
        gf.initCoeffMatrices();
        while(true){
            StringView line = parser.getLineWithNewline();
            if(line.empty()){
                break;
            }
            int status;
            if(line.starts_with("gfc")){
                StringView lineData = line.substr(3);
                int degree, order;
                double c, s;
                if(hasErrors){
                    double sigma_c, sigma_s;
                    status = sscanf(lineData.data(), "%d %d %lf %lf %lf %lf", &degree, &order, &c, &s, &sigma_c, &sigma_s);
                    if(status != 6){
                        aError("invalid gfc line: '%.*s'", lineData.size(), lineData.data());
                        return eErrorParse;
                    }
                }else{
                    status = sscanf(lineData.data(), "%d %d %lf %lf", &degree, &order, &c, &s);
                    if(status != 4){
                        aError("invalid gfc line: '%.*s'", lineData.size(), lineData.data());
                        return eErrorParse;
                    }
                }
                if(gf.isValidDegreeOrder(degree, order)){
                    gf.cnm(degree, order) = c;
                    gf.snm(degree, order) = s;
                }else{
                    aError("gfc degree or order out of range: %d %d", degree, order);
                    return eErrorParse;
                }
            }
        }
    }
    if(head)
        *head = gf.getHead();
    if(loadCoeff)
        *coeff = std::move(gf);
    return eNoError;
}

err_t GravityField::loadSTK(BKVParser &parser)
{
    return loadGravityFieldSTK(parser, nullptr, this);
}

err_t loadGravityFieldGMAT(BKVParser &parser, GravityFieldHead *head, GravityField *coeff)
{
    GravityField gf;
    bool loadCoeff = coeff != nullptr;
    while(1)
    {
        StringView line = parser.getLine();
        if(line.empty()){
            break;
        }
        if(line[0] == 'C')
        {
            // COMMENT line
            continue;
        }
        else if(line.starts_with("POTFIELD"))
        {
            gf.maxDegree_ = aParseInt(line.substr(8, 3));
            gf.maxOrder_ = aParseInt(line.substr(11, 3));
            int cbFlag;
            double normalizedFlag;
            // #pragma warning(suppress: 4996)
            sscanf(line.data() + 14, "%d %lf %lf %lf", &cbFlag, &gf.gm_, &gf.refDistance_, &normalizedFlag);   
            gf.normalized_ = (normalizedFlag != 0.0);
            (void) cbFlag; // @todo: process cbFlag
            gf.initCoeffMatrices();
        }
        else if(line.starts_with("RECOEF"))
        {
            if(!loadCoeff){
                break;
            }
            int degree = aParseInt(line.substr(8, 3));
            int order = aParseInt(line.substr(11, 3));
            double cnm, snm;
            int status;
            if(order == 0){
                // #pragma warning(suppress: 4996)
                status = sscanf(line.data() + 17, "%lf", &cnm);
                snm = 0;
            }else{
                // #pragma warning(suppress: 4996)
                status = sscanf(line.data() + 17, "%lf %lf", &cnm, &snm);
            }
            if(status == EOF){
                aError("Invalid coefficient line: %s", line.data());
                return eErrorParse;
            }
            if(gf.isValidDegreeOrder(degree, order))
            {
                gf.snm(degree, order) = snm;
                gf.cnm(degree, order) = cnm;
            }else{
                aError(
                    "Invalid degree or order: %d %d, with max degree %d and max order %d", 
                    degree, order, gf.maxDegree_, gf.maxOrder_
                );
                return eErrorParse;
            }
        }
        else if(line.starts_with("END"))
        {
            // end of coefficients
        }
    }
    if(head)
        *head = gf.getHead();
    if(loadCoeff)
        *coeff = std::move(gf);
    return eNoError;
}


err_t GravityField::loadGMAT(BKVParser &parser)
{
    return loadGravityFieldGMAT(parser, nullptr, this);
}

void GravityField::initCoeffMatrices()
{
    int size = std::max(maxDegree_, maxOrder_) + 1;
    sinCoeff_.resize(size);
    cosCoeff_.resize(size);
    sinCoeff_.setZero();
    cosCoeff_.setZero();
}



AST_NAMESPACE_END

