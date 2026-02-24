///
/// @file      GravityFieldLoader.cpp
/// @brief     
/// @details   
/// @author    axel
/// @date      2026-02-24
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
#include "GravityFieldLoader.hpp"
#include "AstUtil/BKVParser.hpp"
#include "AstUtil/String.hpp"
#include "AstUtil/Logger.hpp"
#include "AstUtil/FileSystem.hpp"
#include "AstUtil/Span.hpp"
#include "AstCore/RunTime.hpp"


AST_NAMESPACE_BEGIN


using GravityFieldLoaderContext = GravityFieldLoader;

/// @brief 从GMAT格式文件(.cof文件)加载重力场
/// @return 错误码
static err_t loadGravityFieldGMAT(GravityFieldLoaderContext& ctx);

/// @brief 从STK格式文件(.grv文件)加载重力场
/// @param parser 解析器
/// @return 错误码
static err_t loadGravityFieldSTK(GravityFieldLoaderContext& ctx);

/// @brief 从GFC格式文件(.gfc文件)加载重力场
/// @param parser 解析器
/// @return 错误码
static err_t loadGravityFieldGFC(GravityFieldLoaderContext& ctx);

/// @brief 从文件加载重力场
/// @param filepath 文件路径
/// @return 错误码
static err_t loadGravityField(StringView model, GravityFieldLoaderContext& ctx);


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
        size_t last_dot = model.rfind('.');
        #ifdef _WIN32
        size_t last_slash = model.find_last_of("/\\");
        #else
        size_t last_slash = model.find_last_of('/');
        #endif

        bool no_dot = (last_dot == StringView::npos) || ((last_dot != StringView::npos) && (last_dot < last_slash));
	    bool no_dir_sep = last_slash == StringView::npos;
        Span<char const* const> suffixes;
        std::vector<std::string> prefixes;
        if(no_dot){
            static const char* suffixes1[] = {".grv", ".cof", ".gfc" };
            suffixes = suffixes1;
        }
        else{
            static const char* suffixes2[] = {""};
            suffixes = suffixes2;
        }
        if(no_dir_sep){
            prefixes = {
                "",
                aDataDirGet() + "/SolarSystem/Earth/"  // @fixme: 非地球如何处理？
            };
        }else{
            prefixes = {""};
        }

        /// 遍历所有可能的路径组合
        {
            for(const std::string& prefix : prefixes){
                for(const char* suffix : suffixes)
                {
                    std::string newfilepath;
                    if(prefix.empty())
                        newfilepath = model.to_string() + suffix;
                    else
                        newfilepath = prefix + "/" + model.to_string() + suffix;
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
        }
        return eErrorInvalidFile;
    }
    filepath = model.to_string();
    return 0;
}


err_t loadGravityField(StringView model, GravityFieldLoaderContext& ctx)
{
    std::string filepath;
    if(err_t err = openGravityFile(ctx.parser_, model, filepath))
    {
        aError("failed to find gravity model %.*s", (int)model.size(), model.data());
        return err;
    }
    model = filepath;
    StringView firstline = ctx.parser_.getLine();
    if(firstline.starts_with("COMMENT") || model.ends_with(".cof"))
    {
        return loadGravityFieldGMAT(ctx);
    }
    else if(firstline.starts_with("stk") || model.ends_with(".grv"))
    {
        return loadGravityFieldSTK(ctx);
    }
    else if(model.ends_with(".gfc"))
    {
        return loadGravityFieldGFC(ctx);
    }
    aError(
        "unsupported gravity field format, checking by first line: %s and filepath: %s", 
        firstline.data(), model.data()
    );
    return eErrorParse;
}


static void gfInitCoeffMatrices(GravityField& gf, GravityFieldLoaderContext& ctx)
{
    if(ctx.maxLoadDegree_ > 0){
        gf.maxDegree_ = std::min(gf.maxDegree_, ctx.maxLoadDegree_);
    }
    if(ctx.maxLoadOrder_ > 0){
        gf.maxOrder_ = std::min(gf.maxOrder_, ctx.maxLoadOrder_);
    }
    gf.initCoeffMatrices();
}

/// @brief 检查重力场是否已经完成了系数加载
/// @param gf 重力场
/// @return 是否完成了系数加载
static bool gfCoeffIsLoaded(const GravityField& gf)
{
    if(gf.getCnm(gf.maxDegree_, gf.maxOrder_) != 0.0 || gf.getSnm(gf.maxDegree_, gf.maxOrder_) != 0.0)
    {
        for(int n = 2; n <= gf.getMaxDegree(); n++)
        {
            if(gf.getCnm(n, 0) == 0.0){
                return false;
            }
        }
        return true;
    }
    return false;
}

err_t loadGravityFieldSTK(GravityFieldLoaderContext& ctx)
{
    BKVParser::EToken token;
    BKVItemView item;
    GravityField gf;
    bool loadCoeff = ctx.coeff_ != nullptr;
    do{
        token = ctx.parser_.getNext(item);
        if(token == BKVParser::eBlockBegin)
        {
            if(aEqualsIgnoreCase(item.value(), "Coefficients") && loadCoeff)
            {
                gfInitCoeffMatrices(gf, ctx);
                while(1)
                {
                    StringView line = ctx.parser_.getLineSkipComment();
                    int degree, order;
                    double cnm, snm;
                    // #pragma warning(suppress: 4996)
                    int status = sscanf(
                        line.data(), 
                        "%d %d %lf %lf", 
                        &degree, &order, &cnm, &snm
                    );
                    if(status == 4){
                        if(A_LIKELY(gf.isValidDegreeOrder(degree, order)))
                        {
                            gf.snm(degree, order) = snm;
                            gf.cnm(degree, order) = cnm;
                        }else{
                            if(gfCoeffIsLoaded(gf)){
                                goto endparse;
                                // aWarning("already loaded coefficients, ignore the rest");
                                break;
                            }
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

endparse:
    if(ctx.head_ != nullptr)
        *ctx.head_ = gf.getHead();
    if(loadCoeff)
        *ctx.coeff_ = std::move(gf);
    return eNoError;
}

err_t loadGravityFieldGFC(GravityFieldLoaderContext& ctx)
{
    // 1. 查找关键词'product_type'
    while (true)
    {
        StringView line = ctx.parser_.getLineWithNewline();
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
    bool loadCoeff = ctx.coeff_ != nullptr;

    // 2. 读取头部信息
    {
        BKVItemView item;
        BKVParser::EToken token;
        bool findMaxOrder = false;
        do{
            token = ctx.parser_.getNext(item);
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
        gfInitCoeffMatrices(gf, ctx);
        
        while(true){
            StringView line = ctx.parser_.getLineWithNewline();
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
                if(A_LIKELY(gf.isValidDegreeOrder(degree, order))){
                    gf.cnm(degree, order) = c;
                    gf.snm(degree, order) = s;
                }else{
                    if(gfCoeffIsLoaded(gf)){
                        // aWarning("already loaded coefficients, ignore the rest");
                        break;
                    }
                    aError("gfc degree or order out of range: %d %d", degree, order);
                    return eErrorParse;
                }
            }
        }
    }
    if(ctx.head_ != nullptr)
        *ctx.head_ = gf.getHead();
    if(loadCoeff)
        *ctx.coeff_ = std::move(gf);
    return eNoError;
}


err_t loadGravityFieldGMAT(GravityFieldLoaderContext& ctx)
{
    GravityField gf;
    bool loadCoeff = ctx.coeff_ != nullptr;
    while(1)
    {
        StringView line = ctx.parser_.getLine();
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
            gfInitCoeffMatrices(gf, ctx);
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
            if(A_LIKELY(gf.isValidDegreeOrder(degree, order)))
            {
                gf.snm(degree, order) = snm;
                gf.cnm(degree, order) = cnm;
            }else{
                if(gfCoeffIsLoaded(gf)){
                    // aWarning("already loaded coefficients, ignore the rest");
                    break;
                }
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
    if(ctx.head_ != nullptr)
        *ctx.head_ = gf.getHead();
    if(loadCoeff)
        *ctx.coeff_ = std::move(gf);
    return eNoError;
}




err_t GravityFieldLoader::load(StringView filePath, GravityField & gravityField)
{
    head_ = nullptr;
    coeff_ = &gravityField;
    return loadGravityField(filePath, *this);
}

err_t GravityFieldLoader::load(StringView filePath, GravityFieldHead& head)
{
    head_ = &head;
    coeff_ = nullptr;
    return loadGravityField(filePath, *this);
}


AST_NAMESPACE_END


