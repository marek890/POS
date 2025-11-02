cmake_minimum_required(VERSION 3.22)

project(
	POS_Triatlon
	DESCRIPTION "Simulating Triatlon using threads"
	LANGUAGES C
)

add_executable(triatlon)
target_sources(triatlon PRIVATE main.c)
target_compile_options(triatlon PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
