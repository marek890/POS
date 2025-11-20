add_executable(commandsExample)
target_sources(commandsExample PRIVATE commandsExample.c command.c commandsQueue.c commandsGenerator.c)
target_compile_options(commandsExample PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")

add_executable(fileAnalyser)
target_sources(fileAnalyser PRIVATE fileAnalyser.c command.c commandsGenerator.c)
target_compile_options(fileAnalyser PRIVATE "-std=c11" "-Wall" "-Wextra" "-Wpedantic")
