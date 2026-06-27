cmake_minimum_required(VERSION 3.20)
project(haversine LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(haversine src/main.cpp
    src/math/haversine.hpp
    src/math/haversine.cpp
    src/fileGen/generator.hpp
    src/fileGen/generator.cpp
    src/fileGen/files.hpp
    src/fileGen/files.cpp
    src/parser/lookup.hpp
    src/profiler/



    src/parser/enums.hpp
    src/parser/charSet.hpp
    src/parser/tokenize.hpp
    src/parser/tokenize.cpp
    src/parser/parser.hpp
    src/parser/parser.cpp
    src/profiler/profiler.hpp
    src/profiler/profiler.cpp
)

target_include_directories(haversine PRIVATE
    src/
)

include(GNUInstallDirs)
install(TARGETS haversine
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
