set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(sengine main.cpp)

target_link_libraries(sengine base)
set_target_properties(sengine PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

if (MSVC)
    target_link_options(sengine PRIVATE $<$<CONFIG:RELWITHDEBINFO>:/PROFILE>)
endif ()

#file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/build/shaders/)

file(COPY ../shaders DESTINATION ${CMAKE_BINARY_DIR}/build)