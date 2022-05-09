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

#include <stdio.h>

#include "ninac.h"
#include "args/args.h"
#include "help/help.h"

#include "../color/color.h"
#include "../storage/storage.h"

int ninac_run(int argc, byte_t** argv)
{
    args_t arguments = { 0 };

    const byte_t* err = NULL;
    int parsed = args_parse(argc, argv, &arguments, &err);

    if (parsed != 0)
    {
        if (err != NULL)
        {
            printf(RED("ERR: ") "%s\n", err);
        }

        help_print();

        return EXIT_FAILURE;
    }

    if (arguments.show_help == 1)
    {
        help_print();
        return EXIT_SUCCESS;
    }

    switch (arguments.command)
    {

    case ADD_TODO:
    {
        const byte_t* add_err_msg = NULL;

        STORAGE_ERR_CODE add_err = storage_new_todo(arguments.title, NULL, &add_err_msg);

        if (add_err != STORAGE_NO_ERROR)
        {
            printf(RED("ERR: ") "%s\n", add_err_msg);
            return EXIT_FAILURE;
        }

        break;
    }

    case ERASE:
    {
        const byte_t* erase_err_msg = NULL;

        STORAGE_ERR_CODE add_err = storage_erase(&erase_err_msg);

        if (add_err != STORAGE_NO_ERROR)
        {
            printf(RED("ERR: ") "%s\n", erase_err_msg);
            return EXIT_FAILURE;
        }

        break;
    }

    default:
        printf(RED("ERR: ") "Please provide a valid command.\n");
        help_print();
        return EXIT_FAILURE;
    }

    args_free(&arguments);

    return EXIT_SUCCESS;
}