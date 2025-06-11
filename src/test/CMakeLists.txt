cmake_minimum_required(VERSION 3.10)
project(GameTests)

# Find GTest package
find_package(GTest REQUIRED)

# Add test executables
add_executable(test_timer test_timer.cpp ../game/Timer.cpp)
add_executable(test_gameobject test_gameobject.cpp ../game/GameObject.cpp)

# Link against GTest and SDL3
target_link_libraries(test_timer 
    GTest::GTest 
    GTest::Main
    ${SDL3_LIBRARIES}
)

target_link_libraries(test_gameobject 
    GTest::GTest 
    GTest::Main
    ${SDL3_LIBRARIES}
)

# Add include directories
target_include_directories(test_timer PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${SDL3_INCLUDE_DIRS}
)

target_include_directories(test_gameobject PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${SDL3_INCLUDE_DIRS}
)

# Enable C++17
set_target_properties(test_timer PROPERTIES CXX_STANDARD 17)
set_target_properties(test_timer PROPERTIES CXX_STANDARD_REQUIRED ON)
set_target_properties(test_gameobject PROPERTIES CXX_STANDARD 17)
set_target_properties(test_gameobject PROPERTIES CXX_STANDARD_REQUIRED ON)

# Add tests to CTest
add_test(NAME TimerTest COMMAND test_timer)
add_test(NAME GameObjectTest COMMAND test_gameobject) 