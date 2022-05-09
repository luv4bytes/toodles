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

#include "../types/types.h"

/**
 * @brief Adds the given command to the history.
 *
 * @param command The command to add to the history.
 * @return int Success indicator.
 */
int history_insert(const byte_t* command);

/**
 * @brief Returns the command that is stored on the given index.
 *
 * @param index Index in the history.
 * @param err Pointer to error message.
 * @return const byte_t* Command as a string or NULL.
 */
const byte_t* history_get(int index, byte_t** err);

/**
 * @brief Prints the current history.
 *
 * @return int Success indicator.
 */
int history_print();