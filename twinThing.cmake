# Add library cpp files
add_library(twinThing INTERFACE)
target_sources(twinThing INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}/src/State.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/StateObserver.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/StateTemp.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Topic.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TopicGet.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TopicPing.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/Twin.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TwinAbstract.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TwinStdio.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TwinUart.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/TopicStats.cpp
)

# Add include directory
target_include_directories(twinThing INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src)

# Add the standard library to the build
target_link_libraries(twinThing INTERFACE pico_stdlib hardware_adc json_maker tiny_json)