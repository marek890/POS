add_executable(commandsExample)
target_sources(commandsExample PRIVATE main.c command.c commandsQueue.c)
target_compile_options(commandsExample PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
