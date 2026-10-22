cmake_minimum_required(VERSION 3.15)

project(Terminaldle LANGUAGES CXX)

add_executable(terminaldle src/main.cpp)
target_compile_features(terminaldle PRIVATE cxx_std_11)

if(MSVC)
    set_property(
        TARGET terminaldle
        PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

foreach(data_file intro.txt wordlist.txt keywordlist.txt)
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
        src/keywordlist.txt
    DESTINATION .
)