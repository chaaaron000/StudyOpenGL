cmake_minimum_required(VERSION 3.13)

set(PROJECT_NAME StudyOpenGL)
set(CMAKE_CXX_STANDARD 17)

set(WINDOW_NAME "Study OpenGL")
set(WINDOW_WIDTH 640)
set(WINDOW_HEIGHT 480)

project(${PROJECT_NAME})

add_executable(${PROJECT_NAME} src/main.cpp
        src/common.h src/common.cpp
        src/shader.h src/shader.cpp
        src/program.h src/program.cpp
        src/context.h src/context.cpp
        src/buffer.h src/buffer.cpp
        src/vertex_layout.h src/vertex_layout.cpp
        src/image.h src/image.cpp
        src/texture.h src/texture.cpp
)

find_package(spdlog CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(imgui CONFIG REQUIRED)

#include(Dependency.cmake)

# 우리 프로젝트에 include / lib 관련 옵션 추가
#target_include_directories(${PROJECT_NAME} PUBLIC ${DEP_INCLUDE_DIR})
#target_link_directories(${PROJECT_NAME} PUBLIC ${DEP_LIB_DIR})
#target_link_libraries(${PROJECT_NAME} PUBLIC ${DEP_LIBS})

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        glfw
        glad::glad
        glm::glm
        spdlog::spdlog
        imgui::imgui
)

target_include_directories(${PROJECT_NAME}
        PUBLIC
        ${Stb_INCLUDE_DIR}
)

target_compile_definitions(${PROJECT_NAME} PUBLIC
        WINDOW_NAME="${WINDOW_NAME}"
        WINDOW_WIDTH=${WINDOW_WIDTH}
        WINDOW_HEIGHT=${WINDOW_HEIGHT}
)

# Dependency들이 먼저 build 될 수 있게 관계 설정
#add_dependencies(${PROJECT_NAME} ${DEP_LIST})

if (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /utf-8)
endif()