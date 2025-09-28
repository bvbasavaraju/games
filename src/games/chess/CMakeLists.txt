project(chess)

set(SRC_FILES ./main.cpp)

add_executable(
    ${PROJECT_NAME} 
    ${SRC_FILES})

# target_include_directories(
#     ${PROJECT_NAME} 
#     PUBLIC ${PROJECT_SOURCE_DIR}/include
# )

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC SFML::Graphics SFML::Window SFML::System)
