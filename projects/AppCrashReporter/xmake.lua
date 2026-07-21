target("AppCrashReporter")
    add_rules("ast.qt.widgetapp")
    add_files("**.cpp")
    add_files("**.hpp")
    add_deps("AstFault")
    add_frameworks("QtWidgets", "QtGui", "QtCore", "QtNetwork")
    if not has_package("qt") then
        set_enabled(false)
    end