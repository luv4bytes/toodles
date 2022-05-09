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

#include <getopt.h>
#include <stdio.h>
#include <string.h>

#include "args.h"

void args_init(args_t* args)
{
    if (args == NULL)
    {
        return;
    }

    args->show_help = false;
    args->command = NONE;
    args->title = NULL;
}

void args_free(args_t* args)
{
    if (args == NULL)
    {
        return;
    }

    if (args->title != NULL)
    {
        free((byte_t*)args->title);
    }
}

static ARGS_COMMANDS parse_command_val(const byte_t* cmd)
{
    if (cmd == NULL)
    {
        return NONE;
    }

    if (strcmp(cmd, "add") == 0)
    {
        return ADD_TODO;
    }

    if (strcmp(cmd, "erase") == 0)
    {
        return ERASE;
    }

    return NONE;
}

int args_parse(int argc, byte_t** argv, args_t* args, const byte_t** err)
{
    if (argc == 1)
    {
        return -1;
    }

    const byte_t* opts = "c:t:h";

    byte_t c;
    while ((c = getopt(argc, argv, opts)) != -1)
    {
        switch (c)
        {
        case 'c':
            args->command = parse_command_val(optarg);
            break;

        case 't':
            args->title = strdup(optarg);
            break;

        case 'h':
            args->show_help = true;
            break;

        default:
            return -1;
        }
    }

    return 0;
}