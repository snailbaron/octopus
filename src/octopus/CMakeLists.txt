set(ASSETS_DIR "${PROJECT_SOURCE_DIR}/assets")
configure_file(build-info.hpp.in include/build-info.hpp @ONLY)

add_executable(octopus
    main.cpp
    random.cpp
    task.cpp
    world.cpp
 "scene.cpp" "ai.cpp")
target_link_libraries(octopus PRIVATE sdl)
target_include_directories(octopus PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/include")

add_custom_command(TARGET octopus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        -t $<TARGET_FILE_DIR:octopus> $<TARGET_RUNTIME_DLLS:octopus>
    COMMAND_EXPAND_LISTS
)