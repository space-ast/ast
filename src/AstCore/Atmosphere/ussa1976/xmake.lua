target("ussa1976")
    set_kind("static")
    add_files("*.f90")
    
    set_toolchains("ifort")     -- don't use gfortran
    add_fcflags("/Qautodouble")  -- set REAL to double by default
    add_fcflags("/auto")  -- do not generate static vars in subroutings
    --add_fcflags("/Qauto-scalar")
    add_fcflags("/zero")  -- init with zero