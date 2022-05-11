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
#include <stdbool.h>

#define __USE_GNU
#include <string.h>

#include <sqlite3.h>
#include <errno.h>
#include <assert.h>

#include "storage.h"

#include "../color/color.h"
#include "../env/env.h"

#define STORAGE_FILE_NAME "toodles.sqlite"

/**
 * @brief Full path to the storage file
 *
 */
static byte_t* storage_file_path = NULL;

/**
 * @brief Flag for checking if the initializer function was called.
 *
 */
static bool initialized = false;

/**
 * @brief The used handle for sqlite3.
 *
 */
static sqlite3* sqlite_handle;

/**
 * @brief Defines an assignment of option to str.
 *
 */
typedef struct
{
    STORAGE_PRINT_OPTIONS option;
    const wchar_t* str;

} storage_print_option_t;

static const storage_print_option_t OPTIONS[] = {

    {
        .option = ALL,
        .str = L"all"
    },
    {
        .option = DONE,
        .str = L"done"
    },
    {
        .option = OPEN,
        .str = L"open"
    }
};

STORAGE_PRINT_OPTIONS storage_str_to_option(const wchar_t* option)
{
    if (option == NULL)
    {
        return ALL;
    }

    size_t len = sizeof(OPTIONS) / sizeof(OPTIONS[0]);

    for (size_t i = 0; i < len; i++)
    {
        if (wcscmp(option, OPTIONS[i].str) == 0)
        {
            return OPTIONS[i].option;
        }
    }

    return ALL;
}

STORAGE_ERR_CODE storage_init(const byte_t** err)
{
    if (initialized == true)
    {
        return STORAGE_NO_ERROR;
    }

    const byte_t* appdir = env_app_dir();

    size_t storage_file_len = strlen(appdir) + strlen(STORAGE_FILE_NAME) + 1;

    storage_file_path = calloc(storage_file_len, sizeof(byte_t));

    strcat(storage_file_path, appdir);
    strcat(storage_file_path, STORAGE_FILE_NAME);

    initialized = true;

    return STORAGE_NO_ERROR;
}

/**
 * @brief Creates the basic todo table.
 *
 * @return int SQLITE result code.
 */
static int storage_create_todo_table()
{
    const byte_t* sql = "create table if not exists "
        "TODOS ("
        "ID INTEGER"
        ",TITLE TEXT"
        ",DETAILS TEXT"
        ",DONE INTEGER NOT NULL DEFAULT 0 CHECK(DONE = 0 or DONE = 1)"
        ",CREATED DATE DEFAULT (datetime('now', 'localtime'))"
        ",primary key(ID autoincrement))";

    int result = sqlite3_exec(sqlite_handle, sql, NULL, NULL, NULL);

    return result;
}

/**
 * @brief Creates the todo attachment table.
 *
 * @return int SQLITE result code.
 */
static int storage_create_attachment_table()
{
    const byte_t* sql = "create table if not exists "
        "ATTACHMENTS ("
        "ID INTEGER, "
        "NAME TEXT NOT NULL, "
        "TODO_ID INTEGER NOT NULL, "
        "ATTACHMENT BLOB NOT NULL, "
        "SIZE INTEGER NOT NULL, "
        "primary key(ID autoincrement), "
        "foreign key(TODO_ID) references TODOS(ID))";

    int result = sqlite3_exec(sqlite_handle, sql, NULL, NULL, NULL);

    return result;
}

STORAGE_ERR_CODE storage_new_storage(const byte_t** err)
{
    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_CRITICAL_ERROR;
    }

    result = storage_create_todo_table();

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_CRITICAL_ERROR;
    }

    result = storage_create_attachment_table();

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_CRITICAL_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_CRITICAL_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_new_todo(const byte_t* title, const byte_t* details, const byte_t** err)
{
    if (!title || title[0] == 0)
    {
        if (err)
        {
            *err = "Please provide a title.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "insert into TODOS (TITLE, DETAILS) values (?, ?)";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, title, strlen(title), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 2, details, details == NULL ? 0 : strlen(details), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    int rc = sqlite3_step(statement);

    if (rc != SQLITE_DONE)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

static int print_todo(void* arg, int column_count, byte_t** values, byte_t** columns)
{
    byte_t* id = values[0];
    byte_t* title = values[1];
    byte_t* done = values[2];
    byte_t* created = values[3] == NULL ? "" : values[3];

    int done_i = atoi(done);

    printf(CYAN("%-16s")"%-64s%-16s%-24s\n", id, title, done_i == 0 ? CROSS_MARK : CHECK_MARK, created);

    return 0;
}

STORAGE_ERR_CODE storage_print_todos(STORAGE_PRINT_OPTIONS option, const byte_t** err)
{
    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    printf(MAGENTA("%-16s%-64s%-16s%-16s\n"), "Id", "Title", "Done", "Created");

    const byte_t* select = "select ID, TITLE, DONE, CREATED from TODOS ";
    const byte_t* where = "";

    switch (option)
    {
    case ALL:
        break;
    case DONE:
        where = "where DONE = 1";
        break;
    case OPEN:
        where = "where DONE = 0";
        break;
    }

    size_t select_len = strlen(select) + 1;
    size_t where_len = strlen(where) + 1;

    char sql[select_len + where_len];
    memset(sql, 0, (select_len + where_len) * sizeof(char));

    strcat(sql, select);
    strcat(sql, where);

    result = sqlite3_exec(sqlite_handle, sql, print_todo, NULL, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_erase(const byte_t** err)
{
    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "delete from TODOS";

    result = sqlite3_exec(sqlite_handle, sql, NULL, NULL, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    sql = "update sqlite_sequence set seq = 0 where name = 'TODOS'";

    result = sqlite3_exec(sqlite_handle, sql, NULL, NULL, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    sql = "delete from ATTACHMENTS";

    result = sqlite3_exec(sqlite_handle, sql, NULL, NULL, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    sql = "update sqlite_sequence set seq = 0 where name = 'ATTACHMENTS'";

    result = sqlite3_exec(sqlite_handle, sql, NULL, NULL, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_print_search_results(const byte_t* search_str, const byte_t** err)
{
    if (!search_str || search_str[0] == 0)
    {
        search_str = "%";
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    printf(MAGENTA("%-16s%-64s%-16s%-16s\n"), "Id", "Title", "Done", "Created");

    const byte_t* sql = "select ID, TITLE, DONE, CREATED from TODOS where TITLE like ?";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    byte_t* search = calloc(strlen(search_str) + 3, sizeof(char));
    strcat(search, "%");
    strcat(search, search_str);
    strcat(search, "%");

    result = sqlite3_bind_text(statement, 1, search, strlen(search), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    while (1)
    {
        int rc = sqlite3_step(statement);

        if (rc == SQLITE_ROW)
        {
            const ubyte_t* id = sqlite3_column_text(statement, 0);
            const ubyte_t* title = sqlite3_column_text(statement, 1);

            const ubyte_t* done = sqlite3_column_text(statement, 2);
            int done_i = atoi((const byte_t*)done);

            const ubyte_t* created = sqlite3_column_text(statement, 3) == NULL ? (ubyte_t*)"" : sqlite3_column_text(statement, 3);

            printf(CYAN("%-16s") "%-64s%-16s%-24s\n", id, title, done_i == 0 ? CROSS_MARK : CHECK_MARK, created);
        }

        if (rc == SQLITE_DONE)
        {
            break;
        }
    }

    free(search);

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_remove_todo(const byte_t* id, const byte_t** err)
{
    if (!id || id[0] == 0)
    {
        if (err)
        {
            *err = "Please provide an id.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "delete from TODOS where ID = ?";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, id, strlen(id), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_print_details(const byte_t* id, const byte_t** err)
{
    if (!id || id[0] == 0)
    {
        if (err)
        {
            *err = "Please provide an id.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "select DETAILS from TODOS where ID = ?";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, id, strlen(id), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    int rc = 0;
    rc = sqlite3_step(statement);

    if (rc == SQLITE_ROW)
    {
        const ubyte_t* details = sqlite3_column_text(statement, 0) == NULL ? (ubyte_t*)"" : sqlite3_column_text(statement, 0);

        printf("%s\n", details);
    }

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_set_done(const byte_t* id, STORAGE_DONE_FLAG done, const byte_t** err)
{
    if (!id || id[0] == 0)
    {
        if (err)
        {
            *err = "Please provide an id.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "update TODOS set DONE = 1 where ID = ?";

    switch (done)
    {
    case STORAGE_OPEN:
        sql = "update TODOS set DONE = 0 where ID = ?";
        break;

    case STORAGE_DONE:
    default:
        break;
    }

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, id, strlen(id), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    int rc = sqlite3_step(statement);

    if (rc != SQLITE_DONE)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

/**
 * @brief Reads in a given file and stores the content in buffer.
 *
 * @param filepath File to read in.
 * @param buffer_size Size of the filled buffer.
 * @param buffer Buffer which contains the file content.
 * @param err Pointer to error message.
 * @return STORAGE_ERR_CODE Success indicator.
 */
static STORAGE_ERR_CODE storage_read_file(const byte_t* filepath, ssize_t* buffer_size, byte_t** buffer, const byte_t** err)
{
    assert(buffer != NULL);

    FILE* f = fopen(filepath, "rb");

    if (!f)
    {
        if (err)
        {
            int e = errno;
            *err = strerror(e);
        }

        return STORAGE_ERROR;
    }

    fseek(f, 0, SEEK_END);
    *buffer_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    *buffer = calloc(*buffer_size, sizeof(char));

    ssize_t read = fread(*buffer, sizeof(char), *buffer_size, f);

    if (read <= 0)
    {
        if (err)
        {
            int e = errno;
            *err = strerror(e);
        }

        free(*buffer);
        fclose(f);
        return STORAGE_ERROR;
    }

    fclose(f);
    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_attach_file(const byte_t* id, const byte_t* filepath, const byte_t** err)
{
    if (!id || id[0] == 0)
    {
        if (err)
        {
            *err = "Please provide an id.";
        }

        return STORAGE_ERROR;
    }

    if (!filepath || filepath[0] == 0)
    {
        if (err)
        {
            *err = "Please provide a file to attach.";
        }

        return STORAGE_ERROR;
    }

    byte_t* filename = basename(filepath);

    if (strcmp(filename, "") == 0)
    {
        if (err)
        {
            *err = "Please provide a valid filename.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "insert into ATTACHMENTS (NAME, TODO_ID, ATTACHMENT, SIZE) values (?, ?, ?, ?)";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, filename, strlen(filename), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 2, id, strlen(id), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    ssize_t bufsz = 0;
    byte_t* buffer = NULL;
    result = storage_read_file(filepath, &bufsz, &buffer, err);

    if (result != SQLITE_OK)
    {
        return STORAGE_ERROR;
    }

    result = sqlite3_bind_blob(statement, 3, buffer, bufsz, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_int64(statement, 4, bufsz);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        free(buffer);
        return STORAGE_ERROR;
    }

    free(buffer);

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }


    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_remove_attachment(const byte_t* id, const byte_t** err)
{
    if (!id || id[0] == 0)
    {
        if (err)
        {
            *err = "Please provide an id.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "delete from ATTACHMENTS where ID = ?";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, id, strlen(id), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_print_attachments(const byte_t* todo_id, const byte_t** err)
{
    if (!todo_id || todo_id[0] == 0)
    {
        if (err)
        {
            *err = "Please provide an id.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    printf(MAGENTA("%-16s%-64s%-16s\n"), "Id", "Name", "Size in bytes");

    const byte_t* sql = "select t.ID, t.NAME, t.SIZE from ATTACHMENTS t where t.TODO_ID = ?";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, todo_id, strlen(todo_id), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    while (1)
    {
        int rc = sqlite3_step(statement);

        if (rc == SQLITE_ROW)
        {
            const ubyte_t* id = sqlite3_column_text(statement, 0);
            const ubyte_t* name = sqlite3_column_text(statement, 1);
            sqlite3_int64 size = sqlite3_column_int64(statement, 2);

            printf(CYAN("%-16s") "%-64s%-16lld\n", id, name, size);
        }

        if (rc == SQLITE_DONE)
        {
            break;
        }
    }

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_get_details(const byte_t* id, byte_t* buffer, size_t buflen, size_t* written, const byte_t** err)
{
    assert(buffer != NULL);

    if (!id || id[0] == 0)
    {
        if (err)
        {
            *err = "Please provide an id.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "select t.DETAILS from TODOS t where t.ID = ?";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, id, strlen(id), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_ROW && result != SQLITE_DONE)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }
    else
    {
        const ubyte_t* details = sqlite3_column_text(statement, 0);

        if (details)
        {
            for (size_t i = 0; i < buflen - 1; i++)
            {
                if (details[i] == 0)
                {
                    break;
                }

                buffer[i] = details[i];
                *written = i + 1;
            }
        }
    }

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

STORAGE_ERR_CODE storage_save_details(const byte_t* id, byte_t* buffer, size_t buflen, const byte_t** err)
{
    assert(buffer != NULL);

    if (!id || id[0] == 0)
    {
        if (err)
        {
            *err = "Please provide an id.";
        }

        return STORAGE_ERROR;
    }

    int result = sqlite3_open(storage_file_path, &sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    const byte_t* sql = "update TODOS set DETAILS = ? where ID = ?";

    sqlite3_stmt* statement;
    result = sqlite3_prepare(sqlite_handle, sql, strlen(sql), &statement, NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 1, buffer, strlen(buffer), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_bind_text(statement, 2, id, strlen(id), NULL);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_step(statement);

    if (result != SQLITE_DONE)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_finalize(statement);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    result = sqlite3_close(sqlite_handle);

    if (result != SQLITE_OK)
    {
        if (err)
        {
            *err = sqlite3_errmsg(sqlite_handle);
        }

        return STORAGE_ERROR;
    }

    return STORAGE_NO_ERROR;
}

const byte_t* storage_file()
{
    return storage_file_path;
}