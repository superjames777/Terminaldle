cmake_minimum_required(VERSION 3.15)

project(Terminaldle LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)

add_executable(terminaldle src/main.cpp)
target_compile_features(terminaldle PRIVATE cxx_std_11)
target_link_libraries(terminaldle PRIVATE nlohmann_json::nlohmann_json)

if(MSVC)
    set_property(
        TARGET terminaldle
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

foreach(data_file intro.txt wordlist.txt answerlist.txt keywordlist.txt stats.json)
    add_custom_command(
        TARGET terminaldle
        POST_BUILD
        COMMAND
            ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/src/${data_file}"
            "$<TARGET_FILE_DIR:terminaldle>/${data_file}"
        VERBATIM
    )
endforeach()

install(TARGETS terminaldle RUNTIME DESTINATION .)
install(
    FILES
        src/intro.txt
        src/wordlist.txt
        src/answerlist.txt
        src/keywordlist.txt
        src/stats.json
    DESTINATION .
)