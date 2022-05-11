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

#include <wchar.h>
#include <stdbool.h>

#include "../types/types.h"

#define CHECK_MARK "\u2705"
#define CROSS_MARK "\u2718"

/**
 * @brief Converts the given src byte_t* to wchar_t* dest.
 *
 * @param src Multibyte string.
 * @param dest Wchar_t string.
 * @param max_dest Max number of wchar_t in dest.
 * @return true Conversion was successfull.
 * @return false Conversion failed.
 */
bool bstows(const byte_t* src, wchar_t* dest, size_t max_dest);

/**
 * @brief Converts the given src wchar_t* to byte_t* dest.
 *
 * @param src Wchar_t string.
 * @param dest Multibyte string.
 * @param max_dest Max number of byte_t in dest.
 * @return true Conversion was successfull.
 * @return false Conversion failed.
 */
bool wstobs(const wchar_t* src, byte_t* dest, size_t max_dest);