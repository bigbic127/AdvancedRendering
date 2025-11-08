cmake_minimum_required(VERSION 3.19)
project(Rendering)
set(CMAKE_CXX_STANDARD 20)
#library
#glad
set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad)
set(GLAD_SRC 
    ${GLAD_DIR}/src/glad.c
)
add_library(glad STATIC ${GLAD_SRC})
target_include_directories(glad PUBLIC
    ${GLAD_DIR}/include
)
#git
include(FetchContent)
#glfw
FetchContent_Declare(
    GLFW
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW 1
    CMAKE_ARGS
        -DGLFW_BUILD_DOCS=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_EXAMPLES=OFF
)
FetchContent_MakeAvailable(GLFW)
#glm
FetchContent_Declare(
    GLM
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.2
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(GLM)
#imgui
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.92.4-docking
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(imgui)
set(IMGUI_SRC
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/imgui_internal.h
    ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    ${imgui_SOURCE_DIR}/imconfig.h
)
add_library(imguiLib STATIC ${IMGUI_SRC})
target_include_directories(imguiLib PUBLIC
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)
target_compile_definitions(imguiLib PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(imguiLib PUBLIC glfw)
#stb image
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
    GIT_SHALLOW 1
)
FetchContent_MakeAvailable(stb)
add_library(stbLib INTERFACE)
target_include_directories(stbLib INTERFACE
    ${stb_SOURCE_DIR}
)
#sourceCode
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)
add_executable(${PROJECT_NAME} ${SOURCES})
#build
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glad
        glfw
        glm
        imguiLib
        stbLib
        opengl32
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_SOURCE_DIR}/bin"
        OUTPUT_NAME_DEBUG   "${PROJECT_NAME}_debug"
        OUTPUT_NAME_RELEASE "${PROJECT_NAME}_release"
    )
    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
    endif()
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        glad
        glfw
        glm
        imguiLib
        stbLib
        "-framework Cocoa"
        "-framework IOKit"
        "-framework CoreVideo"
        "-framework OpenGL"
    )
    set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY   "${CMAKE_SOURCE_DIR}/bin"
        OUTPUT_NAME_DEBUG   "${PROJECT_NAME}_debug"
        OUTPUT_NAME_RELEASE "${PROJECT_NAME}_release"
        BUILD_RPATH "@executable_path"
        INSTALL_RPATH "@executable_path"
        RPATH_USE_LINK_PATH TRUE
    )
endif()
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core/public
    ${CMAKE_CURRENT_SOURCE_DIR}/src/renderer/public
    ${CMAKE_CURRENT_SOURCE_DIR}/src/component/public
    ${CMAKE_CURRENT_SOURCE_DIR}/src/resource/public
    ${CMAKE_CURRENT_SOURCE_DIR}/src/scene/public
    ${CMAKE_CURRENT_SOURCE_DIR}/src/system/
    )
#assetsCopy
set(assetDir "${CMAKE_SOURCE_DIR}/assets")
set(assetDir_output "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>")
add_custom_target(assetsCopy ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${assetDir}
            ${assetDir_output}
    COMMENT "Copying assets..."
)
#shaderCopy
set(SHADER_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/shader")
set(SHADER_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/bin/$<CONFIG>/shader")
add_custom_target(copy_shaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${SHADER_SOURCE_DIR}
            ${SHADER_OUTPUT_DIR}
    COMMENT "Copying shaders..."
)
