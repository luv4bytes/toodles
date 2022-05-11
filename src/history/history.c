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
#include <string.h>
#include <stdio.h>

#include "history.h"

#include "../color/color.h"

#define HISTORY_SIZE 1024

static wchar_t* HISTORY[HISTORY_SIZE] = { 0 };
static size_t history_index = 0;

int history_insert(const wchar_t* command)
{
    if (!command)
        return 0;

    wchar_t* ins = wcsdup(command);

    if (history_index == HISTORY_SIZE - 1)
    {
        history_index = 0;
    }

    if (HISTORY[history_index] != NULL)
    {
        free(HISTORY[history_index]);
    }

    HISTORY[history_index++] = ins;

    return 1;
}

const wchar_t* history_get(int index, byte_t** err)
{
    if (index < 0)
    {
        if (err)
        {
            *err = "History index must be bigger than 0.";
        }

        return NULL;
    }

    if (index > HISTORY_SIZE - 1)
    {
        if (err)
        {
            *err = "History index must be smaller than 1024.";
        }

        return NULL;
    }

    return HISTORY[index];
}

int history_print()
{
    for (size_t i = 0; i < HISTORY_SIZE; i++)
    {
        if (HISTORY[i] == NULL)
        {
            continue;
        }

        printf(CYAN("[%zu]") " %ls\n", i, HISTORY[i]);
    }

    return 1;
}