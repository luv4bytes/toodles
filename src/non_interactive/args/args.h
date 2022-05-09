/* MIT License

Copyright(c) 2022 Lukas Pfeifer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this softwareand associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE. */

#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "../../types/types.h"

/**
 * @brief Defines values for commands that can be executed.
 *
 */
typedef enum
{
    NONE,
    ADD_TODO,
    ERASE,

} ARGS_COMMANDS;

/**
 * @brief Defines arguments used by the application in non-interactive mode.
 *
 */
typedef struct
{
    /**
     * @brief Command identifier.
     *
     */
    ARGS_COMMANDS command;

    /**
     * @brief Title of a todo entry.
     *
     */
    const byte_t* title;

    /**
     * @brief Identifier for showing non-interactive help.
     *
     */
    bool show_help;

} args_t;

/**
 * @brief Initializes the given args.
 *
 * @param args Args that will be initialized.
 */
void args_init(args_t* args);

/**
 * @brief Frees the memory used by args.
 *
 * @param args Args to free.
 */
void args_free(args_t* args);

/**
 * @brief Parses the application arguments and stores them in args.
 *
 * @param argc Number of arguments.
 * @param argv Arguments.
 * @param args Structure to store the parsed arguments.
 * @param err Pointer to error message.
 *
 * @return Success indicator.
 *
 */
int args_parse(int argc, byte_t** argv, args_t* args, const byte_t** err);