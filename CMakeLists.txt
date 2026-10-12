cmake_minimum_required(VERSION 3.16)
project(MW3NativeScaffold VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_library(mw3_input_bridge STATIC
  app/Source/EngineBridge/EngineBridge.c
  app/Source/EngineBridge/EngineFeatures.c
)
target_include_directories(mw3_input_bridge
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/app/Source/EngineBridge"
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
endif()
