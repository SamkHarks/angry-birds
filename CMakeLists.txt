cmake_minimum_required(VERSION 3.16)
project(AngryBirdsGame LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
# Add SFML
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)

# Add Box2D
FetchContent_Declare(Box2d
    GIT_REPOSITORY https://github.com/erincatto/box2d.git
    GIT_TAG v2.4.1)
FetchContent_MakeAvailable(Box2d)

# Add nlohmann/json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

file(GLOB SOURCES src/*.cpp)

add_executable(AngryBirds ${SOURCES})
target_link_libraries(AngryBirds PRIVATE sfml-graphics sfml-audio box2d nlohmann_json::nlohmann_json)
target_compile_features(AngryBirds PRIVATE cxx_std_20)

# Copy assets directory to build directory
add_custom_command(TARGET AngryBirds POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:AngryBirds>/assets)

if(WIN32)
    add_custom_command(
        TARGET AngryBirds
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:AngryBirds>
        VERBATIM)
endif()

install(TARGETS AngryBirds)
