xpack("ast")
    set_homepage("https://space-ast.github.io/ast")
    set_title("航天仿真算法库SpaceAST")
    set_description("专注于航天仿真领域的算法库，提供了轨道力学、姿态、轨迹规划等核心算法的现代C++实现")
    set_copyright("版权所有 (C) 2025-present, SpaceAST项目.")
    set_author("axel <space_ast@163.com>")
    set_licensefile("LICENSE")
    set_license("Apache-2.0")
    set_formats(
        "nsis", "zip", "targz",  "rpm",                 -- 二进制包格式
        "srczip", "srctargz", "runself", "srpm", "deb"  -- 源文件包格式
    )
    -- 添加工程目标
    add_targets("ascript", "AppCLI", "AppMissionAnalysis")
    -- 源文件安装包的文件
    add_sourcefiles("(src/**)")
    add_sourcefiles("(include/**)")
    add_sourcefiles("(examples/**)")
    add_sourcefiles("(test/**)|**bm*")
    add_sourcefiles("(repo/**)")
    add_sourcefiles("*.lua", "*.md", "LICENSE*")
    -- xmake.sh（脚本式构建，不依赖 xmake）相关文件，runself 源码包解包后靠它现场编译
    add_sourcefiles("configure")
    add_sourcefiles("xmake.sh")
    add_sourcefiles("(scripts/xpack/runself.sh)")   -- 用括号包住，保证在包里保持 scripts/xpack/ 目录结构

    -- 二进制安装包的文件
    add_installfiles("(examples/**)|*.lua")
    add_installfiles("(test/**)|**bm*|*.lua")
    add_installfiles("scripts/xpack/port.lua", {filename = "xmake.lua"})
    on_load(function(package)
        import("core.project.project")
        -- 只有二进制包才添加平台和架构
        if package:with_binary() then
            local plat = package:plat()
            local arch = package:arch()
            package:set("basename", "SpaceAST-v$(version)-" .. plat .. "-" .. arch)
        else
            package:set("basename", "SpaceAST-v$(version)")
        end

        -- @note runself 是源码自解压包，解包后在目标机上用 xmake.sh（configure + make）
        -- 现场编译安装，所以既不需要工程 target，也不需要安装文件（否则会生成
        -- `xmake install -P .` 之类的安装命令）
        if package:format() == "runself" then
            package:set("targets", {})
            package:set("installfiles", {})
            package:set("basename", "SpaceAST-v$(version)-linux-any")
            return
        end

        -- 添加所有工程目标，排除AstVisVTK
        for targetname, _ in pairs(project.targets()) do
            if targetname:startswith("Ast") and targetname ~= "AstVisVTK" then
                package:add("targets", targetname)
            end
        end
        
        -- windows下手动添加debug库和release库
        if package:plat() == "windows" and package:with_binary() then
            local arch = package:arch()
            local debug_dir = "build/windows/" .. arch .. "/debug/"
            package:add("installfiles", debug_dir .. "Ast*D.lib", {prefixdir = "lib"})
            package:add("installfiles", debug_dir .. "Ast*D.dll", {prefixdir = "bin"})
            -- package:add("installfiles", debug_dir .. "Ast*D.pdb", {prefixdir = "bin"})
            local release_dir = "build/windows/" .. arch .. "/release/"
            package:add("installfiles", release_dir .. "Ast*.lib", {prefixdir = "lib"})
            package:add("installfiles", release_dir .. "Ast*.dll", {prefixdir = "bin"})
        end

        -- 添加python库
        if package:with_binary() then
            if package:plat() == "windows" then
                package:add("installfiles", "build/$(plat)/$(arch)/debug/*.pyd",   {prefixdir = "bin"})
                package:add("installfiles", "build/$(plat)/$(arch)/release/*.pyd", {prefixdir = "bin"})
                package:add("installfiles", "build/$(plat)/$(arch)/debug/*.py",   {prefixdir = "bin"})
            end
            package:add("installfiles", "build/$(plat)/$(arch)/release/*.py",   {prefixdir = "bin"})
            -- release_pyfiles = os.files("build/$(plat)/$(arch)/release/*.py")
            -- debug_pyfiles = os.files("build/$(plat)/$(arch)/debug/*.py")
            -- print("release_pyfiles: ", release_pyfiles)
            -- print("debug_pyfiles: ", debug_pyfiles)
        end
    end)

    -- runself（源码自解压安装包）：解包后调用工程里的 scripts/xpack/runself.sh，
    -- 用 xmake.sh 现场编译并安装到 --prefix（默认 ~/.local），
    -- 用户执行 .run 时传的参数（--prefix/-j/--mode/...）由 makeself 透传进来
    after_installcmd(function (package, batchcmds)
        if package:format() == "runself" then
            batchcmds:runv("sh", {"./scripts/xpack/runself.sh", "$@"})
        end
    end)

    
