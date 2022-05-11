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

#include "../symbols/symbols.h"
#include "../types/types.h"

/**
 * @brief Defines options for printing todos.
 *
 */
typedef enum
{
    ALL,
    DONE,
    OPEN,

} STORAGE_PRINT_OPTIONS;

/**
 * @brief Defines error codes for storage operations.
 *
 */
typedef enum
{
    STORAGE_NO_ERROR,
    STORAGE_ERROR,
    STORAGE_CRITICAL_ERROR

} STORAGE_ERR_CODE;

/**
 * @brief Defines open and done for todo entries.
 *
 */
typedef enum
{
    STORAGE_OPEN,
    STORAGE_DONE,

} STORAGE_DONE_FLAG;

/**
 * @brief Returns the equivalent print option for the given string.
 *
 * @param option
 * @return const byte_t*
 */
STORAGE_PRINT_OPTIONS storage_str_to_option(const wchar_t* option);

/**
 * @brief Initializes data that is needed for the storage.
 *
 * @param err Pointer to error message.
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_init(const byte_t** err);

/**
 * @brief Creates a new storage for todo entries.
 *
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_new_storage(const byte_t** err);

/**
 * @brief Creates a new todo with given data.
 *
 * @param title Title of the todo entry.
 * @param details Detailed information of the todo.
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_new_todo(const byte_t* title, const byte_t* details, const byte_t** err);

/**
 * @brief Prints current entries in the database.
 *
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_print_todos(STORAGE_PRINT_OPTIONS option, const byte_t** err);

/**
 * @brief Erases all entries from the database.
 *
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_erase(const byte_t** err);

/**
 * @brief Searches for the given string and prints entries that contain this string.
 *
 * @param search_str The string to search.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_print_search_results(const byte_t* search_str, const byte_t** err);

/**
 * @brief Removes the entry with given id from the database.
 *
 * @param id Id of the entry that should be deleted.
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_remove_todo(const byte_t* id, const byte_t** err);

/**
 * @brief Prints the details of the entry with given id.
 *
 * @param id The id of the entry.
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_print_details(const byte_t* id, const byte_t** err);

/**
 * @brief Sets the entry with given id to done.
 *
 * @param id Id of the entry.
 * @param done The done flag.
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_set_done(const byte_t* id, STORAGE_DONE_FLAG done, const byte_t** err);

/**
 * @brief Stores a file in the attachments table for the todo entry with given id.
 *
 * @param id Id of the todo entry.
 * @param filepath Path of the file that should be attached.
 * @param err Pointer to error message.
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_attach_file(const byte_t* id, const byte_t* filepath, const byte_t** err);

/**
 * @brief Removes an attachment from the database.
 *
 * @param id Id of the attachment that should be deleted.
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_remove_attachment(const byte_t* id, const byte_t** err);

/**
 * @brief Prints attachments for the given todo.
 *
 * @param id The id of the todo entry.
 * @param err Pointer to error message.
 *
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_print_attachments(const byte_t* todo_id, const byte_t** err);

/**
 * @brief Reads the details for the todo entry with given id.
 *
 * @param id Id of the todo.
 * @param buffer The buffer that will contain the details or be NULL.
 * @param buflen Size of the buffer.
 * @param written Pointer to the actual number of bytes that were written into the buffer.
 * @param err Pointer to error message.
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_get_details(const byte_t* id, byte_t* buffer, size_t buflen, size_t* written, const byte_t** err);

/**
 * @brief Saves the given buffer as the details of the entry with given id.
 *
 * @param id Id of the todo entry.
 * @param buffer Detail buffer.
 * @param buflen Buffer size.
 * @param err Pointer to error message.
 * @return STORAGE_ERR_CODE Success indicator.
 */
STORAGE_ERR_CODE storage_save_details(const byte_t* id, byte_t* buffer, size_t buflen, const byte_t** err);

/**
 * @brief Returns the path to the storage file.
 *
 * @return const byte_t* Storage file.
 */
const byte_t* storage_file();