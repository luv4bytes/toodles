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
#include <stdbool.h>
#include <sys/stat.h>

#include "env.h"

#define APP_DIR_NAME ".toodles"

#define ERR_HOME_NOT_FOUND "HOME environment variable not set."

/**
 * @brief Application directory.
 *
 */
static byte_t* application_dir = NULL;

/**
 * @brief Flag for checking if the initializer function was called.
 *
 */
static bool initialized = false;

int env_init(const byte_t** err)
{
    if (initialized == true)
    {
        return 0;
    }

    byte_t* home = getenv("HOME");

    if (home == NULL)
    {
        if (err)
        {
            *err = ERR_HOME_NOT_FOUND;
        }

        return 1;
    }

    size_t appdir_len = strlen(home) + strlen("/") + strlen(APP_DIR_NAME) + strlen("/") + 1;
    byte_t* appdir = calloc(appdir_len, sizeof(byte_t));

    strcat(appdir, home);
    strcat(appdir, "/");
    strcat(appdir, APP_DIR_NAME);
    strcat(appdir, "/");

    application_dir = appdir;

    mkdir(appdir, S_IRWXU | S_IRWXG);

    initialized = true;

    return 0;
}

const byte_t* env_app_dir()
{
    return application_dir;
}