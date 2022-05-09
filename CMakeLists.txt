cmake_minimum_required(VERSION 3.0.0)
project(toodles VERSION 1.0.0)

include(CTest)
enable_testing()

include(FindPkgConfig)
pkg_check_modules(LIBSQLITE sqlite3 REQUIRED)

add_compile_options(-Wall)
add_compile_definitions(VERSION="1.0.42-alpha")

add_executable(toodles src/main.c
                       src/cli/cli.c
                       src/cli/error.c
                       src/greeter/greeter.c
                       src/storage/storage.c
                       src/history/history.c
                       src/env/env.c
                       src/non_interactive/ninac.c
                       src/non_interactive/args/args.c
                       src/non_interactive/help/help.c)

target_link_libraries(toodles sqlite3)

INSTALL(TARGETS toodles RUNTIME DESTINATION bin)
