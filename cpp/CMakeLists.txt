cmake_minimum_required(VERSION 3.20)
project(dhquant VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(POLICY CMP0148)
    cmake_policy(SET CMP0148 NEW)
endif()

set(PYBIND11_FINDPYTHON ON)
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 REQUIRED)
find_package(GTest REQUIRED)
include(CTest)

# 核心库
add_library(dhquant_core STATIC
    src/engine.cpp
    src/core/event_loop.cpp
    src/core/clock.cpp
    src/core/dispatcher.cpp
    src/core/handler.cpp
    src/core/journal.cpp
    src/core/replay.cpp
    src/core/shutdown_hook.cpp
    src/core/backtest_replay_reader.cpp
    src/core/python_bridge_handler.cpp
    src/core/backtest_matcher.cpp
    src/domain/domain.cpp
    src/utils/Log.cpp
)

target_include_directories(dhquant_core 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(dhquant_core 
    PUBLIC
)

# pybind11 模块
pybind11_add_module(dhquant_cpp_binding
    src/binding.cpp
)
target_link_libraries(dhquant_cpp_binding PRIVATE dhquant_core)

if(BUILD_TESTING)
    add_executable(dhquant_core_tests
        test/unittest/test_result.cpp
        test/unittest/test_log.cpp
        test/unittest/test_core.cpp
    )
    target_link_libraries(dhquant_core_tests
        PRIVATE
            dhquant_core
            GTest::gtest_main
            GTest::gmock
    )
    include(GoogleTest)
    gtest_discover_tests(dhquant_core_tests)
endif()

# 安装配置 (对接 scikit-build-core)
install(TARGETS dhquant_cpp_binding
    DESTINATION dhquant/core
    COMPONENT python_runtime
)
