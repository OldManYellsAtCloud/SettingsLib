cmake_minimum_required(VERSION 3.14)

project(SettingsLib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)




add_library(SettingsLib SHARED
  src/settingslib.cpp
  include/settingslib.h
)


set_target_properties(SettingsLib PROPERTIES VERSION "1.0")
set_target_properties(SettingsLib PROPERTIES SOVERSION "1")
set_target_properties(SettingsLib PROPERTIES PUBLIC_HEADER include/settingslib.h)

target_compile_definitions(SettingsLib PRIVATE SETTINGSLIB_LIBRARY)

target_include_directories(SettingsLib PUBLIC "include")

include(GNUInstallDirs)
install(TARGETS SettingsLib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

option(RUN_TESTS "Compile and run tests" OFF)

if (RUN_TESTS)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
#set(GTEST_LIBRARY )
#find_package(GTest REQUIRED)

add_executable(settings_test tests/settings_test.cpp)
target_link_libraries(settings_test
                      GTest::gtest_main
                      SettingsLib)
include(GoogleTest)
gtest_discover_tests(settings_test)

endif()
