import{_ as n}from"./plugin-vue_export-helper-DlAUqK2U.js";import{c as a,e as i,o as l}from"./app-C6lktLSC.js";const e={};function p(d,s){return l(),a("div",null,[...s[0]||(s[0]=[i(`<h1 id="orbit-elements-definition-and-conversion" tabindex="-1"><a class="header-anchor" href="#orbit-elements-definition-and-conversion"><span>Orbit Elements Definition and Conversion</span></a></h1><p><em>Note: This document is AI-translated. Please refer to the source code for specific APIs and behaviors.</em></p><h2 id="module-overview" tabindex="-1"><a class="header-anchor" href="#module-overview"><span>Module Overview</span></a></h2><p>The OrbitElement module provides conversion functions between various orbit elements (Cartesian coordinates, classical orbit elements, modified orbit elements, equinoctial elements, and modified equinoctial elements). It is a fundamental component for astrodynamics and space dynamics calculations.</p><h2 id="core-concepts" tabindex="-1"><a class="header-anchor" href="#core-concepts"><span>Core Concepts</span></a></h2><h3 id="types-of-orbit-elements" tabindex="-1"><a class="header-anchor" href="#types-of-orbit-elements"><span>Types of Orbit Elements</span></a></h3><p>The module defines the following main types of orbit elements:</p><ol><li><p><strong>Cartesian Coordinates (CartState)</strong></p><ul><li>Contains position vector and velocity vector</li><li>Intuitive representation of spatial position and motion state</li></ul></li><li><p><strong>Classical Orbit Elements (OrbElem)</strong></p><ul><li>Semi-major axis (a)</li><li>Eccentricity (e)</li><li>Inclination (i)</li><li>Right ascension of ascending node (raan)</li><li>Argument of perigee (argper)</li><li>True anomaly (trueA)</li></ul></li><li><p><strong>Modified Orbit Elements (ModOrbElem)</strong></p><ul><li>Perigee radius (rp)</li><li>Eccentricity (e)</li><li>Inclination (i)</li><li>Right ascension of ascending node (raan)</li><li>Argument of perigee (argper)</li><li>True anomaly (trueA)</li></ul></li><li><p><strong>Equinoctial Elements (EquinElem)</strong></p><ul><li>Semi-major axis (a)</li><li>h = e*sin(argper + raan)</li><li>k = e*cos(argper + raan)</li><li>p = tan(i/2)*sin(raan)</li><li>q = tan(i/2)*cos(raan)</li><li>lambda = mean longitude = M + raan + argper</li></ul></li><li><p><strong>Modified Equinoctial Elements (ModEquinElem)</strong></p><ul><li>Semi-latus rectum (p = a(1-e²))</li><li>f = e*cos(argper+raan)</li><li>g = e*sin(argper+raan)</li><li>h = tan(i/2)cos(raan)</li><li>k = tan(i/2)sin(raan)</li><li>L = raan + argper + trueA</li></ul></li></ol><h3 id="key-constants-and-units" tabindex="-1"><a class="header-anchor" href="#key-constants-and-units"><span>Key Constants and Units</span></a></h3><ul><li>Position unit: meter (m)</li><li>Velocity unit: meter/second (m/s)</li><li>Angle unit: radian (rad)</li><li>Gravitational parameter unit: cubic meter per second squared (m³/s²)</li></ul><h2 id="usage-examples" tabindex="-1"><a class="header-anchor" href="#usage-examples"><span>Usage Examples</span></a></h2><h3 id="_1-conversion-between-classical-orbit-elements-and-cartesian-coordinates" tabindex="-1"><a class="header-anchor" href="#_1-conversion-between-classical-orbit-elements-and-cartesian-coordinates"><span>1. Conversion Between Classical Orbit Elements and Cartesian Coordinates</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/OrbitElement.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &lt;iostream&gt;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() {</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    AST_USING_NAMESPACE </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 地球引力参数 (m^3/s^2)</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> gmEarth </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 3.986004418</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">e</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">14</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 定义经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">6</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> {</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">7000000.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.01</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">};</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 转换为直角坐标</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> pos</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">3</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">], </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">vel</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">3</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">];</span></span>
<span class="line"><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">    err_t</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> err </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> coe2rv</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(coe, gmEarth, pos, vel);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    if</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> (err </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">==</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> eNoError) {</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;位置向量: [&quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> pos</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;, &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> pos</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;, &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> pos</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;]</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;速度向量: [&quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> vel</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;, &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> vel</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;, &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> vel</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;]</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">        // 从直角坐标转回经典轨道根数</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">        double</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> coeBack</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">6</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">];</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        err </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> rv2coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(pos, vel, gmEarth, coeBack);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">        if</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> (err </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">==</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> eNoError) {</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">            std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;长半轴: &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> coeBack</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot; m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">            std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;偏心率: &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> coeBack</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> std::endl;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        }</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    }</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h3 id="_2-using-class-reference-version-functions" tabindex="-1"><a class="header-anchor" href="#_2-using-class-reference-version-functions"><span>2. Using Class Reference Version Functions</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/OrbitElement.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstMath/Vector.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &lt;iostream&gt;</span></span>
<span class="line"></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() {</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    AST_USING_NAMESPACE </span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 地球引力参数 (m^3/s^2)</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> gmEarth </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 3.986004418</span><span style="--shiki-light:#986801;--shiki-dark:#E06C75;">e</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">14</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 定义位置和速度向量</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d pos{</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">6778137.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">};</span><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">  // 近地轨道位置</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    Vector3d vel{</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">7726.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">};</span><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">      // 近地轨道速度</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 转换为经典轨道根数</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    OrbElem elem;</span></span>
<span class="line"><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">    err_t</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> err </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> aCartToOrbElem</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(pos, vel, gmEarth, elem);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    if</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> (err </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">==</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> eNoError) {</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;长半轴: &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> elem</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">a</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot; m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;偏心率: &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> elem</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">e</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;轨道倾角: &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> elem</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">i</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot; rad</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">        // 从经典轨道根数转回直角坐标</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        Vector3d posBack, velBack;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        err </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> aOrbElemToCart</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(elem, gmEarth, posBack, velBack);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">        if</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> (err </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">==</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> eNoError) {</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">            std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;位置向量: [&quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> posBack</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;, &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> posBack</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;, &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> posBack</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">[</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">] </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;]</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">        }</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    }</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h3 id="_3-conversion-between-different-orbit-element-types" tabindex="-1"><a class="header-anchor" href="#_3-conversion-between-different-orbit-element-types"><span>3. Conversion Between Different Orbit Element Types</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/OrbitElement.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &lt;iostream&gt;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">using</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> namespace</span><span style="--shiki-light:#383A42;--shiki-dark:#E5C07B;"> AST_NAMESPACE</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() {</span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 定义经典轨道根数</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    OrbElem coe;</span></span>
<span class="line"><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">    coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">a</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 7000000.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">  // 7000 km</span></span>
<span class="line"><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">    coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">e</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0.1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">    coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">i</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">        // 0 radians</span></span>
<span class="line"><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">    coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">raan</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">    coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">argper</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">    coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">trueA</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0.0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 转换为春分点根数</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    EquinElem ee;</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    coe2ee</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">data</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(), </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">ee</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">data</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">());</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 转换为修正轨道根数</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    ModOrbElem moe;</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    coe2moe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">data</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(), </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">moe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">data</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">());</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 转换为改进春分点轨道根数</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    ModEquinElem mee;</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    coe2mee</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">coe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">data</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(), </span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;">mee</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">data</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">());</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;转换完成</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;春分点根数长半轴: &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> ee</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">a</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot; m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    std::cout </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;修正轨道根数近拱点半径: &quot;</span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;"> &lt;&lt;</span><span style="--shiki-light:#E45649;--shiki-dark:#E5C07B;"> moe</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">.</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">rp</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">() </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">&lt;&lt;</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot; m</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h2 id="dependencies" tabindex="-1"><a class="header-anchor" href="#dependencies"><span>Dependencies</span></a></h2><ul><li><code>AstGlobal.h</code>: Project global definitions</li><li><code>AstCore/Vector.hpp</code>: Vector class support</li></ul><h2 id="notes" tabindex="-1"><a class="header-anchor" href="#notes"><span>Notes</span></a></h2><ul><li>All angle parameters use radians</li><li>Distance parameters default to meters (m)</li><li>Time parameters default to seconds (s)</li><li>Gravitational parameter unit is m³/s²</li><li>Some conversion functions return error codes, please handle error cases appropriately</li><li>Zero eccentricity orbits (circular orbits) may require special handling in some conversions</li><li>Modified equinoctial elements may have singularity issues when the orbit inclination is close to 180 degrees</li></ul><h2 id="api-reference" tabindex="-1"><a class="header-anchor" href="#api-reference"><span>API Reference</span></a></h2><div class="language-hpp line-numbers-mode" data-highlighter="shiki" data-ext="hpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-hpp"><span class="line"><span>///</span></span>
<span class="line"><span>/// @file      OrbitElement.hpp</span></span>
<span class="line"><span>/// @brief     轨道根数定义及转换接口</span></span>
<span class="line"><span>/// @details   提供各种轨道根数（直角坐标、经典轨道根数、修正轨道根数、春分点根数、改进春分点轨道根数）之间的转换函数。</span></span>
<span class="line"><span>/// @author    axel</span></span>
<span class="line"><span>/// @date      19.11.2025</span></span>
<span class="line"><span>/// @copyright 版权所有 (C) 2025-present, ast项目.</span></span>
<span class="line"><span>///</span></span>
<span class="line"><span>/// ast项目（https://github.com/space-ast/ast）</span></span>
<span class="line"><span>/// 本项目基于 Apache 2.0 开源许可证分发。</span></span>
<span class="line"><span>/// 您可在遵守许可证条款的前提下使用、修改和分发本软件。</span></span>
<span class="line"><span>/// 许可证全文请见：</span></span>
<span class="line"><span>/// </span></span>
<span class="line"><span>///    http://www.apache.org/licenses/LICENSE-2.0</span></span>
<span class="line"><span>/// </span></span>
<span class="line"><span>/// 重要须知：</span></span>
<span class="line"><span>/// 软件按“现有状态”提供，无任何明示或暗示的担保条件。</span></span>
<span class="line"><span>/// 除非法律要求或书面同意，作者与贡献者不承担任何责任。</span></span>
<span class="line"><span>/// 使用本软件所产生的风险，需由您自行承担。</span></span>
<span class="line"><span> </span></span>
<span class="line"><span>#pragma once</span></span>
<span class="line"><span> </span></span>
<span class="line"><span>#include &quot;AstGlobal.h&quot;</span></span>
<span class="line"><span>#include &quot;AstCore/Vector.hpp&quot;</span></span>
<span class="line"><span>#include &lt;string&gt;</span></span>
<span class="line"><span> </span></span>
<span class="line"><span>AST_NAMESPACE_BEGIN</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标</span></span>
<span class="line"><span>class CartState</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    Vector3d pos_;   ///&lt; 位置</span></span>
<span class="line"><span>    Vector3d vel_;   ///&lt; 速度</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    A_DEF_POD_ITERABLE(double)</span></span>
<span class="line"><span>    const Vector3d&amp; pos() const {return pos_;} </span></span>
<span class="line"><span>    Vector3d&amp; pos() {return pos_;}</span></span>
<span class="line"><span>    const Vector3d&amp; vel() const {return vel_;} </span></span>
<span class="line"><span>    Vector3d&amp; vel() {return vel_;}</span></span>
<span class="line"><span>};</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数</span></span>
<span class="line"><span>class OrbElem</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    double	 a_;          ///&lt; 长半轴 </span></span>
<span class="line"><span>    double	 e_;          ///&lt; 偏心率</span></span>
<span class="line"><span>    double	 i_;          ///&lt; 轨道倾角 </span></span>
<span class="line"><span>    double	 raan_;       ///&lt; 升交点赤经 </span></span>
<span class="line"><span>    double	 argper_;     ///&lt; 近拱点角</span></span>
<span class="line"><span>    double	 trueA_;      ///&lt; 真近点角</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    A_DEF_POD_ITERABLE(double)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, a)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, e)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, i)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, raan)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, argper)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, trueA)</span></span>
<span class="line"><span>};</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 修正轨道根数</span></span>
<span class="line"><span>class ModOrbElem</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    double rp_;         ///&lt;近拱点半径 </span></span>
<span class="line"><span>    double e_;          ///&lt;偏心率 </span></span>
<span class="line"><span>    double i_;          ///&lt;轨道倾角</span></span>
<span class="line"><span>    double raan_;       ///&lt;升交点赤经</span></span>
<span class="line"><span>    double argper_;     ///&lt;近拱点角</span></span>
<span class="line"><span>    double trueA_;      ///&lt;真近点角</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    /// @brief 计算平均角速度变化率</span></span>
<span class="line"><span>    double getMeanMotion(double gm) const {return sqrt(gm / pow(getA(), 3));}</span></span>
<span class="line"><span>    </span></span>
<span class="line"><span>    /// @brief 计算半长轴</span></span>
<span class="line"><span>    double getA() const {return rp_ / (1 - e_);}</span></span>
<span class="line"><span></span></span>
<span class="line"><span>    /// @brief 计算半通径</span></span>
<span class="line"><span>    double getP() const {return rp_ * (1 + e_);}</span></span>
<span class="line"><span></span></span>
<span class="line"><span>    /// @brief 转换为字符串</span></span>
<span class="line"><span>    AST_CORE_API</span></span>
<span class="line"><span>    std::string toString() const;</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    A_DEF_POD_ITERABLE(double)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, rp)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, e)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, i)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, raan)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, argper)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, trueA)</span></span>
<span class="line"><span>};</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 春分点根数</span></span>
<span class="line"><span>class EquinElem</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    double a_;          ///&lt; semimajor axis length</span></span>
<span class="line"><span>    double h_;          ///&lt; e*sin(argper + raan)   omegabar=argper + raan</span></span>
<span class="line"><span>    double k_;          ///&lt; e*cos(argper + raan)</span></span>
<span class="line"><span>    double p_;          ///&lt; tan(i/2)*sin(raan)</span></span>
<span class="line"><span>    double q_;          ///&lt; tan(i/2)*cos(raan)</span></span>
<span class="line"><span>    double lambda_;     ///&lt; mean longitude = M + raan + argper</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    A_DEF_POD_ITERABLE(double)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, a)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, h)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, k)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, p)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, q)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, lambda)</span></span>
<span class="line"><span>};</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 改进春分点轨道根数, 180度奇异</span></span>
<span class="line"><span>class ModEquinElem</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    double p_;       ///&lt; p = a(1-e^2) 半通径</span></span>
<span class="line"><span>    double f_;        ///&lt; f = e*cos(argper+RAAN)</span></span>
<span class="line"><span>    double g_;        ///&lt; g = e*sin(argper+RAAN)</span></span>
<span class="line"><span>    double h_;        ///&lt; h = tan(i/2)cos(RAAN)</span></span>
<span class="line"><span>    double k_;        ///&lt; k = tan(i/2)sin(RAAN)</span></span>
<span class="line"><span>    double L_;        ///&lt; L = RAAN + argper + trueA</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    A_DEF_POD_ITERABLE(double)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, p)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, f)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, g)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, h)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, k)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, L)</span></span>
<span class="line"><span>};</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 德洛奈根数 </span></span>
<span class="line"><span>/// @details </span></span>
<span class="line"><span>/// 德洛奈根数是天体力学中的一组正则共轭变量</span></span>
<span class="line"><span>/// 由法国学者Delaunay,C.E.于19世纪60年代研究月球运动时提出</span></span>
<span class="line"><span>/// 常用于解析轨道理论</span></span>
<span class="line"><span>/// 依据：</span></span>
<span class="line"><span>///     《英汉天文学名词》李竞、许邦信主编，上海科技教育出版社，2000</span></span>
<span class="line"><span>///     《英汉天文学名词》，中国科学技术出版社，2015</span></span>
<span class="line"><span>class DelaunayElem</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span></span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    double L_;</span></span>
<span class="line"><span>    double G_;</span></span>
<span class="line"><span>    double H_;</span></span>
<span class="line"><span>    double l_;</span></span>
<span class="line"><span>    double g_;</span></span>
<span class="line"><span>    double h_;</span></span>
<span class="line"><span>public:</span></span>
<span class="line"><span>    A_DEF_POD_ITERABLE(double)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, L)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, G)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, H)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, l)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, g)</span></span>
<span class="line"><span>    AST_DEF_ACCESS_METHOD(double, h)</span></span>
<span class="line"><span>};</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数转换为直角坐标</span></span>
<span class="line"><span>/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param pos 输出位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 输出速度矢量 [m/s]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t coe2rv(const double* coe, double gm, double* pos, double* vel);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数转换为改进春分点轨道根数</span></span>
<span class="line"><span>/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t coe2mee(const double* coe, double* mee);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 春分点根数转换为直角坐标</span></span>
<span class="line"><span>/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param pos 输出位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 输出速度矢量 [m/s]</span></span>
<span class="line"><span>AST_CORE_CAPI void ee2rv(const double* ee, double gm, double* pos, double* vel);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 改进春分点轨道根数转换为直角坐标</span></span>
<span class="line"><span>/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param pos 输出位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 输出速度矢量 [m/s]</span></span>
<span class="line"><span>AST_CORE_CAPI void mee2rv(const double* mee, double gm, double* pos, double* vel);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为改进春分点轨道根数</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t rv2mee(const double* pos, const double* vel, double gm, double* mee);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 改进春分点轨道根数转换为经典轨道根数</span></span>
<span class="line"><span>/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]</span></span>
<span class="line"><span>/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t mee2coe(const double* mee, double* coe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为春分点根数</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]</span></span>
<span class="line"><span>AST_CORE_CAPI void rv2ee(const double* pos, const double* vel, double gm, double* ee);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为修正轨道根数</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t rv2moe(const double* pos, const double* vel, double gm, double* moe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为经典轨道根数</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t rv2coe(const double* pos, const double* vel, double gm, double* coe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 春分点根数转换为修正轨道根数</span></span>
<span class="line"><span>/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]</span></span>
<span class="line"><span>/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t ee2moe(const double* ee, double* moe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 修正轨道根数转换为春分点根数</span></span>
<span class="line"><span>/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t moe2ee(const double* moe, double* ee);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 修正轨道根数转换为经典轨道根数</span></span>
<span class="line"><span>/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>AST_CORE_CAPI err_t moe2coe(const double* moe, double* coe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数转换为修正轨道根数</span></span>
<span class="line"><span>/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>AST_CORE_CAPI void coe2moe(const double* coe, double* moe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 修正轨道根数转换为直角坐标</span></span>
<span class="line"><span>/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param pos 输出位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 输出速度矢量 [m/s]</span></span>
<span class="line"><span>AST_CORE_CAPI err_t moe2rv(const double* moe, double gm, double* pos, double* vel);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 修正轨道根数转换为改进春分点轨道根数</span></span>
<span class="line"><span>/// @param moe 修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]</span></span>
<span class="line"><span>AST_CORE_CAPI void moe2mee(const double* moe, double* mee);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数转换为春分点根数</span></span>
<span class="line"><span>/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]</span></span>
<span class="line"><span>AST_CORE_CAPI void coe2ee(const double* coe, double* ee);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 春分点根数转换为经典轨道根数</span></span>
<span class="line"><span>/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]</span></span>
<span class="line"><span>/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>AST_CORE_CAPI void ee2coe(const double* ee, double* coe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 春分点根数转换为改进春分点轨道根数</span></span>
<span class="line"><span>/// @param ee 春分点根数 [长半轴, h, k, p, q, 平经度]</span></span>
<span class="line"><span>/// @param mee 输出改进春分点轨道根数 [半通径, f, g, h, k, L]</span></span>
<span class="line"><span>AST_CORE_CAPI void ee2mee(const double* ee, double* mee);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 改进春分点轨道根数转换为春分点根数</span></span>
<span class="line"><span>/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]</span></span>
<span class="line"><span>/// @param ee 输出春分点根数 [长半轴, h, k, p, q, 平经度]</span></span>
<span class="line"><span>AST_CORE_CAPI void mee2ee(const double* mee, double* ee);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 改进春分点轨道根数转换为修正轨道根数</span></span>
<span class="line"><span>/// @param mee 改进春分点轨道根数 [半通径, f, g, h, k, L]</span></span>
<span class="line"><span>/// @param moe 输出修正轨道根数 [近拱点半径, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>AST_CORE_CAPI void mee2moe(const double* mee, double* moe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数转换为德洛奈根数</span></span>
<span class="line"><span>/// @param coe 经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param del 输出德洛奈根数, see DelaunayElem</span></span>
<span class="line"><span>AST_CORE_CAPI err_t coe2dela(const double* coe, double gm, double* dela);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 德洛奈根数转换为经典轨道根数</span></span>
<span class="line"><span>/// @param del 德洛奈根数, see DelaunayElem</span></span>
<span class="line"><span>/// @param coe 输出经典轨道根数 [长半轴, 偏心率, 轨道倾角, 升交点赤经, 近拱点角, 真近点角]</span></span>
<span class="line"><span>AST_CORE_CAPI err_t dela2coe(const double* dela, double gm, double* coe);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 改进春分点轨道根数转换为直角坐标（类引用版本）</span></span>
<span class="line"><span>/// @param mee 改进春分点轨道根数</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param pos 输出位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 输出速度矢量 [m/s]</span></span>
<span class="line"><span>AST_CORE_CAPI void aModEquinElemToCart(</span></span>
<span class="line"><span>    const ModEquinElem&amp; mee, </span></span>
<span class="line"><span>    double gm, </span></span>
<span class="line"><span>    Vector3d&amp; pos, </span></span>
<span class="line"><span>    Vector3d&amp; vel</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为改进春分点轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param mee 输出改进春分点轨道根数</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t aCartToModEquinElem(</span></span>
<span class="line"><span>    const Vector3d&amp; pos,</span></span>
<span class="line"><span>    const Vector3d&amp; vel,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    ModEquinElem&amp; mee</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数转换为改进春分点轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param elem 经典轨道根数</span></span>
<span class="line"><span>/// @param mee 输出改进春分点轨道根数</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t aOrbElemToModEquinElem(</span></span>
<span class="line"><span>    const OrbElem&amp; elem,</span></span>
<span class="line"><span>    ModEquinElem&amp; mee</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 改进春分点轨道根数转换为经典轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param mee 改进春分点轨道根数</span></span>
<span class="line"><span>/// @param elem 输出经典轨道根数</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t aModEquinElemToOrbElem(</span></span>
<span class="line"><span>    const ModEquinElem&amp; mee,</span></span>
<span class="line"><span>    OrbElem&amp; elem</span></span>
<span class="line"><span>);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为修正轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param modOrb 输出修正轨道根数</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI err_t	aCartToModOrbElem(</span></span>
<span class="line"><span>    const Vector3d&amp; pos,</span></span>
<span class="line"><span>    const Vector3d&amp; vel,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    ModOrbElem&amp; modOrb);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为经典轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param elem 输出经典轨道根数</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI</span></span>
<span class="line"><span>err_t aCartToOrbElem     (</span></span>
<span class="line"><span>    const Vector3d&amp; pos,</span></span>
<span class="line"><span>    const Vector3d&amp; vel,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    OrbElem&amp; elem);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 春分点根数转换为修正轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param equinElem 春分点根数</span></span>
<span class="line"><span>/// @param modOrb 输出修正轨道根数</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI</span></span>
<span class="line"><span>err_t aEquinElemToModOrb (</span></span>
<span class="line"><span>    const EquinElem&amp; equinElem,</span></span>
<span class="line"><span>    ModOrbElem&amp; modOrb);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 修正轨道根数转换为春分点根数（类引用版本）</span></span>
<span class="line"><span>/// @param modOrb 修正轨道根数</span></span>
<span class="line"><span>/// @param equinElem 输出春分点根数</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI</span></span>
<span class="line"><span>err_t aModOrbToEquinElem (</span></span>
<span class="line"><span>    const ModOrbElem&amp; modOrb,</span></span>
<span class="line"><span>    EquinElem&amp; equinElem);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 修正轨道根数转换为直角坐标（类引用版本）</span></span>
<span class="line"><span>/// @param modOrb 修正轨道根数</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param pos 输出位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 输出速度矢量 [m/s]</span></span>
<span class="line"><span>AST_CORE_CAPI</span></span>
<span class="line"><span>err_t aModOrbElemToCart  (</span></span>
<span class="line"><span>    const ModOrbElem&amp; modOrb,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    Vector3d&amp; pos,</span></span>
<span class="line"><span>    Vector3d&amp; vel);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数转换为直角坐标（类引用版本）</span></span>
<span class="line"><span>/// @param elem 经典轨道根数</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param pos 输出位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 输出速度矢量 [m/s]</span></span>
<span class="line"><span>/// @return 错误码，成功返回eNoError</span></span>
<span class="line"><span>AST_CORE_CAPI</span></span>
<span class="line"><span>err_t aOrbElemToCart     (</span></span>
<span class="line"><span>    const OrbElem&amp; elem,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    Vector3d&amp; pos,</span></span>
<span class="line"><span>    Vector3d&amp; vel);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为春分点根数（类引用版本）</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param equinElem 输出春分点根数</span></span>
<span class="line"><span>AST_CORE_CAPI</span></span>
<span class="line"><span>void	aCartToEquinElem(</span></span>
<span class="line"><span>    const Vector3d&amp; pos,</span></span>
<span class="line"><span>    const Vector3d&amp; vel,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    EquinElem&amp; equinElem);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 春分点根数转换为直角坐标（类引用版本）</span></span>
<span class="line"><span>/// @param equinElem 春分点根数</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param pos 输出位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 输出速度矢量 [m/s]</span></span>
<span class="line"><span>AST_CORE_CAPI</span></span>
<span class="line"><span>void aEquinElemToCart(</span></span>
<span class="line"><span>    const EquinElem&amp; equinElem,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    Vector3d&amp; pos,</span></span>
<span class="line"><span>    Vector3d&amp; vel);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 经典轨道根数转换为修正轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param elem 经典轨道根数</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param delaunay 输出修正轨道根数</span></span>
<span class="line"><span>AST_CORE_CAPI </span></span>
<span class="line"><span>err_t aOrbElemToDelaunay(</span></span>
<span class="line"><span>    const OrbElem&amp; elem,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    DelaunayElem&amp; delaunay);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 修正轨道根数转换为经典轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param delaunay 修正轨道根数</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @param elem 输出经典轨道根数</span></span>
<span class="line"><span>AST_CORE_CAPI </span></span>
<span class="line"><span>err_t aDelaunayToOrbElem(</span></span>
<span class="line"><span>    const DelaunayElem&amp; delaunay,</span></span>
<span class="line"><span>    double gm,</span></span>
<span class="line"><span>    OrbElem&amp; elem);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 直角坐标转换为修正轨道根数（类引用版本）</span></span>
<span class="line"><span>/// @param pos 位置矢量 [m]</span></span>
<span class="line"><span>/// @param vel 速度矢量 [m/s]</span></span>
<span class="line"><span>/// @param gm 引力参数 [m^3/s^2]</span></span>
<span class="line"><span>/// @return 修正轨道根数</span></span>
<span class="line"><span>A_ALWAYS_INLINE</span></span>
<span class="line"><span>ModOrbElem aCartToModOrbElem(const Vector3d&amp; r, const Vector3d&amp; v, double gm)</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>    ModOrbElem modOrbElem;</span></span>
<span class="line"><span>    aCartToModOrbElem(r, v, gm, modOrbElem);</span></span>
<span class="line"><span>    return modOrbElem;</span></span>
<span class="line"><span>}</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>AST_NAMESPACE_END</span></span>
<span class="line"><span> </span></span>
<span class="line"><span>AST_DECL_TYPE_ALIAS(CartState)</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div>`,23)])])}const c=n(e,[["render",p]]),h=JSON.parse('{"path":"/en/core/orbit-element.html","title":"Orbit Elements Definition and Conversion","lang":"en-US","frontmatter":{"description":"Orbit Elements Definition and Conversion Note: This document is AI-translated. Please refer to the source code for specific APIs and behaviors. Module Overview The OrbitElement ...","head":[["script",{"type":"application/ld+json"},"{\\"@context\\":\\"https://schema.org\\",\\"@type\\":\\"Article\\",\\"headline\\":\\"Orbit Elements Definition and Conversion\\",\\"image\\":[\\"\\"],\\"dateModified\\":\\"2026-01-21T11:03:44.000Z\\",\\"author\\":[{\\"@type\\":\\"Person\\",\\"name\\":\\"space-ast\\",\\"url\\":\\"https://github.com/space-ast\\"}]}"],["meta",{"property":"og:url","content":"https://github.com/space-ast/ast/ast/en/core/orbit-element.html"}],["meta",{"property":"og:site_name","content":"ast docs"}],["meta",{"property":"og:title","content":"Orbit Elements Definition and Conversion"}],["meta",{"property":"og:description","content":"Orbit Elements Definition and Conversion Note: This document is AI-translated. Please refer to the source code for specific APIs and behaviors. Module Overview The OrbitElement ..."}],["meta",{"property":"og:type","content":"article"}],["meta",{"property":"og:locale","content":"en-US"}],["meta",{"property":"og:locale:alternate","content":"zh-CN"}],["meta",{"property":"og:updated_time","content":"2026-01-21T11:03:44.000Z"}],["meta",{"property":"article:modified_time","content":"2026-01-21T11:03:44.000Z"}],["link",{"rel":"alternate","hreflang":"zh-cn","href":"https://github.com/space-ast/ast/ast/core/orbit-element.html"}]]},"git":{"createdTime":1763654899000,"updatedTime":1768993424000,"contributors":[{"name":"axel","username":"axel","email":"space_ast@163.com","commits":6,"url":"https://github.com/axel"}]},"readingTime":{"minutes":1.17,"words":351},"filePathRelative":"en/core/orbit-element.md","autoDesc":true}');export{c as comp,h as data};
