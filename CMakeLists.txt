cmake_minimum_required(VERSION 3.15)
project(FPSMonitorOverlay VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Windows-specific settings
if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DNOMINMAX)
endif()

# Source files
set(CORE_SOURCES
    src/core/fps_calculator.cpp
    src/core/drop_detector.cpp
    src/core/stats_tracker.cpp
    src/core/config.cpp
)

set(CORE_HEADERS
    src/core/fps_calculator.h
    src/core/ring_buffer.h
    src/core/drop_detector.h
    src/core/stats_tracker.h
    src/core/config.h
)

set(OVERLAY_SOURCES
    src/overlay/window_manager.cpp
    src/overlay/d2d_renderer.cpp
    src/overlay/graph_renderer.cpp
    src/overlay/text_renderer.cpp
    src/overlay/theme_manager.cpp
)

set(OVERLAY_HEADERS
    src/overlay/window_manager.h
    src/overlay/d2d_renderer.h
    src/overlay/graph_renderer.h
    src/overlay/text_renderer.h
    src/overlay/theme_manager.h
)

set(DETECTION_SOURCES
    src/detection/game_detector.cpp
    src/detection/window_tracker.cpp
)

set(DETECTION_HEADERS
    src/detection/game_detector.h
    src/detection/window_tracker.h
)

set(UTILS_SOURCES
    src/utils/timer.cpp
    src/utils/logger.cpp
)

set(UTILS_HEADERS
    src/utils/timer.h
    src/utils/logger.h
)

set(MAIN_SOURCE
    src/main.cpp
)

# Combine all sources
set(ALL_SOURCES
    ${CORE_SOURCES}
    ${OVERLAY_SOURCES}
    ${DETECTION_SOURCES}
    ${UTILS_SOURCES}
    ${MAIN_SOURCE}
)

set(ALL_HEADERS
    ${CORE_HEADERS}
    ${OVERLAY_HEADERS}
    ${DETECTION_HEADERS}
    ${UTILS_HEADERS}
)

# Create executable
if(WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${ALL_SOURCES} ${ALL_HEADERS})
else()
    add_executable(${PROJECT_NAME} ${ALL_SOURCES} ${ALL_HEADERS})
endif()

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/overlay
    ${CMAKE_SOURCE_DIR}/src/detection
    ${CMAKE_SOURCE_DIR}/src/utils
)

# Link Windows libraries
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        d2d1.lib
        dwrite.lib
        dxguid.lib
        windowscodecs.lib
        dwmapi.lib
        user32.lib
        gdi32.lib
        kernel32.lib
    )
endif()

# Compiler-specific options
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        $<$<CONFIG:Debug>:/WX>
        $<$<CONFIG:Release>:/O2 /GL>
    )
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS_RELEASE "/LTCG /SUBSYSTEM:WINDOWS"
        LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS"
    )
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        OUTPUT_NAME "fps-monitor-overlay"
    )
else()
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wextra -pedantic
        $<$<CONFIG:Release>:-O3>
    )
endif()

# Copy resources to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/resources
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources
    COMMENT "Copying resources to build directory"
)

# Copy config example
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/config.ini.example
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/config.ini.example
    COMMENT "Copying config.ini.example to build directory"
)

# Installation rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY resources/
    DESTINATION bin/resources
)

install(FILES config.ini.example
    DESTINATION bin
)