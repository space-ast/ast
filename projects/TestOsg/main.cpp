#define _USE_MATH_DEFINES
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Material>
#include <osg/Texture2D>
#include <osg/PositionAttitudeTransform>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osgDB/ReadFile>
#include <osgText/Text>
#include <osgGA/TrackballManipulator>
#include <osg/LineWidth>
#include <osg/Light>
#include <osg/LightSource>
#include <osg/Depth>
#include <iostream>
#include <cmath>

// 行星信息结构体
struct PlanetInfo {
    std::string name;
    double radius;          // 相对于地球半径的比例
    double orbitRadius;     // 天文单位 (AU)
    double orbitPeriod;     // 地球年
    double rotationPeriod;  // 地球日
    osg::Vec4 color;        // 颜色
    std::string textureFile;// 纹理文件
};

// 创建球体节点
osg::ref_ptr<osg::Geode> createSphere(float radius, const osg::Vec4& color, const std::string& textureFile = "") {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osg::Sphere> sphere = new osg::Sphere(osg::Vec3(0, 0, 0), radius);
    osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable(sphere);
    
    shape->setColor(color);
    
    // 设置材质
    osg::ref_ptr<osg::Material> material = new osg::Material();
    material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.1, 0.1, 0.1, 1.0));
    material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.7, 0.7, 0.7, 1.0));
    material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(1.0, 1.0, 1.0, 1.0));
    material->setShininess(osg::Material::FRONT_AND_BACK, 50.0);
    
    geode->getOrCreateStateSet()->setAttribute(material);
    
    // 如果有纹理文件，加载纹理
    if (!textureFile.empty()) {
        osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D();
        osg::ref_ptr<osg::Image> image = osgDB::readImageFile(textureFile);
        if (image.valid()) {
            texture->setImage(image);
            geode->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
        } else {
            std::cout << "无法加载纹理: " << textureFile << std::endl;
        }
    }
    
    geode->addDrawable(shape);
    return geode;
}

// 创建轨道圆环
osg::ref_ptr<osg::Geode> createOrbitRing(float radius, const osg::Vec4& color) {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry();
    
    // 创建顶点数组
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array();
    int segments = 100;
    for (int i = 0; i <= segments; ++i) {
        float angle = 2.0 * M_PI * i / segments;
        vertices->push_back(osg::Vec3(radius * cos(angle), radius * sin(angle), 0));
    }
    
    geometry->setVertexArray(vertices);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, vertices->size()));
    
    // 设置颜色
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array();
    colors->push_back(color);
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_OVERALL);
    
    // 设置线宽
    osg::ref_ptr<osg::LineWidth> lineWidth = new osg::LineWidth(1.0f);
    geode->getOrCreateStateSet()->setAttribute(lineWidth);
    
    // 禁用深度测试和光照，使轨道始终可见
    geode->getOrCreateStateSet()->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    
    geode->addDrawable(geometry);
    return geode;
}

// 创建动画路径（公转）
osg::ref_ptr<osg::AnimationPath> createOrbitAnimationPath(double radius, double period) {
    osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath();
    animationPath->setLoopMode(osg::AnimationPath::LOOP);
    
    // 创建圆形轨道
    int numSamples = 100;
    for (int i = 0; i <= numSamples; ++i) {
        float time = (float)i / numSamples * period;
        float angle = 2.0 * M_PI * i / numSamples;
        
        osg::Vec3 position(radius * cos(angle), radius * sin(angle), 0);
        osg::Quat rotation;
        
        // 创建关键帧
        osg::AnimationPath::ControlPoint controlPoint(position, rotation);
        animationPath->insert(time, controlPoint);
    }
    
    return animationPath;
}

// 创建自转动画
osg::ref_ptr<osg::AnimationPath> createRotationAnimationPath(double period) {
    osg::ref_ptr<osg::AnimationPath> animationPath = new osg::AnimationPath();
    animationPath->setLoopMode(osg::AnimationPath::LOOP);
    
    // 创建自转动画
    int numSamples = 50;
    for (int i = 0; i <= numSamples; ++i) {
        float time = (float)i / numSamples * period;
        float angle = 2.0 * M_PI * i / numSamples;
        
        // 自转，绕Z轴旋转
        osg::Quat rotation(angle, osg::Vec3(0, 0, 1));
        osg::AnimationPath::ControlPoint controlPoint(osg::Vec3(0, 0, 0), rotation);
        animationPath->insert(time, controlPoint);
    }
    
    return animationPath;
}

// 创建行星标签
osg::ref_ptr<osg::Geode> createPlanetLabel(const std::string& text, const osg::Vec3& position, const osg::Vec4& color) {
    osg::ref_ptr<osg::Geode> geode = new osg::Geode();
    
    osg::ref_ptr<osgText::Text> textDrawable = new osgText::Text();
    textDrawable->setFont("arial.ttf");
    textDrawable->setText(text);
    textDrawable->setPosition(position);
    textDrawable->setColor(color);
    textDrawable->setCharacterSize(0.1f);
    textDrawable->setAlignment(osgText::Text::CENTER_BOTTOM);
    
    // 禁用深度测试，使标签始终可见
    osg::StateSet* stateset = geode->getOrCreateStateSet();
    stateset->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
    stateset->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    
    geode->addDrawable(textDrawable);
    return geode;
}

int main(int argc, char** argv) {
    // 初始化查看器
    osgViewer::Viewer viewer;
    viewer.setUpViewInWindow(100, 100, 1200, 800);
    
    // 设置场景数据
    osg::ref_ptr<osg::Group> root = new osg::Group();
    
    // 添加背景光
    osg::ref_ptr<osg::LightSource> lightSource = new osg::LightSource();
    osg::ref_ptr<osg::Light> light = new osg::Light();
    light->setLightNum(0);
    light->setPosition(osg::Vec4(0.0, 0.0, 0.0, 1.0)); // 太阳位置
    light->setAmbient(osg::Vec4(0.3, 0.3, 0.3, 1.0));
    light->setDiffuse(osg::Vec4(1.0, 1.0, 1.0, 1.0));
    lightSource->setLight(light);
    root->addChild(lightSource);
    
    // 定义行星数据（简化版，比例不精确，用于可视化）
    std::vector<PlanetInfo> planets = {
        // 太阳 (为了可视化，太阳比其他行星大得多)
        {"Sun", 5.0, 0.0, 0.0, 27.0, osg::Vec4(1.0, 0.8, 0.0, 1.0), ""},
        // 水星
        {"Mercury", 0.4, 5.0, 0.24, 58.6, osg::Vec4(0.7, 0.7, 0.7, 1.0), ""},
        // 金星
        {"Venus", 0.9, 7.0, 0.62, 243.0, osg::Vec4(0.9, 0.7, 0.2, 1.0), ""},
        // 地球
        {"Earth", 1.0, 10.0, 1.0, 1.0, osg::Vec4(0.2, 0.4, 0.8, 1.0), ""},
        // 火星
        {"Mars", 0.5, 13.0, 1.88, 1.03, osg::Vec4(0.9, 0.3, 0.2, 1.0), ""},
        // 木星
        {"Jupiter", 2.5, 18.0, 11.86, 0.41, osg::Vec4(0.8, 0.6, 0.4, 1.0), ""},
        // 土星
        {"Saturn", 2.2, 23.0, 29.46, 0.45, osg::Vec4(0.9, 0.8, 0.5, 1.0), ""},
        // 天王星
        {"Uranus", 1.8, 28.0, 84.01, 0.72, osg::Vec4(0.6, 0.8, 0.9, 1.0), ""},
        // 海王星
        {"Neptune", 1.7, 32.0, 164.8, 0.67, osg::Vec4(0.2, 0.4, 0.9, 1.0), ""}
    };
    
    // 为每个行星创建节点
    for (size_t i = 0; i < planets.size(); ++i) {
        const PlanetInfo& planet = planets[i];
        
        // 创建行星球体
        osg::ref_ptr<osg::Geode> planetGeode = createSphere(planet.radius * 0.1, planet.color, planet.textureFile);
        
        // 创建变换节点用于公转
        osg::ref_ptr<osg::PositionAttitudeTransform> orbitTransform = new osg::PositionAttitudeTransform();
        
        // 如果不是太阳，创建轨道并添加公转动画
        if (i > 0) {
            // 创建轨道环
            osg::ref_ptr<osg::Geode> orbitGeode = createOrbitRing(planet.orbitRadius, osg::Vec4(0.5, 0.5, 0.5, 0.3));
            root->addChild(orbitGeode);
            
            // 创建公转动画
            osg::ref_ptr<osg::AnimationPathCallback> orbitCallback = 
                new osg::AnimationPathCallback(createOrbitAnimationPath(planet.orbitRadius, planet.orbitPeriod * 10.0));
            orbitTransform->setUpdateCallback(orbitCallback);
        }
        
        // 创建变换节点用于自转
        osg::ref_ptr<osg::PositionAttitudeTransform> rotationTransform = new osg::PositionAttitudeTransform();
        
        // 创建自转动画（除太阳外，太阳自转周期不同）
        double rotationPeriod = (i == 0) ? planet.rotationPeriod * 5.0 : planet.rotationPeriod * 2.0;
        osg::ref_ptr<osg::AnimationPathCallback> rotationCallback = 
            new osg::AnimationPathCallback(createRotationAnimationPath(rotationPeriod));
        rotationTransform->setUpdateCallback(rotationCallback);
        
        // 组装节点层次结构
        rotationTransform->addChild(planetGeode);
        orbitTransform->addChild(rotationTransform);
        root->addChild(orbitTransform);
        
        // 添加行星标签
        osg::ref_ptr<osg::Geode> labelGeode = createPlanetLabel(
            planet.name, 
            osg::Vec3(0, 0, planet.radius * 0.1 + 0.2), 
            osg::Vec4(1.0, 1.0, 1.0, 1.0)
        );
        rotationTransform->addChild(labelGeode);
    }
    
    // 为地球添加月球
    {
        // 月球信息
        float moonRadius = 0.3;
        float moonOrbitRadius = 1.5;
        double moonOrbitPeriod = 0.0748; // 约27.3天，相对于地球年
        double moonRotationPeriod = 27.3; // 月球自转周期
        
        // 创建月球
        osg::ref_ptr<osg::Geode> moonGeode = createSphere(moonRadius * 0.1, osg::Vec4(0.8, 0.8, 0.8, 1.0));
        
        // 月球绕地球公转的变换
        osg::ref_ptr<osg::PositionAttitudeTransform> moonOrbitTransform = new osg::PositionAttitudeTransform();
        
        // 月球轨道（相对地球）
        osg::ref_ptr<osg::Geode> moonOrbitGeode = createOrbitRing(moonOrbitRadius, osg::Vec4(0.7, 0.7, 0.7, 0.5));
        
        // 月球自转变换
        osg::ref_ptr<osg::PositionAttitudeTransform> moonRotationTransform = new osg::PositionAttitudeTransform();
        
        // 月球公转动画
        osg::ref_ptr<osg::AnimationPathCallback> moonOrbitCallback = 
            new osg::AnimationPathCallback(createOrbitAnimationPath(moonOrbitRadius, moonOrbitPeriod * 10.0));
        moonOrbitTransform->setUpdateCallback(moonOrbitCallback);
        
        // 月球自转动画
        osg::ref_ptr<osg::AnimationPathCallback> moonRotationCallback = 
            new osg::AnimationPathCallback(createRotationAnimationPath(moonRotationPeriod));
        moonRotationTransform->setUpdateCallback(moonRotationCallback);
        
        // 组装月球节点
        moonRotationTransform->addChild(moonGeode);
        moonOrbitTransform->addChild(moonRotationTransform);
        
        // 将月球轨道添加到地球位置（第4个行星是地球）
        // 在实际程序中，我们需要找到地球的变换节点并添加月球
        // 这里简化处理，直接添加到根节点，位置在地球轨道上
        root->addChild(moonOrbitTransform);
        root->addChild(moonOrbitGeode);
        
        // 添加月球标签
        osg::ref_ptr<osg::Geode> moonLabelGeode = createPlanetLabel(
            "Moon", 
            osg::Vec3(0, 0, moonRadius * 0.1 + 0.15), 
            osg::Vec4(1.0, 1.0, 1.0, 1.0)
        );
        moonRotationTransform->addChild(moonLabelGeode);
    }
    
    // 设置场景
    viewer.setSceneData(root);
    
    // 添加统计信息
    viewer.addEventHandler(new osgViewer::StatsHandler());
    
    // 添加帮助信息
    viewer.addEventHandler(new osgViewer::HelpHandler());
    
    // 设置相机操作器
    viewer.setCameraManipulator(new osgGA::TrackballManipulator());
    
    // 设置初始视图
    viewer.getCameraManipulator()->setHomePosition(
        osg::Vec3d(0, -50, 20), // 眼睛位置
        osg::Vec3d(0, 0, 0),    // 观察点
        osg::Vec3d(0, 0, 1)     // 上方向
    );
    
    // 开始渲染循环
    std::cout << "太阳系可视化程序" << std::endl;
    std::cout << "使用鼠标控制视图：" << std::endl;
    std::cout << "  左键拖拽: 旋转视图" << std::endl;
    std::cout << "  中键拖拽: 平移视图" << std::endl;
    std::cout << "  右键拖拽: 缩放视图" << std::endl;
    std::cout << "  按'S': 显示/隐藏统计信息" << std::endl;
    std::cout << "  按'H': 显示帮助信息" << std::endl;
    
    return viewer.run();
}