target("nrlmsise-00")
    add_files("*.c")
    add_files("*.cpp")

    remove_files("nrlmsise-00_test.c")
    add_includedirs(".")
    add_headerfiles("*.h")
    add_headerfiles("*.hpp")

    set_kind("static")