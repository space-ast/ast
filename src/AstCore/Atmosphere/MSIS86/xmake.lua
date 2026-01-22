target("MSIS86")
    set_kind("static")
    add_files("*.for")
    remove_files("m86tes.for")
    
    set_toolchains("ifort")     -- don't use gfortran
    add_fcflags("/Qautodouble")  -- set REAL to double by default
    add_fcflags("/auto")  -- do not generate static vars in subroutings
    --add_fcflags("/Qauto-scalar")
    add_fcflags("/zero")  -- init with zero