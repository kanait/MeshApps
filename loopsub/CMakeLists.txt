cmake_minimum_required(VERSION 3.13)
project( loopsub )

if(UNIX)
        find_package(GLFW REQUIRED)
        find_package(GLEW REQUIRED)
endif(UNIX)
find_package(OpenGL REQUIRED)

# for Linux
if(UNIX AND NOT APPLE)
        set(OpenGL_GL_PREFERENCE GLVND)
endif()

add_executable( ${PROJECT_NAME}
                main.cc
                LoopSubL.hxx
                )

if(UNIX)
        target_include_directories( ${PROJECT_NAME}
                                    PRIVATE
                                    ${OPENGL_INCLUDE_DIR}
                                    ${GLEW_INCLUDE_DIR}
                                    ${GLFW_INCLUDE_DIR}
                                    ${PROJECT_SOURCE_DIR}/../render_Eigen
                                    ${PROJECT_SOURCE_DIR}/../eigen
                                    ${PROJECT_SOURCE_DIR}/../util
                                    ${PROJECT_SOURCE_DIR}/../meshL
                                    )

        target_compile_features( ${PROJECT_NAME}
                                 PRIVATE
                                 cxx_std_11
                                 )

        target_link_libraries( ${PROJECT_NAME}
                               PRIVATE
                               ${OPENGL_LIBRARIES}
                               GLEW::GLEW
                               ${GLFW_LIBRARY}
                               )
endif()
