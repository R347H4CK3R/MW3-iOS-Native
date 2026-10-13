cmake_minimum_required(VERSION 3.16)
project(MW3NativeScaffold VERSION 0.3.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(mw3_input_bridge STATIC
  app/Source/EngineBridge/EngineBridge.c
  app/Source/EngineBridge/EngineFeatures.c
)
target_include_directories(mw3_input_bridge
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/app/Source/EngineBridge"
)

add_library(mw3_d3d9_state STATIC
  app/Source/MetalRenderer/D3D9StateBridge.c
)
target_include_directories(mw3_d3d9_state
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/app/Source/MetalRenderer"
)

add_library(mw3_host_clock STATIC
  app/Source/HostRuntime/HostClock.c
)
target_include_directories(mw3_host_clock
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/app/Source/HostRuntime"
)

find_package(ZLIB REQUIRED)
add_library(mw3_iwd_reader STATIC
  app/Source/AssetRuntime/IWDReader.c
)
target_include_directories(mw3_iwd_reader
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/app/Source/AssetRuntime"
)
target_link_libraries(mw3_iwd_reader PUBLIC ZLIB::ZLIB)

include(CTest)
if(BUILD_TESTING)
  add_executable(test_input_bridge
    app/Source/EngineBridge/Tests/test_engine_bridge.c
  )
  target_link_libraries(test_input_bridge PRIVATE mw3_input_bridge)
  add_test(NAME input_bridge COMMAND test_input_bridge)

  add_executable(test_engine_features
    app/Source/EngineBridge/Tests/test_engine_features.c
  )
  target_link_libraries(test_engine_features PRIVATE mw3_input_bridge)
  add_test(NAME engine_features COMMAND test_engine_features)

  add_executable(test_d3d9_state_bridge
    app/Source/MetalRenderer/Tests/test_d3d9_state_bridge.c
  )
  target_link_libraries(test_d3d9_state_bridge PRIVATE mw3_d3d9_state)
  add_test(NAME d3d9_state_bridge COMMAND test_d3d9_state_bridge)

  add_executable(test_host_clock
    app/Source/HostRuntime/Tests/test_host_clock.c
  )
  target_link_libraries(test_host_clock PRIVATE mw3_host_clock)
  add_test(NAME host_clock COMMAND test_host_clock)

  find_package(Python3 COMPONENTS Interpreter REQUIRED)
  add_executable(test_iwd_reader
    app/Source/AssetRuntime/Tests/test_iwd_reader.c
  )
  target_link_libraries(test_iwd_reader PRIVATE mw3_iwd_reader)
  add_test(
    NAME create_synthetic_iwd
    COMMAND "${Python3_EXECUTABLE}"
      "${CMAKE_CURRENT_SOURCE_DIR}/scripts/make_iwd_fixture.py"
      "${CMAKE_CURRENT_BINARY_DIR}/iwd_fixtures"
  )
  set_tests_properties(create_synthetic_iwd PROPERTIES FIXTURES_SETUP iwd_fixture)
  add_test(NAME iwd_reader COMMAND test_iwd_reader
    "${CMAKE_CURRENT_BINARY_DIR}/iwd_fixtures/valid.iwd"
    "${CMAKE_CURRENT_BINARY_DIR}/iwd_fixtures/not_zip.iwd"
    "${CMAKE_CURRENT_BINARY_DIR}/iwd_fixtures/unsafe.iwd"
  )
  set_tests_properties(iwd_reader PROPERTIES FIXTURES_REQUIRED iwd_fixture)

  # Release defaults to -DNDEBUG, which would otherwise disable assert-based
  # unit checks. Force assertions on for every C regression executable.
  if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    foreach(test_target IN ITEMS
      test_input_bridge test_engine_features test_d3d9_state_bridge
      test_host_clock test_iwd_reader)
      target_compile_options("${test_target}" PRIVATE -UNDEBUG)
    endforeach()
  endif()
endif()
