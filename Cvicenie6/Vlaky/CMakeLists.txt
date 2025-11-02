cmake_minimum_required(VERSION 3.22)

project(
	POS_Vlaky
	DESCRIPTION "Simulating trains using threads"
	LANGUAGES C
)

add_executable(vlaky)
target_sources(vlaky PRIVATE main.c)
target_compile_options(vlaky PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
