cmake_minimum_required(VERSION 3.28)

project(opengl_class LANGUAGES CXX)

find_package(OpenGL REQUIRED)

enable_language(C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Pega automaticamente todos os .cpp da pasta src
file(GLOB_RECURSE files "src/*.cpp")

# Cria o executável principal
add_executable(${PROJECT_NAME} ${files})

# Adiciona o glad
add_library(glad STATIC dependencies/glad/glad.c)

# Inclui diretórios para headers
target_include_directories(${PROJECT_NAME} PUBLIC include)

# Adiciona a glfw (pasta com CMakeLists próprio)
add_subdirectory(dependencies/glfw-3.4 glfw)

# Faz o link das bibliotecas
target_link_libraries(${PROJECT_NAME}
        OpenGL::GL
        glfw
        glad
)
