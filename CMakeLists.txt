# CMakeLists.txt for AstPy Python bindings
#
# Usage:
#   mkdir build && cd build
#   cmake .. -DAST_BUILD_PYTHON=ON -DCMAKE_BUILD_TYPE=Release
#   cmake --build . --config Release
#   cmake --install .

cmake_minimum_required(VERSION 3.15)
project(AstPy VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find pybind11
find_package(pybind11 CONFIG REQUIRED)

# Find Python
find_package(Python COMPONENTS Interpreter Development REQUIRED)

# Find SpaceAST
find_package(AstCore CONFIG REQUIRED)
find_package(AstMath CONFIG REQUIRED)
find_package(AstUtil CONFIG REQUIRED)

# Print configuration
message(STATUS "AstPy Configuration:")
message(STATUS "  Python executable: ${Python_EXECUTABLE}")
message(STATUS "  Python include dir: ${Python_INCLUDE_DIRS}")
message(STATUS "  Python libraries: ${Python_LIBRARIES}")
message(STATUS "  Python version: ${Python_VERSION}")

# Define source files
set(ASTPY_SOURCES
    src/AstPy/module.cpp
    src/AstPy/py_time.cpp
    src/AstPy/py_math.cpp
    src/AstPy/py_orbit.cpp
    src/AstPy/py_frame.cpp
    src/AstPy/py_body.cpp
    src/AstPy/py_quantity.cpp
)

set(ASTPY_HEADERS
    src/AstPy/pybind11.h
    src/AstPy/py_time.h
    src/AstPy/py_math.h
    src/AstPy/py_orbit.h
    src/AstPy/py_frame.h
    src/AstPy/py_body.h
    src/AstPy/py_quantity.h
)

# Create Python extension module
pybind11_add_module(_ast MODULE ${ASTPY_SOURCES} ${ASTPY_HEADERS})

# Set module name (without prefix)
set_target_properties(_ast PROPERTIES PREFIX "")

# Include directories
target_include_directories(_ast PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/AstPy
    ${Python_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(_ast PRIVATE
    AstCore::AstCore
    AstMath::AstMath
    AstUtil::AstUtil
)

# Optimization flags
target_compile_options(_ast PRIVATE
    $<$<CONFIG:Release>:
        -O3
        -march=native
        -DNDEBUG
    >
)

# Define macros
target_compile_definitions(_ast PRIVATE
    PYBIND11_SMALL_ASSERTIONS
    AST_CORE_EXPORTS
)

# Installation
install(TARGETS _ast
    LIBRARY DESTINATION "${Python_SITEARCH}/astpy"
    RUNTIME DESTINATION "${Python_SITEARCH}/astpy"
)

install(FILES ${ASTPY_HEADERS}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/astpy"
)

# Create __init__.py placeholder
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/astpy/__init__.py" "
# AstPy - SpaceAST Python Bindings
from ._ast import *
__version__ = '1.0.0'
__all__ = dir()
")

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/astpy/__init__.py"
    DESTINATION "${Python_SITEARCH}/astpy"
)

# Testing
enable_testing()

# Copy example files
file(COPY examples/python/
    DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/"
    FILES_MATCHING PATTERN "*.py"
)

add_test(NAME TestImport
    COMMAND "${Python_EXECUTABLE}" "-c" "import astpy; print(astpy.__version__)"
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
)
