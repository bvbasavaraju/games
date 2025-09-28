project(shape_war)

set(SRC_FILES ./main.cpp)

add_executable(
    ${PROJECT_NAME} 
    ${SRC_FILES})

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/../../font/" DESTINATION "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/font")

target_include_directories(
    ${PROJECT_NAME} 
    PUBLIC 
    utils
    ecs
    ecs/components
    ecs/entity
    ecs/system
)

target_link_libraries(
    ${PROJECT_NAME} 
    PUBLIC SFML::Graphics SFML::Window SFML::System)
