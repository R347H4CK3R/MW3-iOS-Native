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
endif()
