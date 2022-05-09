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

#include "help.h"

#include "../../color/color.h"

void help_print()
{
    printf("Following arguments can be given to toodles for non-interactive mode:\n");
    printf("\n");
    printf(MAGENTA("%-10s%-30s%-30s\n"), "Argument", "Synopsis", "Function");
    printf("\n");
    printf("%-10s"CYAN("%-30s")"%-30s\n", "-h", "", "Prints out help text for non-interactive mode.");
    printf("%-10s"CYAN("%-30s")"%-30s\n", "-c", "[COMMAND]", "Specifies the command to execute.");
    printf("%-10s"CYAN("%-30s")"%-30s\n", "-t", "[TITLE]", "Title for a todo entry.");
    printf("\n");
    printf(MAGENTA("COMMANDS")"\n");
    printf("\n");
    printf("%-10s%-30s\n", "add", "Adds a new todo entry.");
    printf("%-10s%-30s\n", "erase", "Erase all data that is stored in the toodles database.");
    printf("\n");
}