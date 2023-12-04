cmake_minimum_required(VERSION 3.14)

project(SettingsLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(SettingsLib SHARED
  settingslib.cpp
  settingslib.h
)

target_compile_definitions(SettingsLib PRIVATE SETTINGSLIB_LIBRARY)

include(GNUInstallDirs)
install(TARGETS SettingsLib
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
