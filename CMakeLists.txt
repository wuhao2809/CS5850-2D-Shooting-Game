cmake_minimum_required(VERSION 3.10)
project(GameEngineResourceManager)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add FetchContent for downloading dependencies
include(FetchContent)

# Fetch and configure nlohmann/json
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Add option to control test building
option(BUILD_TESTS "Build the test targets" OFF)

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Platform-specific SDL3 configuration
if(APPLE)
    # macOS specific SDL3 paths
    set(SDL3_DIR "/opt/homebrew/opt/sdl3")
    set(SDL3_TTF_DIR "/opt/homebrew/opt/sdl3_ttf")
    set(SDL3_IMAGE_DIR "/opt/homebrew/opt/sdl3_image")
    
    set(SDL3_INCLUDE_DIRS 
        ${SDL3_DIR}/include
        ${SDL3_TTF_DIR}/include
        ${SDL3_IMAGE_DIR}/include
    )
    
    set(SDL3_LIBRARIES
        ${SDL3_DIR}/lib/libSDL3.dylib
        ${SDL3_TTF_DIR}/lib/libSDL3_ttf.dylib
        ${SDL3_IMAGE_DIR}/lib/libSDL3_image.dylib
    )
else()
    # Windows/Linux SDL3 configuration
    find_package(SDL3 REQUIRED)
    find_package(SDL3_ttf)
    find_package(SDL3_image REQUIRED)

    if(SDL3_ttf_FOUND)
        add_compile_definitions(USE_SDL3_TTF)
    endif()
endif()

# Add source files
file(GLOB_RECURSE SOURCES 
    "src/game/events/*.cpp"
    "src/game/ecs/*.cpp"
    "src/game/resources/*.cpp"
    "src/game/ui/*.cpp"
    "src/game/*.cpp"
)
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Create the main library
add_library(game_ecs ${SOURCES})
target_include_directories(game_ecs PUBLIC 
    src
    ${SDL3_INCLUDE_DIRS}
    ${CMAKE_BINARY_DIR}/_deps/json-src/include
)

if(APPLE)
    target_link_libraries(game_ecs PRIVATE 
        ${SDL3_LIBRARIES}
        nlohmann_json::nlohmann_json
    )
else()
    target_link_libraries(game_ecs PRIVATE 
        SDL3::SDL3
        SDL3_image::SDL3_image
        nlohmann_json::nlohmann_json
    )
    
    if(SDL3_ttf_FOUND)
        target_link_libraries(game_ecs PRIVATE SDL3_ttf::SDL3_ttf)
    endif()
endif()

# Create the main executable
add_executable(GameEngine src/main/main.cpp)
target_include_directories(GameEngine PRIVATE 
    src
    ${CMAKE_BINARY_DIR}/_deps/json-src/include
)
target_link_libraries(GameEngine PRIVATE game_ecs)

# Build tests if enabled
if(BUILD_TESTS)
    # Find GTest package
    find_package(GTest REQUIRED)
    
    # Get list of test source files
    file(GLOB TEST_SOURCES "src/test/test_*.cpp" "src/test/*Test.cpp")

    # Create individual test executables
    foreach(TEST_SOURCE ${TEST_SOURCES})
        # Get the test name from the filename (without extension)
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        
        # Create test executable
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        
        # Link libraries
        target_link_libraries(${TEST_NAME} PRIVATE 
            game_ecs
            GTest::GTest 
            GTest::Main)
            
        target_include_directories(${TEST_NAME} PRIVATE src)
        
        # Add to CTest
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()

    # Enable testing
    enable_testing()
endif()

# Copy assets to build directory
file(COPY ${CMAKE_SOURCE_DIR}/GameAssets DESTINATION ${CMAKE_BINARY_DIR}) 