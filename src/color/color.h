/*
MIT License

Copyright (c) 2022 Lukas Pfeifer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#define CYAN(text)                  "\033[1;36m" text "\033[0m"
#define BLUE(text)                  "\033[1;34m" text "\033[0m"
#define YELLOW(text)                "\033[1;33m" text "\033[0m"
#define RED(text)                   "\033[1;31m" text "\033[0m"
#define GREEN(text)                 "\033[1;32m" text "\033[0m"
#define GREEN_REGULAR(text)         "\033[0;32m" text "\033[0m"
#define BRIGHT_GREEN(text)          "\033[1;92m" text "\033[0m"
#define MAGENTA(text)               "\033[1;35m" text "\033[0m"
#define MAGENTA_FAINT_UNDL(text)    "\033[2;35m\033[4;35m" text "\033[0m"