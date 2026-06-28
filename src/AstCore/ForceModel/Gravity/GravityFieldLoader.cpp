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
#include "AstCore/SolarSystem.hpp"


AST_NAMESPACE_BEGIN


using GravityFieldLoaderContext = GravityFieldLoader;

/// @brief 从GMAT格式文件(.cof文件)加载重力场
/// @return 错误码
static errc_t loadGravityFieldGMAT(GravityFieldLoaderContext& ctx);

/// @brief 从STK格式文件(.grv文件)加载重力场
/// @param parser 解析器
/// @return 错误码
static errc_t loadGravityFieldSTK(GravityFieldLoaderContext& ctx);


/// @brief 从ATK格式文件(.grv文件)加载重力场
/// @param ctx 上下文
/// @return 错误码
static errc_t loadGravityFieldATK(GravityFieldLoaderContext& ctx);


/// @brief 从GFC格式文件(.gfc文件)加载重力场
/// @param parser 解析器
/// @return 错误码
static errc_t loadGravityFieldGFC(GravityFieldLoaderContext& ctx);

/// @brief 从文件加载重力场
/// @param filepath 文件路径
/// @return 错误码
static errc_t loadGravityField(StringView model, GravityFieldLoaderContext& ctx);


/// @brief 打开重力场文件
/// @param parser 解析器
/// @param model 模型名称或文件路径
/// @param filepath 输出文件路径
/// @return 错误码
static errc_t openGravityFile(GravityFieldLoaderContext &ctx, StringView model, std::string& filepath)
{
    auto& parser = ctx.parser_;
    parser.open(model);
    if(!parser.isOpen()){
        // 判断是不是模型名称
        size_t last_dot = model.rfind('.');
        #ifdef _WIN32
        size_t last_slash = model.find_last_of("/\\");
        #else
        size_t last_slash = model.find_last_of('/');
        #endif
	    
        bool no_dir_sep = last_slash == StringView::npos;
        bool no_dot;
        if(no_dir_sep){
            no_dot = (last_dot == StringView::npos);
        }else{
            no_dot = (last_dot == StringView::npos) || ((last_dot != StringView::npos) && (last_dot < last_slash));
        }
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
                std::string(ctx.dirpath_),
                aGetSolarSystem()->getDirpath() + "/Earth/",
                aDataDirGet() + "/SolarSystem/Earth/" 
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
                        newfilepath = std::string(model) + suffix;
                    else
                        newfilepath = prefix + "/" + std::string(model) + suffix;
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
    filepath = std::string(model);
    return 0;
}


errc_t loadGravityField(StringView model, GravityFieldLoaderContext& ctx)
{
    std::string filepath;
    if(errc_t err = openGravityFile(ctx, model, filepath))
    {
        aError("failed to find gravity model '%.*s'", (int)model.size(), model.data());
        return err;
    }
    model = filepath;
    StringView firstline = ctx.parser_.getLine();
    if(firstline.starts_with("COMMENT") || model.ends_with(".cof"))
    {
        return loadGravityFieldGMAT(ctx);
    }
    else if(model.ends_with(".grv"))
    {
        if(firstline.starts_with("stk")){
            return loadGravityFieldSTK(ctx);
        }else{
            return loadGravityFieldATK(ctx);
        }
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

errc_t _loadGravityCoeffsXTK(GravityFieldLoaderContext& ctx, GravityField& gf, bool& skipRest)
{
    while(1)
    {
        StringView line = ctx.parser_.getLineSkipComment();
        int degree, order;
        double cnm, snm;
        // #pragma warning(suppress: 4996)
        if(line.empty()){
            break;
        }
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
                    skipRest = true;
                    return 0;
                }else{
                    continue;
                    // 忽略超出最大阶数的系数
                    
                    // aError(
                    //     "Invalid degree or order: %d %d, with max degree %d and max order %d", 
                    //     degree, order, gf.getMaxDegree(), gf.getMaxOrder()
                    // );
                    // return eErrorParse;
                }
            }
        }else{
            break;
        }
    }
    skipRest = false;
    return 0;
}

errc_t loadGravityFieldATK(GravityFieldLoaderContext& ctx)
{
    ctx.parser_.seek(0, std::ios_base::beg);
    FILE* file = ctx.parser_.getFile();
    if(!file){
        return eErrorInvalidFile;
    }
    bool loadCoeff = ctx.coeff_ != nullptr;
    GravityField gf;
    // 读取重力场头
    double v1, v2, v3;
    int status = fscanf(file, "%lf %lf %lf", &v1, &v2, &v3);
    if(status != 3){
        aError("failed to read gravity field header");
        return eErrorParse;
    }
    gf.model_ = fs::path(ctx.parser_.getFilePath()).stem();
    gf.normalized_ = true;
    if(v2 > v1)
    {
        gf.maxDegree_ = static_cast<int>(v1);
        gf.maxOrder_ = static_cast<int>(v1);
        gf.gm_ = v2;
        gf.refDistance_ = v3;
    }else{
        gf.maxDegree_ = static_cast<int>(v1);
        gf.maxOrder_ = static_cast<int>(v2);
        gf.gm_ = v3;
        status = fscanf(file, "%lf", &gf.refDistance_);
        if(status != 1){
            aError("failed to read gravity field header");
            return eErrorParse;
        }
    }
    gf.centralBody_ = fs::path(ctx.parser_.getFilePath()).parent_path().stem();
    gfInitCoeffMatrices(gf, ctx);
    // 读取系数
    bool skipRest = false;
    errc_t rc = _loadGravityCoeffsXTK(ctx, gf, skipRest);
    if(rc != 0){
        aError("failed to load gravity field coefficients");
        return rc;
    }
    if(ctx.head_ != nullptr)
        *ctx.head_ = gf.getHead();
    if(loadCoeff)
        *ctx.coeff_ = std::move(gf);
    return eNoError;
}

errc_t loadGravityFieldSecularVariations(GravityFieldLoaderContext& ctx, GravityFieldSecularVariations& variations)
{
    BKVParser::EToken token;
    BKVItemView item;
    variations.variations_.clear();
    do{
        token = ctx.parser_.getNext(item);
        if(token == BKVParser::eKeyValue)
        {
            if(aEqualsIgnoreCase(item.key(), "RefEpochTDT"))
            {
                variations.referenceEpoch_ = TimePoint::FromImpreciseJDTT(item.value().toDouble());
            }
            else if(aEqualsIgnoreCase(item.key(), "LinearRate"))
            {
                std::vector<ValueView> items = aStrSplit(item.value(), ByRepeatedWhitespace(), SkipEmpty());
                if(items.size() != 4){
                    aError("invalid linear rate format");
                    return eErrorParse;
                }
                GravityFieldSecularVariations::Variation variation{};
                
                variation.isSin_ = aEqualsIgnoreCase(items[0], "Sin");
                variation.degree_ = items[1].toInt();
                variation.order_ = items[2].toInt();
                variation.linearRate_ = items[3].toDouble();
                variations.variations_.push_back(variation);
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            break;
        }

    }while(token != BKVParser::eEOF);
    return eNoError;
}

void postProcessGravityFieldSecularVariations(GravityField& gf)
{
    auto& secularVariations = gf.secularVariations();
    secularVariations.normalized_ = gf.normalized_;
    for(auto& variation : secularVariations.variations_)
    {
        int degree = variation.degree_;
        int order = variation.order_;
        if(gf.isValidDegreeOrder(degree, order)){
            if(variation.isSin_){
                variation.originalCoefficient_ = gf.snm(degree, order);
            }else{
                variation.originalCoefficient_ = gf.cnm(degree, order);
            }
        }
        else
        {
            // 忽略超出最大阶数的系数
            aWarning("invalid degree or order: %d %d, with max degree %d and max order %d", 
                degree, order, gf.getMaxDegree(), gf.getMaxOrder()
            );
        }
    }
}

errc_t loadGravityFieldSTK(GravityFieldLoaderContext& ctx)
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
                bool skipRest = false;
                errc_t rc = _loadGravityCoeffsXTK(ctx, gf, skipRest);
                if(rc != 0){
                    aError("failed to load gravity field coefficients");
                    return rc;
                }
                // 重力场长期变化率加载逻辑的后处理
                postProcessGravityFieldSecularVariations(gf);
                if(skipRest){
                    goto endparse;
                }
            }
            else if(aEqualsIgnoreCase(item.value(), "SecularVariations"))
            {
                loadGravityFieldSecularVariations(ctx, gf.secularVariations());
            }
        }
        else if(token == BKVParser::eBlockEnd)
        {
            // pass
        }
        else if(token == BKVParser::eKeyValue)
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
            }else if(aEqualsIgnoreCase(item.key(), "ReferenceFrame"))
            {
                gf.referenceFrame_ = item.value().toString();
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

errc_t loadGravityFieldGFC(GravityFieldLoaderContext& ctx)
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
                    // GFC格式(ICGEM)潮汐系统标识:
                    //   "tide_free"  → 无潮汐模型 (不含永久潮汐变形)
                    //   "zero tide"  → 零潮汐模型 (含永久潮汐变形)
                    // 字段名和取值定义: ICGEM 格式规范 (International Centre for Global Earth Models)
                    //   规范主页: http://icgem.gfz-potsdam.de/ 或者 https://icgem.gfz.de/home
                    // 三种潮汐系统的物理含义: IERS 2010 TN36 第1.1节 (p.15)
                    if(aEqualsIgnoreCase(item.value(), "tide_free")){
                        gf.includesPermTide_ = false;
                    }else if(aEqualsIgnoreCase(item.value(), "zero tide")){
                        gf.includesPermTide_ = true;
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


errc_t loadGravityFieldGMAT(GravityFieldLoaderContext& ctx)
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




errc_t GravityFieldLoader::load(StringView filePath, GravityField & gravityField)
{
    head_ = nullptr;
    coeff_ = &gravityField;
    return loadGravityField(filePath, *this);
}

errc_t GravityFieldLoader::load(StringView filePath, GravityFieldHead& head)
{
    head_ = &head;
    coeff_ = nullptr;
    return loadGravityField(filePath, *this);
}


AST_NAMESPACE_END


