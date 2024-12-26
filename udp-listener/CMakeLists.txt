cmake_minimum_required(VERSION 3.5)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(udp_listener C)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

aux_source_directory(src APP_SRC_FILES)

add_executable(udp_listener ${APP_SRC_FILES})

target_include_directories(udp_listener PRIVATE inc)

target_compile_options(udp_listener PRIVATE
	"-fdiagnostics-color=always"
	"-Wall" "-Wextra" "-Wpedantic"
	# "-fsanitize=address,undefined"
	"-pipe"
	"$<$<CONFIG:DEBUG>:-O0;-g3;-ggdb>"
	"$<$<CONFIG:RELEASE>:-O2;-g0>"
)

target_compile_definitions(udp_listener PRIVATE
	$<$<CONFIG:DEBUG>:DEBUG>
)
