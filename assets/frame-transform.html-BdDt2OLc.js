import{_ as n}from"./plugin-vue_export-helper-DlAUqK2U.js";import{c as a,e as i,o as l}from"./app-CrI_Hpyo.js";const p={};function e(t,s){return l(),a("div",null,[...s[0]||(s[0]=[i(`<h1 id="frame-transform-module" tabindex="-1"><a class="header-anchor" href="#frame-transform-module"><span>Frame Transform Module</span></a></h1><p><em>Note: This document is AI-translated. For specific APIs and behaviors, please refer to the source code.</em></p><h2 id="module-overview" tabindex="-1"><a class="header-anchor" href="#module-overview"><span>Module Overview</span></a></h2><p>The Frame Transform module provides conversion functionality between various coordinate systems commonly used in astrodynamics and aerospace dynamics. This module supports precise transformations between inertial frames, fixed frames, and various intermediate reference frames, providing foundational support for orbit calculations, attitude control, and other applications.</p><h2 id="core-concepts" tabindex="-1"><a class="header-anchor" href="#core-concepts"><span>Core Concepts</span></a></h2><h3 id="coordinate-system-definitions" tabindex="-1"><a class="header-anchor" href="#coordinate-system-definitions"><span>Coordinate System Definitions</span></a></h3><p>The module supports the following main coordinate systems:</p><ul><li><strong>CBI (Central Body Inertial Frame)</strong>：Central body inertial frame</li><li><strong>CBF (Central Body Fixed Frame)</strong>：Central body fixed frame</li><li><strong>ECI (Earth Centered Inertial Frame)</strong>：Earth-centered inertial frame, which can represent ICRF or J2000 depending on configuration</li><li><strong>J2000</strong>：Earth mean equatorial frame at TT 2000-01-01 12:00:00</li><li><strong>MOD (Mean of Date)</strong>：Mean equatorial frame, considering precession</li><li><strong>TOD (True of Date)</strong>：True equatorial frame, considering precession and nutation</li><li><strong>GTOD (Greenwich True of Date)</strong>：Greenwich true equatorial frame, with XY plane as equatorial plane, positive X-axis pointing to prime meridian, rotating with Earth <ul><li>Also known as TDR, GCR, PEF, or EFG</li></ul></li><li><strong>ECF (Earth Centered Fixed Frame)</strong>：Earth-centered fixed frame, considering polar motion on top of GTOD</li><li><strong>ICRF (International Celestial Reference Frame)</strong>：International Celestial Reference Frame</li><li><strong>CIRF (Celestial Intermediate Reference Frame)</strong>：Celestial Intermediate Reference Frame <ul><li>The first pole (z-axis direction) is the Celestial Intermediate Pole (CIP) at a given time</li><li>The fundamental plane is the intermediate equator perpendicular to the CIP at that time</li><li>Longitude coordinate starts from the Celestial Intermediate Origin (CIO)</li></ul></li><li><strong>TIRF (Terrestrial Intermediate Reference Frame)</strong>：Terrestrial Intermediate Reference Frame <ul><li>Linked to the International Terrestrial Reference Frame (ITRF) by polar motion and the positioning angle of the terrestrial intermediate origin</li><li>Connected to CIRF by rotating around the CIP by the Earth Rotation Angle (ERA)</li></ul></li></ul><h3 id="transform-types" tabindex="-1"><a class="header-anchor" href="#transform-types"><span>Transform Types</span></a></h3><p>The module provides three main types of coordinate transformation interfaces:</p><ol><li><strong>Transform</strong>：Returns Rotation or KinematicRotation objects</li><li><strong>Matrix</strong>：Returns coordinate transformation matrices</li><li><strong>Direct Transformation</strong>：Directly converts coordinates from one reference frame to another</li></ol><p>Some transformations also support velocity conversion, considering the angular velocity effects of rotating coordinate systems.</p><h2 id="main-functions" tabindex="-1"><a class="header-anchor" href="#main-functions"><span>Main Functions</span></a></h2><p>The module supports transformations between the following coordinate systems:</p><ul><li><strong>ECI ↔ ECF</strong>：Earth-centered inertial frame and Earth-centered fixed frame</li><li><strong>J2000 ↔ ECF</strong>：J2000 inertial frame and Earth-centered fixed frame</li><li><strong>J2000 ↔ MOD</strong>：J2000 inertial frame and mean of date frame</li><li><strong>MOD ↔ TOD</strong>：Mean of date frame and true of date frame</li><li><strong>TOD ↔ GTOD</strong>：True of date frame and Greenwich true of date frame</li><li><strong>GTOD ↔ ECF</strong>：Greenwich true of date frame and Earth-centered fixed frame</li><li><strong>ICRF ↔ ECF</strong>：International Celestial Reference Frame and Earth-centered fixed frame</li><li><strong>ICRF ↔ CIRF</strong>：International Celestial Reference Frame and Celestial Intermediate Reference Frame</li><li><strong>CIRF ↔ TIRF</strong>：Celestial Intermediate Reference Frame and Terrestrial Intermediate Reference Frame</li><li><strong>TIRF ↔ ECF</strong>：Terrestrial Intermediate Reference Frame and Earth-centered fixed frame</li></ul><h2 id="usage-examples" tabindex="-1"><a class="header-anchor" href="#usage-examples"><span>Usage Examples</span></a></h2><h3 id="_1-j2000-to-ecf-coordinate-conversion" tabindex="-1"><a class="header-anchor" href="#_1-j2000-to-ecf-coordinate-conversion"><span>1. J2000 to ECF Coordinate Conversion</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/FrameTransform.hpp&quot;</span></span>
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
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h3 id="_2-j2000-to-ecf-conversion-with-velocity" tabindex="-1"><a class="header-anchor" href="#_2-j2000-to-ecf-conversion-with-velocity"><span>2. J2000 to ECF Conversion with Velocity</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/FrameTransform.hpp&quot;</span></span>
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
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h3 id="_3-using-transformation-matrix" tabindex="-1"><a class="header-anchor" href="#_3-using-transformation-matrix"><span>3. Using Transformation Matrix</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/FrameTransform.hpp&quot;</span></span>
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
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h2 id="dependencies" tabindex="-1"><a class="header-anchor" href="#dependencies"><span>Dependencies</span></a></h2><ul><li><code>AstGlobal.h</code>：Project global definitions</li><li><code>AstCore/TimePoint.hpp</code>：Time point definition</li><li><code>AstMath/Vector.hpp</code>：Vector definition</li><li><code>AstMath/Matrix.hpp</code>：Matrix definition</li><li><code>AstMath/Rotation.hpp</code>：Rotation definition</li><li><code>AstCore/SOFA.hpp</code>：Astronomical algorithm library interface</li><li><code>AstCore/EOP.hpp</code>：Earth Orientation Parameters interface</li></ul><h2 id="notes" tabindex="-1"><a class="header-anchor" href="#notes"><span>Notes</span></a></h2><ul><li>All angle parameters default to radians (rad)</li><li>Transformation accuracy depends on the precession and nutation models used</li><li>Polar motion correction requires EOP data support</li><li>Some transformation functions provide both IAU1980 and JPL DE implementations, which can be selected via <code>aNutationMethodSet</code></li><li>More precise model selection may be required for long time span transformations</li></ul><h2 id="api-reference" tabindex="-1"><a class="header-anchor" href="#api-reference"><span>API Reference</span></a></h2><div class="language-hpp line-numbers-mode" data-highlighter="shiki" data-ext="hpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-hpp"><span class="line"><span>///</span></span>
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
<span class="line"><span>AST_NAMESPACE_END</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div>`,28)])])}const h=n(p,[["render",e]]),c=JSON.parse('{"path":"/en/core/frame-transform.html","title":"Frame Transform Module","lang":"en-US","frontmatter":{"description":"Frame Transform Module Note: This document is AI-translated. For specific APIs and behaviors, please refer to the source code. Module Overview The Frame Transform module provide...","head":[["script",{"type":"application/ld+json"},"{\\"@context\\":\\"https://schema.org\\",\\"@type\\":\\"Article\\",\\"headline\\":\\"Frame Transform Module\\",\\"image\\":[\\"\\"],\\"dateModified\\":\\"2026-01-21T11:03:44.000Z\\",\\"author\\":[{\\"@type\\":\\"Person\\",\\"name\\":\\"space-ast\\",\\"url\\":\\"https://github.com/space-ast\\"}]}"],["meta",{"property":"og:url","content":"https://github.com/space-ast/ast/ast/en/core/frame-transform.html"}],["meta",{"property":"og:site_name","content":"ast docs"}],["meta",{"property":"og:title","content":"Frame Transform Module"}],["meta",{"property":"og:description","content":"Frame Transform Module Note: This document is AI-translated. For specific APIs and behaviors, please refer to the source code. Module Overview The Frame Transform module provide..."}],["meta",{"property":"og:type","content":"article"}],["meta",{"property":"og:locale","content":"en-US"}],["meta",{"property":"og:locale:alternate","content":"zh-CN"}],["meta",{"property":"og:updated_time","content":"2026-01-21T11:03:44.000Z"}],["meta",{"property":"article:modified_time","content":"2026-01-21T11:03:44.000Z"}],["link",{"rel":"alternate","hreflang":"zh-cn","href":"https://github.com/space-ast/ast/ast/core/frame-transform.html"}]]},"git":{"createdTime":1768357987000,"updatedTime":1768993424000,"contributors":[{"name":"axel","username":"axel","email":"space_ast@163.com","commits":2,"url":"https://github.com/axel"}]},"readingTime":{"minutes":1.95,"words":584},"filePathRelative":"en/core/frame-transform.md","autoDesc":true}');export{h as comp,c as data};
