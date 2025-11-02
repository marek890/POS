cmake_minimum_required(VERSION 3.22)

project(
	POS_Suitcase
	DESCRIPTION "Suitcase code cracking"
	LANGUAGES C
)

add_executable(suitcases)
target_sources(suitcases PRIVATE main.c)
target_compile_options(suitcases PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
