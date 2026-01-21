import{_ as n}from"./plugin-vue_export-helper-DlAUqK2U.js";import{c as i,d as a,o as l}from"./app-CMWO2-_H.js";const p={};function e(t,s){return l(),i("div",null,[...s[0]||(s[0]=[a(`<h1 id="坐标转换模块" tabindex="-1"><a class="header-anchor" href="#坐标转换模块"><span>坐标转换模块</span></a></h1><p><em>说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。</em></p><h2 id="模块概述" tabindex="-1"><a class="header-anchor" href="#模块概述"><span>模块概述</span></a></h2><p>坐标变换模块提供了天体力学和航天动力学中常用的各种坐标系之间的转换功能。该模块支持多种参考系之间的精确转换，包括惯性系、固连系以及各种中间参考系，为轨道计算、姿态控制等应用提供基础支持。</p><h2 id="核心概念" tabindex="-1"><a class="header-anchor" href="#核心概念"><span>核心概念</span></a></h2><h3 id="坐标系定义" tabindex="-1"><a class="header-anchor" href="#坐标系定义"><span>坐标系定义</span></a></h3><p>模块支持以下主要坐标系：</p><ul><li><strong>CBI (Central Body Inertial Frame)</strong>：中心天体惯性系</li><li><strong>CBF (Central Body Fixed Frame)</strong>：中心天体固连系</li><li><strong>ECI (Earth Centered Inertial Frame)</strong>：地球惯性系，根据配置可表示ICRF或J2000</li><li><strong>J2000</strong>：地球时(TT)2000年1月1日12时0分0秒的地球平赤道系</li><li><strong>MOD (Mean of Date)</strong>：平赤道系，考虑岁差</li><li><strong>TOD (True of Date)</strong>：真赤道系，考虑岁差和章动</li><li><strong>GTOD (Greenwich True of Date)</strong>：格林尼治真赤道系，XY平面为赤道平面，正X轴指向本初子午线，随地球旋转 <ul><li>又称TDR、GCR、PEF或EFG</li></ul></li><li><strong>ECF (Earth Centered Fixed Frame)</strong>：地球固连系，在GTOD基础上考虑了极移</li><li><strong>ICRF (International Celestial Reference Frame)</strong>：国际天球参考系</li><li><strong>CIRF (Celestial Intermediate Reference Frame)</strong>：天球中间参考系 <ul><li>第一极（z轴方向）为给定时刻的天球中间极（CIP）</li><li>基本平面为与该时刻的天球中间极垂直的中间赤道</li><li>经度坐标起量点为天球中间零点</li></ul></li><li><strong>TIRF (Terrestrial Intermediate Reference Frame)</strong>：地球中间参考系 <ul><li>与国际地球参考系（ITRF）的联系由极移和地球中间零点的定位角确定</li><li>通过绕天球中间极CIP的转动角度（地球自转角ERA）与CIRF相联系</li></ul></li></ul><h3 id="坐标变换类型" tabindex="-1"><a class="header-anchor" href="#坐标变换类型"><span>坐标变换类型</span></a></h3><p>模块提供三种主要的坐标变换接口：</p><ol><li><strong>Transform</strong>：返回Rotation或KinematicRotation对象</li><li><strong>Matrix</strong>：返回坐标转换矩阵</li><li><strong>直接转换</strong>：直接将坐标从一个参考系转换到另一个参考系</li></ol><p>部分变换还支持带速度的转换，考虑旋转坐标系的角速度影响。</p><h2 id="主要功能" tabindex="-1"><a class="header-anchor" href="#主要功能"><span>主要功能</span></a></h2><p>模块支持以下主要坐标系之间的转换：</p><ul><li><strong>ECI ↔ ECF</strong>：地球惯性系与地球固连系之间的转换</li><li><strong>J2000 ↔ ECF</strong>：J2000地球平赤道系与地球固连系之间的转换</li><li><strong>J2000 ↔ MOD</strong>：J2000地球平赤道系与平赤道系之间的转换</li><li><strong>MOD ↔ TOD</strong>：平赤道系与真赤道系之间的转换</li><li><strong>TOD ↔ GTOD</strong>：真赤道系与格林尼治真赤道系之间的转换</li><li><strong>GTOD ↔ ECF</strong>：格林尼治真赤道系与地球固连系之间的转换</li><li><strong>ICRF ↔ ECF</strong>：国际天球参考系与地球固连系之间的转换</li><li><strong>ICRF ↔ CIRF</strong>：国际天球参考系与天球中间参考系之间的转换</li><li><strong>CIRF ↔ TIRF</strong>：天球中间参考系与地球中间参考系之间的转换</li><li><strong>TIRF ↔ ECF</strong>：地球中间参考系与地球固连系之间的转换</li></ul><h2 id="用法示例" tabindex="-1"><a class="header-anchor" href="#用法示例"><span>用法示例</span></a></h2><h3 id="_1-j2000到ecf的坐标转换" tabindex="-1"><a class="header-anchor" href="#_1-j2000到ecf的坐标转换"><span>1. J2000到ECF的坐标转换</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/FrameTransform.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/TimePoint.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstMath/Vector.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstUtil/Literals.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/RunTime.hpp&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">AST_USING_NAMESPACE</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">using </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">namespace</span><span style="--shiki-light:#383A42;--shiki-dark:#E5C07B;"> _AST</span><span style="--shiki-light:#383A42;--shiki-dark:#E5C07B;"> literals</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">()</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">{</span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 加载基础数据并初始化，需要EOP数据</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aInitialize</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">();</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 创建时间点</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    TimePoint tp </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> TimePoint::</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">FromUTC</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2026</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // J2000坐标</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d vecJ2000{</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">3000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">};</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 转换到ECF坐标</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d vecECF;</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aJ2000ToECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(tp, vecJ2000, vecECF);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 输出结果</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;J2000坐标: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">]);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;ECF坐标: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">]);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 清理资源</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aUninitialize</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">();</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    return</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h3 id="_2-带速度的j2000到ecf转换" tabindex="-1"><a class="header-anchor" href="#_2-带速度的j2000到ecf转换"><span>2. 带速度的J2000到ECF转换</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/FrameTransform.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/TimePoint.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstMath/Vector.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstUtil/Literals.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/RunTime.hpp&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">AST_USING_NAMESPACE</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">using </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">namespace</span><span style="--shiki-light:#383A42;--shiki-dark:#E5C07B;"> _AST</span><span style="--shiki-light:#383A42;--shiki-dark:#E5C07B;"> literals</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">()</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">{</span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 加载基础数据并初始化，需要EOP数据</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aInitialize</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">();</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 创建时间点</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    TimePoint tp </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> TimePoint::</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">FromUTC</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2026</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // J2000坐标和速度</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d vecJ2000{</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">3000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">};</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d velJ2000{</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">100</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">/</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">s, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">200</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">/</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">s, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">300</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">/</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">s};</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 转换到ECF坐标和速度</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d vecECF;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d velECF;</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aJ2000ToECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(tp, vecJ2000, velJ2000, vecECF, velECF);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 输出结果</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;J2000坐标: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">]);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;J2000速度: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m/s, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m/s, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m/s</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">velJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">velJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">velJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">]);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;ECF坐标: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">]);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;ECF速度: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m/s, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m/s, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m/s</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">velECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">velECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">velECF</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">]);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 清理资源</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aUninitialize</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">();</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    return</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h3 id="_3-使用转换矩阵" tabindex="-1"><a class="header-anchor" href="#_3-使用转换矩阵"><span>3. 使用转换矩阵</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/FrameTransform.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/TimePoint.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstMath/Vector.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstMath/Matrix.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstUtil/Literals.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/RunTime.hpp&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">AST_USING_NAMESPACE</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">using </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">namespace</span><span style="--shiki-light:#383A42;--shiki-dark:#E5C07B;"> _AST</span><span style="--shiki-light:#383A42;--shiki-dark:#E5C07B;"> literals</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">()</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">{</span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 加载基础数据并初始化，需要EOP数据</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aInitialize</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">();</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 创建时间点</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    TimePoint tp </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> TimePoint::</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">FromUTC</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2026</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">7</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">12</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 获取J2000到MOD的转换矩阵</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Matrix3d matrix;</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aJ2000ToMODMatrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(tp, matrix);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // J2000坐标</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d vecJ2000{</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">3000</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">_km</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">};</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 使用矩阵转换坐标</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d vecMOD </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> matrix </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">*</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> vecJ2000;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 输出结果</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;J2000坐标: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecJ2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">]);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;转换矩阵:</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> %.9f</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> %.9f</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">), </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">), </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">));</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> %.9f</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> %.9f</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">), </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">), </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">));</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> %.9f</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> %.9f</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">), </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">), </span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">matrix</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">,</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">));</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;MOD坐标: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.3f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecMOD</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecMOD</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vecMOD</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">]);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 清理资源</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aUninitialize</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">();</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    return</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h2 id="依赖关系" tabindex="-1"><a class="header-anchor" href="#依赖关系"><span>依赖关系</span></a></h2><ul><li><code>AstGlobal.h</code>：项目全局定义</li><li><code>AstCore/TimePoint.hpp</code>：时间点定义</li><li><code>AstMath/Vector.hpp</code>：向量定义</li><li><code>AstMath/Matrix.hpp</code>：矩阵定义</li><li><code>AstMath/Rotation.hpp</code>：旋转定义</li><li><code>AstCore/SOFA.hpp</code>：天文算法库接口</li><li><code>AstCore/EOP.hpp</code>：地球定向参数接口</li></ul><h2 id="注意事项" tabindex="-1"><a class="header-anchor" href="#注意事项"><span>注意事项</span></a></h2><ul><li>所有角度参数默认使用弧度(rad)</li><li>转换精度取决于所使用的岁差、章动模型</li><li>极移校正需要EOP数据支持</li><li>部分转换函数提供了IAU1980和JPL DE两种实现，可通过<code>aNutationMethodSet</code>选择</li><li>长时间跨度的转换可能需要更精确的模型选择</li></ul><h2 id="api参考" tabindex="-1"><a class="header-anchor" href="#api参考"><span>API参考</span></a></h2><div class="language-hpp line-numbers-mode" data-highlighter="shiki" data-ext="hpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-hpp"><span class="line"><span>///</span></span>
<span class="line"><span>/// @file      FrameTransform.hpp</span></span>
<span class="line"><span>/// @brief     ~</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @author    axel</span></span>
<span class="line"><span>/// @date      2026-01-05</span></span>
<span class="line"><span>/// @copyright 版权所有 (C) 2026-present, ast项目.</span></span>
<span class="line"><span>///</span></span>
<span class="line"><span>/// ast项目（https://github.com/space-ast/ast）</span></span>
<span class="line"><span>/// 本项目基于 Apache 2.0 开源许可证分发。</span></span>
<span class="line"><span>/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。</span></span>
<span class="line"><span>/// 许可证全文请见：</span></span>
<span class="line"><span>/// </span></span>
<span class="line"><span>///    http://www.apache.org/licenses/LICENSE-2.0</span></span>
<span class="line"><span>/// </span></span>
<span class="line"><span>/// 重要须知：</span></span>
<span class="line"><span>/// 软件按&quot;现有状态&quot;提供，无任何明示或暗示的担保条件。</span></span>
<span class="line"><span>/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。</span></span>
<span class="line"><span>/// 使用本软件所产生的风险，需由您自行承担。</span></span>
<span class="line"><span></span></span>
<span class="line"><span>#pragma once</span></span>
<span class="line"><span></span></span>
<span class="line"><span>#include &quot;AstGlobal.h&quot;</span></span>
<span class="line"><span></span></span>
<span class="line"><span>AST_NAMESPACE_BEGIN</span></span>
<span class="line"><span></span></span>
<span class="line"><span>class TimePoint;</span></span>
<span class="line"><span>class Rotation;</span></span>
<span class="line"><span>class KinematicRotation;</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/*</span></span>
<span class="line"><span> * </span></span>
<span class="line"><span> * 坐标系定义：</span></span>
<span class="line"><span> * </span></span>
<span class="line"><span> * CBI(Central Body Inertial Frame): 中心天体惯性系</span></span>
<span class="line"><span> * CBF(Central Body Fixed Frame): 中心天体固连系</span></span>
<span class="line"><span> * ECI(Earth Centered Inertial Frame): 地球惯性系，根据相关的配置，ECI可能为ICRF或J2000</span></span>
<span class="line"><span> * J2000: 地球时(TT)2000年1月1日12时0分0秒的地球平赤道系</span></span>
<span class="line"><span> * MOD(Mean of Date): 平赤道系，考虑岁差</span></span>
<span class="line"><span> * TOD(True of Date): 真赤道系，考虑岁差和章动</span></span>
<span class="line"><span> * GTOD(Greenwich True of Date): 格林尼治真赤道系，XY平面为赤道平面，正X轴指向本初子午线，随地球旋转</span></span>
<span class="line"><span> *      参考NASA文档：https://science-data.larc.nasa.gov/LITE/level1doc/gtod.html</span></span>
<span class="line"><span> *      其又称为：</span></span>
<span class="line"><span> *      - TDR(True of Date Rotating frame) 旋转真赤道系，参考orekit文档: https://www.orekit.org/site-orekit-12.0/apidocs/org/orekit/frames/GTODProvider.html</span></span>
<span class="line"><span> *      - GCR(Greenwich Rotating Coordinate frame) 格林尼治旋转系，参考orekit文档</span></span>
<span class="line"><span> *      - PEF(Pseudo Earth-fixed) 伪地球固连系，参考 Kayhan Space: https://app.kayhan.io/docs/data-standards/reference-frames/</span></span>
<span class="line"><span> *      - EFG(Earth-Fixed Greenwich) 地球固连格林尼治系，参考 Kayhan Space</span></span>
<span class="line"><span> *      </span></span>
<span class="line"><span> * ECF(Earth Centered Fixed Frame): 地球固连系，在GTOD基础上考虑了极移</span></span>
<span class="line"><span> * </span></span>
<span class="line"><span> * ICRF(International Celestial Reference Frame): 国际天球参考系</span></span>
<span class="line"><span> * CIRF(Celestial Intermediate Reference Frame): 天球中间参考系</span></span>
<span class="line"><span> *      天球中间参考系的第一极（z轴方向）为给定时刻的天球中间极（CIP），</span></span>
<span class="line"><span> *      与之等价的基本平面为与该时刻的天球中间极垂直的中间赤道，经度坐标起量点为天球中间零点。</span></span>
<span class="line"><span> *      它类似于传统说法中的真赤道和真春分点参考系，两者基本平面完全相同，</span></span>
<span class="line"><span> *      不同之处在于天球中间参考系的赤经零点为天球中间零点，而真赤道参考系的赤经零点为春分点。</span></span>
<span class="line"><span> *      两个参考系的转换为绕着天球中间极旋转一个角度EO，称为零点差，它的表达式为EO=ERA-GAST，即地球自转角与真恒星时之差。</span></span>
<span class="line"><span> *      由此可以看出，真赤道真春分点参考系可以认为基于春分点的转换中的天球中间参考系。</span></span>
<span class="line"><span> *      由于天球中间参考系的首字母缩略语与国际天球参考系（ICRF）非常接近，因此，在提及该参考系时建议尽量使用全名</span></span>
<span class="line"><span> * TIRF(Terrestrial Intermediate Reference Frame): 地球中间参考系</span></span>
<span class="line"><span> *      TIRF与国际地球参考系（ITRF）的联系由极移和地球中间零点的定位角确定。</span></span>
<span class="line"><span> *      它通过绕天球中间极CIP的转动角度即地球自转角（ERA）与天球中间参考系（CIRS）相联系。</span></span>
<span class="line"><span> *      天球中间极是这两个中间参考系TIRS和CIRS共同的Z轴。</span></span>
<span class="line"><span> *      因为地球中间参考系的首字母缩写词与国际地球参考系ITRF接近，建议使用全名。</span></span>
<span class="line"><span> */</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span>// ECI -&gt; ECF</span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ECI转换为ECF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_API void aECIToECFTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ECI转换为ECF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换，带角速度信息</span></span>
<span class="line"><span>AST_CORE_API void aECIToECFTransform(const TimePoint&amp; tp, KinematicRotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ECI转换为ECF的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_CAPI void aECIToECFMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ECI转换为ECF的坐标转换</span></span>
<span class="line"><span>/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecECI  ECI坐标</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>AST_CORE_API void aECIToECF(const TimePoint&amp; tp, const Vector3d&amp; vecECI, Vector3d&amp; vecECF);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ECI转换为ECF的坐标转换</span></span>
<span class="line"><span>/// @details   ECI为地球惯性系，根据相关的配置，ECI可能为J2000或ICRF。</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecECI  ECI坐标</span></span>
<span class="line"><span>/// @param     velECI  ECI速度</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>/// @param     velECF  ECF速度</span></span>
<span class="line"><span>AST_CORE_API void aECIToECF(</span></span>
<span class="line"><span>    const TimePoint&amp; tp,</span></span>
<span class="line"><span>    const Vector3d&amp; vecECI,</span></span>
<span class="line"><span>    const Vector3d&amp; velECI,</span></span>
<span class="line"><span>    Vector3d&amp; vecECF,</span></span>
<span class="line"><span>    Vector3d&amp; velECF</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span>// J2000 -&gt; ECF</span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从J2000转换为ECF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_API void aJ2000ToECFTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从J2000转换为ECF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换，带角速度信息</span></span>
<span class="line"><span>AST_CORE_API void aJ2000ToECFTransform(const TimePoint&amp; tp, KinematicRotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从J2000转换为ECF的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_API void aJ2000ToECFMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从J2000转换为ECF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecJ2000  J2000坐标</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>AST_CORE_API void aJ2000ToECF(const TimePoint&amp; tp, const Vector3d&amp; vecJ2000, Vector3d&amp; vecECF);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从J2000转换为ECF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecJ2000  J2000坐标</span></span>
<span class="line"><span>/// @param     velJ2000  J2000速度</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>/// @param     velECF  ECF速度</span></span>
<span class="line"><span>AST_CORE_API void aJ2000ToECF(</span></span>
<span class="line"><span>    const TimePoint&amp; tp,</span></span>
<span class="line"><span>    const Vector3d&amp; vecJ2000,</span></span>
<span class="line"><span>    const Vector3d&amp; velJ2000,</span></span>
<span class="line"><span>    Vector3d&amp; vecECF,</span></span>
<span class="line"><span>    Vector3d&amp; velECF</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span>// J2000 -&gt; MOD</span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从J2000转换为MOD的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_CAPI void aJ2000ToMODTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从J2000转换为MOD的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_CAPI void aJ2000ToMODMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从J2000转换为MOD的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecJ2000  J2000坐标</span></span>
<span class="line"><span>/// @param     vecMOD  MOD坐标</span></span>
<span class="line"><span>AST_CORE_CAPI void aJ2000ToMOD(const TimePoint&amp; tp, const Vector3d&amp; vecJ2000, Vector3d&amp; vecMOD); </span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span>// MOD -&gt; TOD</span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从MOD转换为TOD的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_CAPI void aMODToTODTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从MOD转换为TOD的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_CAPI void aMODToTODMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从MOD转换为TOD的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecMOD  MOD坐标</span></span>
<span class="line"><span>/// @param     vecTOD  TOD坐标</span></span>
<span class="line"><span>AST_CORE_CAPI void aMODToTOD(const TimePoint&amp; tp, const Vector3d&amp; vecMOD, Vector3d&amp; vecTOD);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span>// TOD -&gt; GTOD</span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从TOD转换为GTOD的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_API void aTODToGTODTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从TOD转换为GTOD的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换，带角速度信息</span></span>
<span class="line"><span>AST_CORE_API void aTODToGTODTransform(const TimePoint&amp; tp, KinematicRotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从TOD转换为GTOD的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_API void aTODToGTODMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从TOD转换为GTOD的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecTOD  TOD坐标</span></span>
<span class="line"><span>/// @param     vecGTOD  GTOD坐标</span></span>
<span class="line"><span>AST_CORE_API void aTODToGTOD(const TimePoint&amp; tp, const Vector3d&amp; vecTOD, Vector3d&amp; vecGTOD);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从TOD转换为GTOD的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecTOD  TOD坐标</span></span>
<span class="line"><span>/// @param     velTOD  TOD速度</span></span>
<span class="line"><span>/// @param     vecGTOD  GTOD坐标</span></span>
<span class="line"><span>/// @param     velGTOD  GTOD速度</span></span>
<span class="line"><span>AST_CORE_API void aTODToGTOD(</span></span>
<span class="line"><span>    const TimePoint&amp; tp, </span></span>
<span class="line"><span>    const Vector3d&amp; vecTOD, const Vector3d&amp; velTOD,</span></span>
<span class="line"><span>    Vector3d&amp; vecGTOC, Vector3d&amp; velGTOC</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span>// GTOD -&gt; ECF</span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从GTOD转换为ECF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_CAPI void aGTODToECFTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从GTOD转换为ECF的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_CAPI void aGTODToECFMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从GTOD转换为ECF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecGTOD  GTOD坐标</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>AST_CORE_CAPI void aGTODToECF(const TimePoint&amp; tp, const Vector3d&amp; vecGTOD, Vector3d&amp; vecECF);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span>// ICRF -&gt; ECF</span></span>
<span class="line"><span>//----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ICRF转换为ECF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_API void aICRFToECFTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ICRF转换为ECF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换，带角速度信息</span></span>
<span class="line"><span>AST_CORE_API void aICRFToECFTransform(const TimePoint&amp; tp, KinematicRotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ICRF转换为ECF的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_CAPI void aICRFToECFMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ICRF转换为ECF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecICRF  ICRF坐标</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>AST_CORE_API void aICRFToECF(const TimePoint&amp; tp, const Vector3d&amp; vecICRF, Vector3d&amp; vecECF);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ECF转换为ICRF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>/// @param     vecICRF  ICRF坐标</span></span>
<span class="line"><span>AST_CORE_API void aECFToICRF(const TimePoint&amp; tp, const Vector3d&amp; vecECF, Vector3d&amp; vecICRF);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ICRF转换为ECF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecICRF  ICRF坐标</span></span>
<span class="line"><span>/// @param     velICRF  ICRF速度</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>/// @param     velECF  ECF速度</span></span>
<span class="line"><span>AST_CORE_API void aICRFToECF(</span></span>
<span class="line"><span>    const TimePoint&amp; tp, </span></span>
<span class="line"><span>    const Vector3d&amp; vecICRF, const Vector3d&amp; velICRF, </span></span>
<span class="line"><span>    Vector3d&amp; vecECF, Vector3d&amp; velECF</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>// ----------------</span></span>
<span class="line"><span>// ICRF -&gt; CIRF</span></span>
<span class="line"><span>// ----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ICRF转换为CIRF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_CAPI void aICRFToCIRFTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ICRF转换为CIRF的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_CAPI void aICRFToCIRFMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从ICRF转换为CIRF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecICRF  ICRF坐标</span></span>
<span class="line"><span>/// @param     vecCIRF  CIRF坐标</span></span>
<span class="line"><span>AST_CORE_CAPI void aICRFToCIRF(const TimePoint&amp; tp, const Vector3d&amp; vecICRF, Vector3d&amp; vecCIRF);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>// ----------------</span></span>
<span class="line"><span>// CIRF -&gt; TIRF</span></span>
<span class="line"><span>// ----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从CIRF转换为TIRF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_API void aCIRFToTIRFTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从CIRF转换为TIRF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换，带角速度信息</span></span>
<span class="line"><span>AST_CORE_API void aCIRFToTIRFTransform(const TimePoint&amp; tp, KinematicRotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从CIRF转换为TIRF的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_CAPI void aCIRFToTIRFMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从CIRF转换为TIRF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecCIRF  CIRF坐标</span></span>
<span class="line"><span>/// @param     vecTIRF  TIRF坐标</span></span>
<span class="line"><span>AST_CORE_API void aCIRFToTIRF(const TimePoint&amp; tp, const Vector3d&amp; vecCIRF, Vector3d&amp; vecTIRF);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从CIRF转换为TIRF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecCIRF  CIRF坐标</span></span>
<span class="line"><span>/// @param     velCIRF  CIRF速度</span></span>
<span class="line"><span>/// @param     vecTIRF  TIRF坐标</span></span>
<span class="line"><span>/// @param     velTIRF  TIRF速度</span></span>
<span class="line"><span>AST_CORE_API void aCIRFToTIRF(</span></span>
<span class="line"><span>    const TimePoint&amp; tp, </span></span>
<span class="line"><span>     const Vector3d&amp; vecCIRF, const Vector3d&amp; velCIRF,</span></span>
<span class="line"><span>     Vector3d&amp; vecTIRF, Vector3d&amp; velTIRF</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>// ----------------</span></span>
<span class="line"><span>// TIRF -&gt; ECF</span></span>
<span class="line"><span>// ----------------</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从TIRF转换为ECF的坐标旋转变换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     rotation  坐标旋转变换</span></span>
<span class="line"><span>AST_CORE_CAPI void aTIRFToECFTransform(const TimePoint&amp; tp, Rotation&amp; rotation);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从TIRF转换为ECF的坐标转换矩阵</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     matrix  坐标转换矩阵</span></span>
<span class="line"><span>AST_CORE_CAPI void aTIRFToECFMatrix(const TimePoint&amp; tp, Matrix3d&amp; matrix);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief     从TIRF转换为ECF的坐标转换</span></span>
<span class="line"><span>/// @details   ~</span></span>
<span class="line"><span>/// @param     tp  时间点</span></span>
<span class="line"><span>/// @param     vecTIRF  TIRF坐标</span></span>
<span class="line"><span>/// @param     vecECF  ECF坐标</span></span>
<span class="line"><span>AST_CORE_CAPI void aTIRFToECF(const TimePoint&amp; tp, const Vector3d&amp; vecTIRF, Vector3d&amp; vecECF);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>AST_NAMESPACE_END</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div>`,28)])])}const r=n(p,[["render",e]]),k=JSON.parse('{"path":"/core/frame-transform.html","title":"坐标转换模块","lang":"zh-CN","frontmatter":{"description":"坐标转换模块 说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。 模块概述 坐标变换模块提供了天体力学和航天动力学中常用的各种坐标系之间的转换功能。该模块支持多种参考系之间的精确转换，包括惯性系、固连系以及各种中间参考系，为轨道计算、姿态控制等应用提供基础支持。 核心概念 坐标系定义 模块支持以下主要坐标系： CBI (Central B...","head":[["script",{"type":"application/ld+json"},"{\\"@context\\":\\"https://schema.org\\",\\"@type\\":\\"Article\\",\\"headline\\":\\"坐标转换模块\\",\\"image\\":[\\"\\"],\\"dateModified\\":\\"2026-01-14T02:33:07.000Z\\",\\"author\\":[{\\"@type\\":\\"Person\\",\\"name\\":\\"space-ast\\",\\"url\\":\\"https://github.com/space-ast\\"}]}"],["meta",{"property":"og:url","content":"https://github.com/space-ast/ast/ast/core/frame-transform.html"}],["meta",{"property":"og:site_name","content":"ast文档"}],["meta",{"property":"og:title","content":"坐标转换模块"}],["meta",{"property":"og:description","content":"坐标转换模块 说明：本文档旨在帮助理解模块功能，具体API和行为请查阅源代码。 模块概述 坐标变换模块提供了天体力学和航天动力学中常用的各种坐标系之间的转换功能。该模块支持多种参考系之间的精确转换，包括惯性系、固连系以及各种中间参考系，为轨道计算、姿态控制等应用提供基础支持。 核心概念 坐标系定义 模块支持以下主要坐标系： CBI (Central B..."}],["meta",{"property":"og:type","content":"article"}],["meta",{"property":"og:locale","content":"zh-CN"}],["meta",{"property":"og:locale:alternate","content":"en-US"}],["meta",{"property":"og:updated_time","content":"2026-01-14T02:33:07.000Z"}],["meta",{"property":"article:modified_time","content":"2026-01-14T02:33:07.000Z"}],["link",{"rel":"alternate","hreflang":"en-us","href":"https://github.com/space-ast/ast/ast/en/core/frame-transform.html"}]]},"git":{"createdTime":1768357987000,"updatedTime":1768357987000,"contributors":[{"name":"axel","username":"axel","email":"space_ast@163.com","commits":1,"url":"https://github.com/axel"}]},"readingTime":{"minutes":3.12,"words":937},"filePathRelative":"core/frame-transform.md","autoDesc":true}');export{r as comp,k as data};
