cmake_minimum_required(VERSION 3.5)
project(libsi24
    VERSION 0.30.0
    LANGUAGES C
)
set(PROJECT_DESCRIPTION "si24r1 library")
set(PROJECT_HOMEPAGE_URL https://github.com/robinkrens/libsi24)

add_compile_options(-Wall -Wextra -pedantic)

add_library(libsi24 libsi24.c)

target_include_directories(libsi24
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

include(GNUInstallDirs)

install(TARGETS libsi24
    EXPORT libsi24-export
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(FILES libsi24.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(EXPORT libsi24-export
    FILE libsi24.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/libsi24
)

include(CPack)
