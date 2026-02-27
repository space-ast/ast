import{_ as n}from"./plugin-vue_export-helper-DlAUqK2U.js";import{c as a,a as i,o as l}from"./app-CwP4iUow.js";const e={};function p(d,s){return l(),a("div",null,[...s[0]||(s[0]=[i(`<h1 id="基础天文计算模块" tabindex="-1"><a class="header-anchor" href="#基础天文计算模块"><span>基础天文计算模块</span></a></h1><h2 id="模块概述" tabindex="-1"><a class="header-anchor" href="#模块概述"><span>模块概述</span></a></h2><p>SOFA模块是ast项目中的天文基础计算核心模块，基于国际天文联合会(IAU)的SOFA(Standards of Fundamental Astronomy)库实现。<br> 该模块提供了一系列高精度的天文计算函数，用于处理岁差、章动、恒星时等天文数据的计算。</p><p>SOFA模块在原始SOFA库的基础上进行了接口适配，使其能够与ast项目的自定义类型（如TimePoint）无缝集成，并提供了更加灵活的扩展机制。<br> 函数命名遵循ast项目的命名规范，部分实现细节根据项目需求进行了优化和调整，不保证与原始SOFA库在边界情况和精度上完全一致。</p><h2 id="核心概念" tabindex="-1"><a class="header-anchor" href="#核心概念"><span>核心概念</span></a></h2><h3 id="_1-sofa库简介" tabindex="-1"><a class="header-anchor" href="#_1-sofa库简介"><span>1. SOFA库简介</span></a></h3><p>SOFA库是国际天文联合会(IAU)发布的权威天文算法库，包含了实现各种天文标准模型的算法和程序。SOFA模块基于SOFA库2023-10-11版本实现。</p><h3 id="_2-时间系统" tabindex="-1"><a class="header-anchor" href="#_2-时间系统"><span>2. 时间系统</span></a></h3><p>SOFA模块涉及多种时间系统：</p><ul><li><strong>TT（地球时）</strong>：用于计算岁差、章动等长期天文现象</li><li><strong>TDB（质心动力学时）</strong>：理论上用于高精度天文计算</li><li><strong>UT1（世界时）</strong>：用于计算地球自转角度</li></ul><h3 id="_3-天文标准" tabindex="-1"><a class="header-anchor" href="#_3-天文标准"><span>3. 天文标准</span></a></h3><p>SOFA模块实现了多种IAU天文标准：</p><ul><li><strong>IAU1976</strong>：岁差模型</li><li><strong>IAU1980</strong>：章动模型</li><li><strong>IERS1996</strong>：章动修正模型</li><li><strong>IAU1982</strong>：格林尼治平恒星时模型</li><li><strong>IAU1994</strong>：格林尼治视恒星时模型</li><li><strong>IAU2000</strong>：地球自转角度模型</li></ul><h2 id="主要功能" tabindex="-1"><a class="header-anchor" href="#主要功能"><span>主要功能</span></a></h2><h3 id="_1-岁差计算" tabindex="-1"><a class="header-anchor" href="#_1-岁差计算"><span>1. 岁差计算</span></a></h3><ul><li><code>aPrecession_IAU1976</code>：依据IAU1976规范计算岁差角</li></ul><h3 id="_2-章动计算" tabindex="-1"><a class="header-anchor" href="#_2-章动计算"><span>2. 章动计算</span></a></h3><ul><li><code>aNutation_IAU1980</code>：依据IAU1980规范计算章动角</li><li><code>aNutationFuncSet</code>：设置自定义章动角计算函数</li><li><code>aNutationMethodSet</code>：设置章动角计算方法</li></ul><h3 id="_3-黄赤交角计算" tabindex="-1"><a class="header-anchor" href="#_3-黄赤交角计算"><span>3. 黄赤交角计算</span></a></h3><ul><li><code>aMeanObliquity_IAU1980</code>：计算平均黄赤交角</li></ul><h3 id="_4-恒星时计算" tabindex="-1"><a class="header-anchor" href="#_4-恒星时计算"><span>4. 恒星时计算</span></a></h3><ul><li><code>aGMST_IAU1982</code>：计算格林尼治平恒星时</li><li><code>aGAST_IAU1994</code>：计算格林尼治视恒星时</li><li><code>aEquationOfEquinoxes_IAU1994</code>：计算赤经章动</li></ul><h3 id="_5-地球自转计算" tabindex="-1"><a class="header-anchor" href="#_5-地球自转计算"><span>5. 地球自转计算</span></a></h3><ul><li><code>aEarthRotationAngle_IAU2000</code>：计算地球自转角度</li></ul><h2 id="用法示例" tabindex="-1"><a class="header-anchor" href="#用法示例"><span>用法示例</span></a></h2><h3 id="_1-岁差角计算" tabindex="-1"><a class="header-anchor" href="#_1-岁差角计算"><span>1. 岁差角计算</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/SOFA.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/TimePoint.hpp&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">AST_USING_NAMESPACE</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">()</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">{</span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 创建时间点</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    TimePoint tp </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> TimePoint::</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">FromUTC</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2026</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 计算岁差角</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> zeta, z, theta;</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aPrecession_IAU1976</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(tp, zeta, z, theta);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 输出结果</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;时间点: 2026-01-01 00:00:00 UTC</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;岁差角 zeta: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> 弧度</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, zeta);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;岁差角 z: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> 弧度</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, z);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;岁差角 theta: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> 弧度</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, theta);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    return</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h3 id="_2-章动角计算" tabindex="-1"><a class="header-anchor" href="#_2-章动角计算"><span>2. 章动角计算</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/SOFA.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/TimePoint.hpp&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">AST_USING_NAMESPACE</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">()</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">{</span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 创建时间点</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    TimePoint tp </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> TimePoint::</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">FromUTC</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2026</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 计算章动角</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> dpsi, deps;</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aNutation</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(tp, dpsi, deps);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 输出结果</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;时间点: 2026-01-01 00:00:00 UTC</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;黄经章动角 dpsi: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> 弧度</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, dpsi);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;交角章动角 deps: </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> 弧度</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, deps);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    return</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h3 id="_3-格林尼治恒星时计算" tabindex="-1"><a class="header-anchor" href="#_3-格林尼治恒星时计算"><span>3. 格林尼治恒星时计算</span></a></h3><div class="language-cpp line-numbers-mode" data-highlighter="shiki" data-ext="cpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-cpp"><span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/SOFA.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/TimePoint.hpp&quot;</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">#include</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> &quot;AstCore/RunTime.hpp&quot;</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">AST_USING_NAMESPACE</span></span>
<span class="line"></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">int</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> main</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">()</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">{</span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 加载基础数据并初始化，需要EOP数据中的 UT1 - UTC</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aInitialize</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">();</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 创建时间点</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    TimePoint tp </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> TimePoint::</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">FromUTC</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">2026</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">1</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 计算格林尼治平恒星时</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> gmst </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> aGMST_IAU1982</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(tp);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 计算格林尼治视恒星时</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> gast </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> aGAST_IAU1994</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(tp);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 计算地球自转角度</span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    double</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;"> era </span><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">=</span><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;"> aEarthRotationAngle_IAU2000</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(tp);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 输出结果</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;时间点: 2026-01-01 00:00:00 UTC</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;格林尼治平恒星时 (GMST): </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> 弧度</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, gmst);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;格林尼治视恒星时 (GAST): </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> 弧度</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, gast);</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    printf</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">(</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;地球自转角度 (ERA): </span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;">%.9f</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;"> 弧度</span><span style="--shiki-light:#0184BC;--shiki-dark:#56B6C2;">\\n</span><span style="--shiki-light:#50A14F;--shiki-dark:#98C379;">&quot;</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">, era);</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A0A1A7;--shiki-light-font-style:italic;--shiki-dark:#7F848E;--shiki-dark-font-style:italic;">    // 清理资源</span></span>
<span class="line"><span style="--shiki-light:#4078F2;--shiki-dark:#61AFEF;">    aUninitialize</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">();</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">    </span></span>
<span class="line"><span style="--shiki-light:#A626A4;--shiki-dark:#C678DD;">    return</span><span style="--shiki-light:#986801;--shiki-dark:#D19A66;"> 0</span><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">;</span></span>
<span class="line"><span style="--shiki-light:#383A42;--shiki-dark:#ABB2BF;">}</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div><h2 id="注意事项" tabindex="-1"><a class="header-anchor" href="#注意事项"><span>注意事项</span></a></h2><ol><li><strong>时间系统差异</strong>：注意区分TT、TDB、UT1等不同时间系统的使用场景</li><li><strong>精度考量</strong>：SOFA模块提供高精度计算，但某些近似计算（如直接用TT代替TDB）可能在极端情况下影响精度</li><li><strong>自定义实现</strong>：可以通过<code>aNutationFuncSet</code>设置自定义章动角计算函数，但需确保接口一致性</li><li><strong>SOFA许可证</strong>：SOFA模块基于SOFA库衍生，需遵守SOFA软件许可证条款</li></ol><h2 id="许可证声明" tabindex="-1"><a class="header-anchor" href="#许可证声明"><span>许可证声明</span></a></h2><p>本模块基于国际天文联合会(IAU)的SOFA软件库衍生实现，必须遵守SOFA软件许可证条款。</p><p>根据SOFA软件许可证的要求，特此声明：</p><ol><li><p>本模块的代码使用了从SOFA软件库（版本2023-10-11）获得的算法和计算例程。<br> 具体对应的SOFA函数请参考函数注释</p></li><li><p>SOFA软件由国际天文联合会(IAU) SOFA Board所有。<br> SOFA软件许可证请参考：<a href="https://www.iausofa.org/terms-and-conditions" target="_blank" rel="noopener noreferrer">https://www.iausofa.org/terms-and-conditions</a></p></li><li><p>根据SOFA许可证第3(c)条要求，本衍生作品的函数名不使用&quot;iau&quot;或&quot;sofa&quot;前缀。<br> 本模块使用&quot;a&quot;前缀表示&quot;astronomy&quot;天文计算。</p></li><li><p>根据SOFA许可证第3(a)条要求，声明本软件：<br> (i) 使用了从SOFA软件派生的计算例程；<br> (ii) 不是由SOFA提供或认可的软件。</p></li><li><p>本衍生作品与原始SOFA软件的主要区别：</p><ul><li>函数接口针对ast项目的TimePoint等自定义类型进行了适配</li><li>函数命名遵循ast项目的命名规范</li><li>部分实现细节根据项目需求进行了优化和调整</li><li>不保证与原始SOFA库在边界情况和精度上完全一致</li></ul></li><li><p>根据SOFA许可证第3(e)条要求，必须在SOFA软件衍生作品源代码分发中完整保留SOFA软件的版权声明<br> SOFA软件的版权声明请参阅源代码文件。</p></li></ol><h2 id="api参考" tabindex="-1"><a class="header-anchor" href="#api参考"><span>API参考</span></a></h2><div class="language-hpp line-numbers-mode" data-highlighter="shiki" data-ext="hpp" style="--shiki-light:#383A42;--shiki-dark:#abb2bf;--shiki-light-bg:#FAFAFA;--shiki-dark-bg:#282c34;"><pre class="shiki shiki-themes one-light one-dark-pro vp-code"><code class="language-hpp"><span class="line"><span>///</span></span>
<span class="line"><span>/// @file      SOFA.hpp</span></span>
<span class="line"><span>/// @brief     天文基础计算函数（基于SOFA库实现）</span></span>
<span class="line"><span>/// @details   本模块的函数基于国际天文联合会(IAU) SOFA库实现，</span></span>
<span class="line"><span>///            但为适应本项目需求进行了接口和实现上的调整。</span></span>
<span class="line"><span>/// @author    axel</span></span>
<span class="line"><span>/// @date      2026-01-06</span></span>
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
<span class="line"><span>#include &quot;AstCore/TimePoint.hpp&quot;</span></span>
<span class="line"><span></span></span>
<span class="line"><span>AST_NAMESPACE_BEGIN</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>//==============================================================================</span></span>
<span class="line"><span>// SOFA (Standards of Fundamental Astronomy) 许可证声明</span></span>
<span class="line"><span>//==============================================================================</span></span>
<span class="line"><span>/*</span></span>
<span class="line"><span> * 重要声明：本文件中的函数基于国际天文联合会(IAU)的SOFA软件库实现。</span></span>
<span class="line"><span> *</span></span>
<span class="line"><span> * 根据SOFA软件许可证的要求，特此声明：</span></span>
<span class="line"><span> *</span></span>
<span class="line"><span> * 1. 本模块的代码使用了从SOFA软件库（版本2023-10-11）获得的算法和计算例程。</span></span>
<span class="line"><span> *    具体对应的SOFA函数请参考函数注释</span></span>
<span class="line"><span> *</span></span>
<span class="line"><span> * 2. SOFA软件由国际天文联合会(IAU) SOFA Board所有。</span></span>
<span class="line"><span> *    SOFA软件许可证请参考：https://www.iausofa.org/terms-and-conditions</span></span>
<span class="line"><span> *</span></span>
<span class="line"><span> * 3. 根据SOFA许可证第3(c)条要求，本衍生作品的函数名不使用&quot;iau&quot;或&quot;sofa&quot;前缀。</span></span>
<span class="line"><span> *    本模块使用&quot;a&quot;前缀表示&quot;astronomy&quot;天文计算。</span></span>
<span class="line"><span> *</span></span>
<span class="line"><span> * 4. 根据SOFA许可证第3(a)条要求，声明本软件：</span></span>
<span class="line"><span> *    (i) 使用了从SOFA软件派生的计算例程；</span></span>
<span class="line"><span> *    (ii) 不是由SOFA提供或认可的软件。</span></span>
<span class="line"><span> *</span></span>
<span class="line"><span> * 5. 本衍生作品与原始SOFA软件的主要区别：</span></span>
<span class="line"><span> *    - 函数接口针对ast项目的TimePoint等自定义类型进行了适配</span></span>
<span class="line"><span> *    - 函数命名遵循ast项目的命名规范</span></span>
<span class="line"><span> *    - 部分实现细节根据项目需求进行了优化和调整</span></span>
<span class="line"><span> *    - 不保证与原始SOFA库在边界情况和精度上完全一致</span></span>
<span class="line"><span> *</span></span>
<span class="line"><span> * 6. 根据SOFA许可证第3(e)条要求，必须在SOFA软件衍生作品源代码分发中完整保留SOFA软件的版权声明</span></span>
<span class="line"><span> *    SOFA软件的版权声明请参阅本文件底部。</span></span>
<span class="line"><span> * </span></span>
<span class="line"><span> */</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定时间差的岁差角，依据IAU1976规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauPrec76</span></span>
<span class="line"><span>/// @param[in] t 与J2000.0 TDB 的时间差（儒略世纪）</span></span>
<span class="line"><span>/// @param[out] zeta 岁差zeta角（弧度）</span></span>
<span class="line"><span>/// @param[out] z 岁差z角（弧度）</span></span>
<span class="line"><span>/// @param[out] theta 岁差theta角（弧度）</span></span>
<span class="line"><span>AST_CORE_CAPI void aPrecession_IAU1976(double t, double&amp; zeta, double&amp; z, double&amp; theta);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定时间差的平均黄赤交角，依据IAU1980规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauObl80</span></span>
<span class="line"><span>/// @param[in] t 与J2000.0 TT 的时间差（儒略世纪）</span></span>
<span class="line"><span>/// @return 平均黄赤交角（弧度）</span></span>
<span class="line"><span>AST_CORE_CAPI double aMeanObliquity_IAU1980(double t);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定时间差的章动角，依据IERS1996规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauNut06</span></span>
<span class="line"><span>/// @param[in] t 与J2000.0 TT 的时间差（儒略世纪）</span></span>
<span class="line"><span>/// @param[out] dpsi 黄经章动角dpsi（弧度）</span></span>
<span class="line"><span>/// @param[out] deps 交角章动角deps（弧度）</span></span>
<span class="line"><span>/// @param[out] eqecorr 赤经章动修正量</span></span>
<span class="line"><span>AST_CORE_CAPI void aNutation_IERS1996(double t, double &amp;dpsi, double &amp;deps, double* eqecorr);</span></span>
<span class="line"><span>AST_CORE_CAPI void aNutation_IERS1996_Cache(double t, double &amp;dpsi, double &amp;deps, double* eqecorr);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定时间差的章动角，依据IAU1980规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauNut80</span></span>
<span class="line"><span>/// @param[in] t 与J2000.0 TT 的时间差（儒略世纪）</span></span>
<span class="line"><span>/// @param[out] dpsi 黄经章动角dpsi（弧度）</span></span>
<span class="line"><span>/// @param[out] deps 交角章动角deps（弧度）</span></span>
<span class="line"><span>A_ALWAYS_INLINE void aNutation_IAU1980(double t, double &amp;dpsi, double &amp;deps)</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>    aNutation_IERS1996(t, dpsi, deps, nullptr);</span></span>
<span class="line"><span>}</span></span>
<span class="line"><span>A_ALWAYS_INLINE void aNutation_IAU1980_Cache(double t, double &amp;dpsi, double &amp;deps)</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>    aNutation_IERS1996_Cache(t, dpsi, deps, nullptr);</span></span>
<span class="line"><span>}</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>A_ALWAYS_INLINE void aPrecession_IAU1976(const TimePoint&amp; tp, double&amp; zeta, double&amp; z, double&amp; theta){</span></span>
<span class="line"><span>    /// @fixme</span></span>
<span class="line"><span>    /// aPrecession_IAU1976函数的参数是TDB，而不是TT</span></span>
<span class="line"><span>    /// 直接将TT作为TDB使用是可行的吗？</span></span>
<span class="line"><span>    /// 参考SOFA的 iauPmat76， 也是将TT作为TDB使用</span></span>
<span class="line"><span>    aPrecession_IAU1976(tp.julianCenturyFromJ2000TT(), zeta, z, theta);</span></span>
<span class="line"><span>}</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>A_ALWAYS_INLINE double aMeanObliquity_IAU1980(const TimePoint&amp; tp){</span></span>
<span class="line"><span>    return aMeanObliquity_IAU1980(tp.julianCenturyFromJ2000TT());</span></span>
<span class="line"><span>}</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>typedef err_t (*NutationFunc)(const TimePoint&amp; tp, double &amp;dpsi, double &amp;deps);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>A_ALWAYS_INLINE void aNutation_IAU1980(const TimePoint&amp; tp, double&amp; dpsi, double&amp; deps)</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>    return aNutation_IAU1980(tp.julianCenturyFromJ2000TT(), dpsi, deps);</span></span>
<span class="line"><span>}</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 章动角计算函数指针</span></span>
<span class="line"><span>/// @details 指向内部实现的函数指针，默认指向aNutation_IAU1980</span></span>
<span class="line"><span>/// 可根据需要切换为其他实现，例如通过JPL DE星历数据计算章动角</span></span>
<span class="line"><span>/// 或者用户提供的其他实现，注意需要确保接口一致（参数和返回值），否则会导致调用错误</span></span>
<span class="line"><span>AST_CORE_CAPI NutationFunc aNutation;</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 设置章动角计算函数指针</span></span>
<span class="line"><span>/// @details </span></span>
<span class="line"><span>/// 根据输入的函数指针，切换内部实现的章动角计算函数指针，例如用户提供的其他实现</span></span>
<span class="line"><span>/// 注意需要确保接口一致（参数和返回值），否则会导致调用错误</span></span>
<span class="line"><span>inline void aNutationFuncSet(NutationFunc func){</span></span>
<span class="line"><span>    aNutation = func;</span></span>
<span class="line"><span>}</span></span>
<span class="line"><span></span></span>
<span class="line"><span>enum class ENutationMethod</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>    eIAU1980 = 0,</span></span>
<span class="line"><span>    eJplDe = 1,</span></span>
<span class="line"><span>    eUser = 2,      // 用户自定义实现</span></span>
<span class="line"><span>};</span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 设置章动角计算函数</span></span>
<span class="line"><span>/// @details 根据输入的枚举值，切换内部实现的章动角计算函数指针</span></span>
<span class="line"><span>/// @param[in] func 枚举值，指定要切换的章动角计算函数</span></span>
<span class="line"><span>/// @return err_t 错误码，0表示成功，其他值表示失败</span></span>
<span class="line"><span>AST_CORE_CAPI err_t aNutationMethodSet(ENutationMethod method);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 获取当前章动角计算函数</span></span>
<span class="line"><span>/// @details 返回当前内部实现的章动角计算函数指针</span></span>
<span class="line"><span>/// @return ENutationMethod 当前章动角计算函数的枚举值</span></span>
<span class="line"><span>AST_CORE_CAPI ENutationMethod aNutationMethodGet();</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定时间点的格林尼治平恒星时（GMST），依据IAU1982规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauGmst82</span></span>
<span class="line"><span>/// @param[in] tp 时间点</span></span>
<span class="line"><span>/// @return 对应时间点的格林尼治平恒星时 GMST（弧度）</span></span>
<span class="line"><span>AST_CORE_CAPI double aGMST_IAU1982(const TimePoint&amp; tp);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定UT1时间的格林尼治平恒星时（GMST），依据IAU1982规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauGmst82</span></span>
<span class="line"><span>/// @param[in] jdUT1 UT1时间（儒略日）</span></span>
<span class="line"><span>/// @return 对应UT1时间的格林尼治平恒星时 GMST（弧度）</span></span>
<span class="line"><span>AST_CORE_CAPI double aGMST_UT1_IAU1982(const JulianDate&amp; jdUT1);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定时间点的格林尼治视恒星时（GAST），依据IAU1982规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauGst94</span></span>
<span class="line"><span>/// @param[in] tp 时间点</span></span>
<span class="line"><span>/// @return 对应时间点的格林尼治视恒星时 GAST（弧度）</span></span>
<span class="line"><span>AST_CORE_CAPI double aGAST_IAU1994(const TimePoint&amp; tp);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定时间点的赤经章动，依据IAU1994规范</span></span>
<span class="line"><span>/// @details 赤经章动是真春分点平春分点之间的时角差，</span></span>
<span class="line"><span>/// 用于将格林尼治平恒星时（GMST）转换为格林尼治视恒星时（GAST）</span></span>
<span class="line"><span>/// 公式：GAST = GMST + Equation Of Equinoxes</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauEqeq94</span></span>
<span class="line"><span>/// @param[in] t 与J2000.0 TDB 的时间差（儒略世纪）</span></span>
<span class="line"><span>/// @return 赤经章动（弧度）</span></span>
<span class="line"><span>AST_CORE_CAPI double aEquationOfEquinoxes_IAU1994(double t);</span></span>
<span class="line"><span></span></span>
<span class="line"><span>A_ALWAYS_INLINE double aEquationOfEquinoxes_IAU1994(const TimePoint&amp; tp)</span></span>
<span class="line"><span>{</span></span>
<span class="line"><span>    /// 直接将TT作为TDB使用是可行的吗？</span></span>
<span class="line"><span>    return aEquationOfEquinoxes_IAU1994(tp.julianCenturyFromJ2000TT());</span></span>
<span class="line"><span>}</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定时间点的地球自转角度，依据IAU2000规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauEra00</span></span>
<span class="line"><span>/// @param[in] tp 时间点</span></span>
<span class="line"><span>/// @return 对应时间点的地球自转角度（弧度）</span></span>
<span class="line"><span>AST_CORE_CAPI double aEarthRotationAngle_IAU2000(const TimePoint&amp; tp);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/// @brief 计算给定UT1时间的地球自转角度，依据IAU2000规范</span></span>
<span class="line"><span>/// @details 参考SOFA函数 iauEra00</span></span>
<span class="line"><span>/// @param[in] jdUT1 UT1时间（儒略日）</span></span>
<span class="line"><span>/// @return 对应UT1时间的地球自转角度（弧度）</span></span>
<span class="line"><span>AST_CORE_CAPI double aEarthRotationAngleUT1_IAU2000(const JulianDate&amp; jdUT1);</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>AST_NAMESPACE_END</span></span>
<span class="line"><span></span></span>
<span class="line"><span></span></span>
<span class="line"><span>/* ----------------------------------------------------------------------------------</span></span>
<span class="line"><span> * 根据SOFA许可证第3(e)条要求，必须在SOFA软件衍生作品源代码分发中完整保留SOFA软件的版权声明</span></span>
<span class="line"><span> * 下面是SOFA软件的版权声明</span></span>
<span class="line"><span> * --------------------------------------------------------------------------------- */ </span></span>
<span class="line"><span></span></span>
<span class="line"><span>/*----------------------------------------------------------------------</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  Copyright (C) 2023</span></span>
<span class="line"><span>**  Standards of Fundamental Astronomy Board</span></span>
<span class="line"><span>**  of the International Astronomical Union.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  =====================</span></span>
<span class="line"><span>**  SOFA Software License</span></span>
<span class="line"><span>**  =====================</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  NOTICE TO USER:</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  BY USING THIS SOFTWARE YOU ACCEPT THE FOLLOWING SIX TERMS AND</span></span>
<span class="line"><span>**  CONDITIONS WHICH APPLY TO ITS USE.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  1. The Software is owned by the IAU SOFA Board (&quot;SOFA&quot;).</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  2. Permission is granted to anyone to use the SOFA software for any</span></span>
<span class="line"><span>**     purpose, including commercial applications, free of charge and</span></span>
<span class="line"><span>**     without payment of royalties, subject to the conditions and</span></span>
<span class="line"><span>**     restrictions listed below.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  3. You (the user) may copy and distribute SOFA source code to others,</span></span>
<span class="line"><span>**     and use and adapt its code and algorithms in your own software,</span></span>
<span class="line"><span>**     on a world-wide, royalty-free basis.  That portion of your</span></span>
<span class="line"><span>**     distribution that does not consist of intact and unchanged copies</span></span>
<span class="line"><span>**     of SOFA source code files is a &quot;derived work&quot; that must comply</span></span>
<span class="line"><span>**     with the following requirements:</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**     a) Your work shall be marked or carry a statement that it</span></span>
<span class="line"><span>**        (i) uses routines and computations derived by you from</span></span>
<span class="line"><span>**        software provided by SOFA under license to you; and</span></span>
<span class="line"><span>**        (ii) does not itself constitute software provided by and/or</span></span>
<span class="line"><span>**        endorsed by SOFA.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**     b) The source code of your derived work must contain descriptions</span></span>
<span class="line"><span>**        of how the derived work is based upon, contains and/or differs</span></span>
<span class="line"><span>**        from the original SOFA software.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**     c) The names of all routines in your derived work shall not</span></span>
<span class="line"><span>**        include the prefix &quot;iau&quot; or &quot;sofa&quot; or trivial modifications</span></span>
<span class="line"><span>**        thereof such as changes of case.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**     d) The origin of the SOFA components of your derived work must</span></span>
<span class="line"><span>**        not be misrepresented;  you must not claim that you wrote the</span></span>
<span class="line"><span>**        original software, nor file a patent application for SOFA</span></span>
<span class="line"><span>**        software or algorithms embedded in the SOFA software.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**     e) These requirements must be reproduced intact in any source</span></span>
<span class="line"><span>**        distribution and shall apply to anyone to whom you have</span></span>
<span class="line"><span>**        granted a further right to modify the source code of your</span></span>
<span class="line"><span>**        derived work.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**     Note that, as originally distributed, the SOFA software is</span></span>
<span class="line"><span>**     intended to be a definitive implementation of the IAU standards,</span></span>
<span class="line"><span>**     and consequently third-party modifications are discouraged.  All</span></span>
<span class="line"><span>**     variations, no matter how minor, must be explicitly marked as</span></span>
<span class="line"><span>**     such, as explained above.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  4. You shall not cause the SOFA software to be brought into</span></span>
<span class="line"><span>**     disrepute, either by misuse, or use for inappropriate tasks, or</span></span>
<span class="line"><span>**     by inappropriate modification.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  5. The SOFA software is provided &quot;as is&quot; and SOFA makes no warranty</span></span>
<span class="line"><span>**     as to its use or performance.   SOFA does not and cannot warrant</span></span>
<span class="line"><span>**     the performance or results which the user may obtain by using the</span></span>
<span class="line"><span>**     SOFA software.  SOFA makes no warranties, express or implied, as</span></span>
<span class="line"><span>**     to non-infringement of third party rights, merchantability, or</span></span>
<span class="line"><span>**     fitness for any particular purpose.  In no event will SOFA be</span></span>
<span class="line"><span>**     liable to the user for any consequential, incidental, or special</span></span>
<span class="line"><span>**     damages, including any lost profits or lost savings, even if a</span></span>
<span class="line"><span>**     SOFA representative has been advised of such damages, or for any</span></span>
<span class="line"><span>**     claim by any third party.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  6. The provision of any version of the SOFA software under the terms</span></span>
<span class="line"><span>**     and conditions specified herein does not imply that future</span></span>
<span class="line"><span>**     versions will also be made available under the same terms and</span></span>
<span class="line"><span>**     conditions.</span></span>
<span class="line"><span>*</span></span>
<span class="line"><span>**  In any published work or commercial product which uses the SOFA</span></span>
<span class="line"><span>**  software directly, acknowledgement (see www.iausofa.org) is</span></span>
<span class="line"><span>**  appreciated.</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**  Correspondence concerning SOFA software should be addressed as</span></span>
<span class="line"><span>**  follows:</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**      By email:  sofa@ukho.gov.uk</span></span>
<span class="line"><span>**      By post:   IAU SOFA Center</span></span>
<span class="line"><span>**                 HM Nautical Almanac Office</span></span>
<span class="line"><span>**                 UK Hydrographic Office</span></span>
<span class="line"><span>**                 Admiralty Way, Taunton</span></span>
<span class="line"><span>**                 Somerset, TA1 2DN</span></span>
<span class="line"><span>**                 United Kingdom</span></span>
<span class="line"><span>**</span></span>
<span class="line"><span>**--------------------------------------------------------------------*/</span></span></code></pre><div class="line-numbers" aria-hidden="true" style="counter-reset:line-number 0;"><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div><div class="line-number"></div></div></div>`,39)])])}const c=n(e,[["render",p]]),h=JSON.parse('{"path":"/core/sofa.html","title":"基础天文计算模块","lang":"zh-CN","frontmatter":{"description":"基础天文计算模块 模块概述 SOFA模块是ast项目中的天文基础计算核心模块，基于国际天文联合会(IAU)的SOFA(Standards of Fundamental Astronomy)库实现。 该模块提供了一系列高精度的天文计算函数，用于处理岁差、章动、恒星时等天文数据的计算。 SOFA模块在原始SOFA库的基础上进行了接口适配，使其能够与ast项...","head":[["script",{"type":"application/ld+json"},"{\\"@context\\":\\"https://schema.org\\",\\"@type\\":\\"Article\\",\\"headline\\":\\"基础天文计算模块\\",\\"image\\":[\\"\\"],\\"dateModified\\":\\"2026-01-14T02:33:07.000Z\\",\\"author\\":[{\\"@type\\":\\"Person\\",\\"name\\":\\"space-ast\\",\\"url\\":\\"https://github.com/space-ast\\"}]}"],["meta",{"property":"og:url","content":"https://github.com/space-ast/ast/ast/core/sofa.html"}],["meta",{"property":"og:site_name","content":"🛰️航天仿真算法库文档"}],["meta",{"property":"og:title","content":"基础天文计算模块"}],["meta",{"property":"og:description","content":"基础天文计算模块 模块概述 SOFA模块是ast项目中的天文基础计算核心模块，基于国际天文联合会(IAU)的SOFA(Standards of Fundamental Astronomy)库实现。 该模块提供了一系列高精度的天文计算函数，用于处理岁差、章动、恒星时等天文数据的计算。 SOFA模块在原始SOFA库的基础上进行了接口适配，使其能够与ast项..."}],["meta",{"property":"og:type","content":"article"}],["meta",{"property":"og:locale","content":"zh-CN"}],["meta",{"property":"og:locale:alternate","content":"en-US"}],["meta",{"property":"og:updated_time","content":"2026-01-14T02:33:07.000Z"}],["meta",{"property":"article:modified_time","content":"2026-01-14T02:33:07.000Z"}],["link",{"rel":"alternate","hreflang":"en-us","href":"https://github.com/space-ast/ast/ast/en/core/sofa.html"}]]},"git":{"createdTime":1768357987000,"updatedTime":1768357987000,"contributors":[{"name":"axel","username":"axel","email":"space_ast@163.com","commits":1,"url":"https://github.com/axel"}]},"readingTime":{"minutes":3.53,"words":1059},"filePathRelative":"core/sofa.md","autoDesc":true}');export{c as comp,h as data};
