cmake_minimum_required(VERSION 3.23)

project(
	POS_Vlaky
	LANGUAGES C
	)

add_executable(vlaky)
target_sources(vlaky PRIVATE main.c)

