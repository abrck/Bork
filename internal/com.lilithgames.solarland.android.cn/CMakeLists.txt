add_library(Solarland SHARED
    entry.cpp
    aimass.cpp
    And64InlineHook.cpp
)

target_include_directories(Solarland PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Solarland log)