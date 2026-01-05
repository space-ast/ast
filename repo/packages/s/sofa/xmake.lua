package("sofa")
    set_homepage("http://www.iausofa.org/")
    set_description("authoritative set of algorithms and procedures that implement standard models used in fundamental astronomy.")
    
    add_urls("http://www.iausofa.org/s/sofa_c-$(version)tar.gz", {filename="sofa_c.tar.gz"})

    add_versions("20231011", "d9c10833cae8b4d9361a0ffda31ec361fd1262362025bec4d4e51a880150ace2")
    -- add_versions("20210512", "3362baba47ce965af1fc6a3141052a0e1f643d186e8deca83df1ea97455bb2fd")
    
    on_install(function (package)
        io.writefile("xmake.lua", string.format([[
            target("sofa")
            	add_rules("mode.debug", "mode.release")
                set_languages("ansi")
                set_kind("$(kind)")
                add_includedirs("%s/c/src")
                add_headerfiles("%s/c/src/*.h")
                add_files("%s/c/src/*.c")
                set_fpmodels("precise")
                add_cxflags("-precise")
                add_cxflags("-fp:precise")
        ]], package:version(), package:version(), package:version()))
        local configs = {}
        if package:config("shared") then
            configs.kind = "shared"
        elseif not package:is_plat("windows", "mingw") and package:config("pic") ~= false then
            configs.cxflags = "-fPIC"
        end
        import("package.tools.xmake").install(package, configs)
    end)


