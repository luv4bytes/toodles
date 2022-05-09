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

#include <stdlib.h>
#include <stdio.h>
#include <locale.h>

#include "types/types.h"
#include "greeter/greeter.h"
#include "cli/cli.h"
#include "storage/storage.h"
#include "color/color.h"
#include "env/env.h"
#include "non_interactive/ninac.h"

static void print_byebye()
{
    printf(CYAN("Byyyeee!\n"));
}

/**
 * @brief Main routine.
 *
 * @param argc Argument counter.
 * @param argv Arguments.
 * @return int Exit code.
 */
int main(int argc, byte_t** argv)
{
    setlocale(LC_ALL, "");

    const byte_t* env_err = NULL;
    int env_ret = env_init(&env_err);

    if (env_ret != 0)
    {
        printf(RED("ERR: ") "%s\n", env_err);
        return EXIT_FAILURE;
    }

    const byte_t* storage_err = NULL;
    STORAGE_ERR_CODE si_ret = storage_init(&storage_err);

    if (si_ret == STORAGE_CRITICAL_ERROR)
    {
        printf(RED("ERR: ") "%s\n", storage_err);
        return EXIT_FAILURE;
    }

    const byte_t* err = NULL;
    STORAGE_ERR_CODE error = storage_new_storage(&err);

    if (error == STORAGE_CRITICAL_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return EXIT_FAILURE;
    }

    if (argc == 1)
    {
        atexit(print_byebye);
        greeter_hello();
        cli_prompt();
    }
    else
    {
        return ninac_run(argc, argv);
    }

    return EXIT_SUCCESS;
}