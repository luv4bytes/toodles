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
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <linux/limits.h>

#include "cli.h"
#include "error.h"

#include "../types/types.h"
#include "../color/color.h"
#include "../storage/storage.h"
#include "../history/history.h"
#include "../env/env.h"
#include "../symbols/symbols.h"

#define FWDECL // Indicator for forward declarative statements.

#define BUFLEN_ID 17
#define BUFLEN_CLI 8193
#define BUFLEN_TITLE 65
#define BUFLEN_DETAIL 513
#define BUFLEN_LIST_OPTION 17
#define BUFLEN_YES_NO 3
#define BUFLEN_SEARCH_STR 129
#define BUFLEN_HISTORY_INDEX 5

#define EDIT_TEMP_FILE_NAME "toodles.details.edit"
#define DEFAULT_EDITOR "vim"

#define CHAR_ARR_EMPTY(arr) arr[0] == 0
#define ARR_SIZE(arr) sizeof(arr) / sizeof(arr[0])

/**
 * @brief Command categories.
 *
 */
typedef enum
{
    TODOS,
    ATTACHMENTS,
    MISC,

} COMMAND_CATEGORIES;

/**
 * @brief Defines a command.
 *
 */
typedef struct inner_command_t
{
    /**
     * @brief Name of the command.
     *
     */
    const wchar_t* command;

    /**
    * @brief Short version of the command..
    *
    */
    const wchar_t* short_command;

    /**
     * @brief Is set to true if the command was issued as a short command.
     *
     */
    bool short_cmd_active;

    /**
     * @brief Description of the command signature.
     *
     */
    const byte_t* synopsis;

    /**
     * @brief Category that the command belongs to.
     *
     */
    const COMMAND_CATEGORIES category;

    /**
     * @brief Description of the command.
     *
     */
    const byte_t* description;

    /**
     * @brief The function that is executed when the command is issued.
     *
     */
    const void (*func)(const struct inner_command_t* cmd, const wchar_t* cmdstr);

} command_t;

const byte_t* prompts[] = {
    GREEN_REGULAR("toodles :) > "),
    GREEN_REGULAR(">>> "),
    GREEN_REGULAR("==> "),
    GREEN_REGULAR("*-* > "),
    GREEN_REGULAR(":-* > "),
    GREEN_REGULAR("wanna party? > "),
    GREEN_REGULAR("omg i love cookies!... > "),
    GREEN_REGULAR("funny, huh? > "),
    GREEN_REGULAR("¯\\_(ツ)_/¯ > "),
    GREEN_REGULAR("ugh... duh... > "),
    MAGENTA("\xF0\x9F\x8D\x86") CYAN("\xF0\x9F\x92\xA6") " > ",
    GREEN_REGULAR("I know, right?... > "),
    GREEN_REGULAR("Okay, boomer... > "),
};

FWDECL static void cli_print_help();
FWDECL static void cli_exit();
FWDECL static void cli_add();
FWDECL static void cli_list();
FWDECL static void cli_erase();
FWDECL static void cli_search();
FWDECL static void cli_clear();
FWDECL static void cli_remove();
FWDECL static void cli_detail();
FWDECL static void cli_done();
FWDECL static void cli_open();
FWDECL static void cli_history();
FWDECL static void cli_history_exec();
FWDECL static void cli_edit();
FWDECL static void cli_version();
FWDECL static void cli_attach();
FWDECL static void cli_delete_attachment();
FWDECL static void cli_show_attachments();
FWDECL static void cli_print_attachment();
FWDECL static void cli_save_attachment_to_disk();
FWDECL static void cli_execute_cmdstr();
FWDECL static void cli_env();

/**
 * @brief Array of available commands.
 *
 */
static command_t COMMANDS[] = {
    {
        .command = L"add",
        .short_command = L"a",
        .description = "Adds a new todo entry.",
        .func = cli_add,
        .synopsis = "[TITLE](opt)",
        .category = TODOS,
    },
    {
        .command = L"remove",
        .short_command = L"r",
        .description = "Removes a todo entry.",
        .func = cli_remove,
        .synopsis = "[ID]",
        .category = TODOS,
    },
    {
        .command = L"edit",
        .short_command = L"e",
        .description = "Edit a todo entry.",
        .func = cli_edit,
        .synopsis = "[ID]",
        .category = TODOS,
    },
    {
        .command = L"detail",
        .short_command = L"d",
        .description = "Displays the details of an entry.",
        .func = cli_detail,
        .synopsis = "[ID]",
        .category = TODOS,
    },
    {
        .command = L"list",
        .short_command = L"l",
        .description = "Lists all current entries.",
        .func = cli_list,
        .synopsis = "[LIST OPTION](opt)",
        .category = TODOS,
    },
    {
        .command = L"search",
        .short_command = L"s",
        .description = "Search entries by title.",
        .synopsis = "[SEARCH EXPR]",
        .func = cli_search,
        .category = TODOS,
    },
    {
        .command = L"done",
        .description = "Marks the given todo as done.",
        .func = cli_done,
        .synopsis = "[ID]",
        .category = TODOS,
    },
    {
        .command = L"open",
        .description = "Marks the given todo as open.",
        .func = cli_open,
        .synopsis = "[ID]",
        .category = TODOS,
    },
    {
        .command = L"erase",
        .description = "Erases all entries from the database.",
        .func = cli_erase,
        .category = MISC,
    },
    {
        .command = L"help",
        .short_command = L"h",
        .description = "Displays helpful information for using toodle.",
        .func = cli_print_help,
        .category = MISC,
    },
    {
        .command = L"exit",
        .description = "Exits toodles.",
        .func = cli_exit,
        .category = MISC,
    },
    {
        .command = L"quit",
        .description = "Exits toodles.",
        .func = cli_exit,
        .category = MISC,
    },
    {
        .command = L"clear",
        .description = "Clears the screen.",
        .func = cli_clear,
        .category = MISC,
    },
    {
        .command = L"history",
        .description = "Displays the command history of the session.",
        .func = cli_history,
        .category = MISC,
    },
    {
        .command = L"version",
        .description = "Displays toodles version number.",
        .func = cli_version,
        .category = MISC,
    },
    {
        .command = L"attach",
        .description = "Attaches a file to an existing todo.",
        .func = cli_attach,
        .category = ATTACHMENTS,
    },
    {
        .command = L"delatt",
        .description = "Deletes the attachment with given id.",
        .func = cli_delete_attachment,
        .synopsis = "[ID]",
        .category = ATTACHMENTS
    },
    {
        .command = L"showatt",
        .description = "Shows all attachments for given todo id.",
        .func = cli_show_attachments,
        .synopsis = "[ID]",
        .category = ATTACHMENTS
    },
    {
        .command = L"patt",
        .description = "Prints out the content of the attachment.",
        .func = cli_print_attachment,
        .synopsis = "[ID]",
        .category = ATTACHMENTS
    },
    {
        .command = L"satt",
        .description = "Save an attachment to disk.",
        .func = cli_save_attachment_to_disk,
        .synopsis = "[ID]",
        .category = ATTACHMENTS
    },
    {
        .command = L"!",
        .description = "Executes a command that is stored in the history.",
        .func = cli_history_exec,
        .synopsis = "[HISTORY INDEX]",
        .category = MISC,
    },
    {
        .command = L"env",
        .description = "Displays environment data for toodles.",
        .func = cli_env,
        .category = MISC,
    }
};

/**
 * @brief Self tailored getline. Discards all that is left in stdin after reading into the buffer and replaces newline in buffer with 0.
 *
 * @param buffer The buffer to read into.
 * @param buflen Size of the buffer.
 */
static void cli_getline_discard(wchar_t* buffer, size_t buflen)
{
    if (!buffer)
        return;

    wchar_t* check = fgetws(buffer, buflen, stdin);

    if (!check)
    {
        printf(RED("ERR: ") "%s\n", "Error retrieving line.");
        return;
    }

    size_t len = wcslen(buffer);
    size_t len_nl = wcscspn(buffer, L"\n");

    if (len == len_nl)
    {
        wchar_t c;
        while ((c = getwc(stdin)) != '\n');
    }

    buffer[wcscspn(buffer, L"\n")] = 0;
}

/**
 * @brief Parses the command and fills in arguments that were given.
 *
 * @param cmd The command.
 * @param cmdstr The command as a string.
 * @param argc Number of actual arguments for the command.
 * @param buffers Array of buffers that will hold the argument values.
 * @param buflens Array of size_t that holds the length of any argument in the argument order.
 * @return int Number of arguments.
 */
static int cli_parse_cmd(const command_t* cmd, const wchar_t* cmdstr, size_t argc, wchar_t** buffers, size_t* buflens)
{
    if (cmd == NULL)
    {
        return -1;
    }

    if (cmdstr == NULL)
    {
        return -1;
    }

    if (argc == 0)
    {
        return -1;
    }

    if (buffers == NULL)
    {
        return -1;
    }

    size_t found_args = 0;
    size_t buffer_index = 0;
    short min_one = 0;

    size_t whitespaces = 0;

    for (size_t i = 0; i < BUFLEN_CLI; i++)
    {
        if (cmdstr[i] == ' ' || cmdstr[i] == '\t')
        {
            whitespaces++;
            continue;
        }

        break;
    }

    size_t cmdlen = (cmd->short_cmd_active == true ? wcslen(cmd->short_command) : wcslen(cmd->command)) + whitespaces + 1;
    for (size_t i = cmdlen; i < BUFLEN_CLI; i++)
    {
        if (cmdstr[i] == 0) // End of cmdstr
        {
            if (min_one != 0)
            {
                found_args++;
            }
            break;
        }

        if (buffer_index == buflens[found_args] - 1) // End of current argument
        {
            break;
        }

        if (cmdstr[i] == ' ' && argc > 1) // If we want more than one argument we treat the spaces as seperators
        {                                 // Found an argument
            buffer_index = 0;
            found_args++;
            continue;
        }

        if (found_args == argc) // Found all wanted arguments
        {
            break;
        }

        buffers[found_args][buffer_index++] = cmdstr[i];

        min_one = 1;
    }

    if (min_one == 0)
    {
        return -1;
    }

    return found_args;
}

/**
 * @brief Exits the application.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_exit(command_t* cmd, const wchar_t* cmdstr)
{
    exit(EXIT_SUCCESS);
}

static inline void pcmd(const command_t* cmd)
{
    if (cmd == NULL)
    {
        return;
    }

    const byte_t* synops = cmd->synopsis == NULL ? "" : cmd->synopsis;
    const wchar_t* scmd = cmd->short_command == NULL ? L"" : cmd->short_command;

    printf(MAGENTA("%-15ls%-15ls%-30s") "%-20s\n", cmd->command, scmd, synops, cmd->description);
}

/**
 * @brief Prints the application help information.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_print_help(command_t* cmd, const wchar_t* cmdstr)
{
    printf("\n");
    size_t len = sizeof(COMMANDS) / sizeof(COMMANDS[0]);

    printf(CYAN("%-15s%-15s%-30s%-20s\n"), "Long command", "Short command", "Synopsis", "Description");
    printf("\n");

    printf(YELLOW("Commands for ToDo entries\n\n"));

    for (size_t i = 0; i < len; i++)
    {
        if (COMMANDS[i].category == TODOS)
            pcmd(&COMMANDS[i]);
    }

    printf("\n");
    printf(YELLOW("Commands for attachments\n\n"));

    for (size_t i = 0; i < len; i++)
    {
        if (COMMANDS[i].category == ATTACHMENTS)
            pcmd(&COMMANDS[i]);
    }

    printf("\n");
    printf(YELLOW("Miscellaneous commands\n\n"));

    for (size_t i = 0; i < len; i++)
    {
        if (COMMANDS[i].category == MISC)
            pcmd(&COMMANDS[i]);
    }

    printf("\n");
    printf(YELLOW("Non-interactive mode")"\n\n");
    printf("Toodles can be run in non-interactive mode too.\n");
    printf("Non-interactive mode will be used if at least one argument is given to toodles.\n");
    printf("For more information on non-interactive mode use " CYAN("'toodles -h'")".\n");
    printf("\n");
}

/**
 * @brief Add a new todo.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_add(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t title[BUFLEN_TITLE] = { 0 };
    wchar_t details[BUFLEN_DETAIL] = { 0 };

    wchar_t* args[] = {
        title,
    };

    size_t buflens[] = {
        BUFLEN_TITLE,
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, buflens);

    if (read == -1)
    {
        printf("Title: ");
        cli_getline_discard(title, BUFLEN_TITLE);

        printf("Details (can be empty): ");
        cli_getline_discard(details, BUFLEN_DETAIL);
    }

    const byte_t* err = NULL;

    byte_t bs_title[BUFLEN_TITLE * sizeof(wchar_t)] = { 0 };
    wstobs(title, bs_title, BUFLEN_TITLE * sizeof(wchar_t));

    byte_t bs_details[BUFLEN_DETAIL * sizeof(wchar_t)] = { 0 };
    wstobs(details, bs_details, BUFLEN_DETAIL * sizeof(wchar_t));

    STORAGE_ERR_CODE result = storage_new_todo(bs_title, bs_details, &err);

    if (result != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Prints out all todo entries.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_list(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t opt_str[BUFLEN_LIST_OPTION] = { 0 };

    wchar_t* args[] = {
        opt_str
    };

    size_t lens[] = {
        BUFLEN_LIST_OPTION
    };

    cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    STORAGE_PRINT_OPTIONS option = storage_str_to_option(opt_str);

    const byte_t* err = NULL;
    STORAGE_ERR_CODE error = storage_print_todos(option, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Erases all data from the database.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_erase(command_t* cmd, const wchar_t* cmdstr)
{
    printf(YELLOW("Do you really want to erase all data? [y,n]: "));

    wchar_t yes_no[BUFLEN_YES_NO] = { 0 };
    cli_getline_discard(yes_no, BUFLEN_YES_NO);

    if (wcscmp(yes_no, L"y") != 0)
    {
        printf("Cancel\n");
        return;
    }

    const byte_t* err = NULL;
    STORAGE_ERR_CODE error = storage_erase(&err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }

    printf("Done\n");
}

/**
 * @brief Searches for a string in all entries.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_search(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t search[BUFLEN_SEARCH_STR] = { 0 };

    wchar_t* args[] = {
        search
    };

    size_t lens[] = {
        BUFLEN_SEARCH_STR
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_search[BUFLEN_SEARCH_STR * sizeof(wchar_t)] = { 0 };
    wstobs(search, bs_search, BUFLEN_SEARCH_STR * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_print_search_results(bs_search, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Clears the screen.
 *
 */
static void cli_clear()
{
    printf("\033[2J\033[H");
}

/**
 * @brief Remove an entry from the database.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_remove(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_remove_todo(bs_id, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Display the details of an entry.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_detail(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_print_details(bs_id, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Marks an entry as done.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_done(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_set_done(bs_id, STORAGE_DONE, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Marks an entry as open.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_open(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_set_done(bs_id, STORAGE_OPEN, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Displays the command history of the current session.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_history(command_t* cmd, const wchar_t* cmdstr)
{
    history_print();
}

/**
 * @brief Executes a command that is stored in history on the given index.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_history_exec(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t history_index[BUFLEN_HISTORY_INDEX] = { 0 };

    wchar_t* args[] = {
        history_index
    };

    size_t lens[] = {
        BUFLEN_HISTORY_INDEX
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    byte_t bs_index[BUFLEN_HISTORY_INDEX * sizeof(wchar_t)] = { 0 };
    wstobs(history_index, bs_index, BUFLEN_HISTORY_INDEX * sizeof(wchar_t));

    int history_index_int = atoi(bs_index);

    if (read == -1)
        return;

    byte_t* err = NULL;
    const wchar_t* exec = history_get(history_index_int, &err);

    if (exec == NULL)
    {
        if (err != NULL)
        {
            printf(RED("ERR: ") "%s\n", err);
        }

        return;
    }

    cli_execute_cmdstr(exec);
}

/**
 * @brief Prints toodles version number.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_version(command_t* cmd, const wchar_t* cmdstr)
{
#ifndef NDEBUG
    printf("%s %s\n", VERSION, "(Debug Build)");
#else
    printf("%s %s\n", VERSION, "(Release Build)");
#endif
}

/**
 * @brief Starts the default editor or the editor set by EDITOR env variable to edit the temp detail file.
 *
 * @return int Editor return code.
 */
static int edit_temp_details()
{
    const byte_t* editor = getenv("EDITOR");
    int editor_return = 0;

    if (!editor)
    {
        editor_return = system(DEFAULT_EDITOR " " EDIT_TEMP_FILE_NAME);
    }
    else
    {
        size_t edlen = strlen(editor);
        size_t len = edlen + strlen(EDIT_TEMP_FILE_NAME) + 2;
        char editcmd[len];

        memset(editcmd, 0, len);

        strcat(editcmd, editor);
        strcat(editcmd, " ");
        strcat(editcmd, EDIT_TEMP_FILE_NAME);

        editor_return = system(editcmd);
    }

    return editor_return;
}

/**
 * @brief Edits the data of a given entry.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_edit(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };
    const byte_t* errstr = NULL;

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    remove(EDIT_TEMP_FILE_NAME);

    FILE* write_file = fopen(EDIT_TEMP_FILE_NAME, "w");

    if (write_file == NULL)
    {
        int err = errno;
        errstr = strerror(err);

        printf(RED("ERR: ") "%s\n", errstr);
        return;
    }

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    byte_t bs_buf[BUFLEN_DETAIL * sizeof(wchar_t)] = { 0 };

    size_t num_written_bytes = 0;
    STORAGE_ERR_CODE storage_error = storage_get_details(bs_id, bs_buf, BUFLEN_DETAIL, &num_written_bytes, &errstr);

    if (storage_error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", errstr);
        fclose(write_file);
        remove(EDIT_TEMP_FILE_NAME);
        return;
    }

    size_t written = fwrite(bs_buf, sizeof(byte_t), num_written_bytes, write_file);

    if (written < num_written_bytes)
    {
        int errind = ferror(write_file);

        if (errind != 0)
        {
            printf(RED("ERR: ") "Error writing temporary detail file.\n");
            fclose(write_file);
            remove(EDIT_TEMP_FILE_NAME);
            return;
        }
    }

    fclose(write_file);

    int editor_return = edit_temp_details();

    if (editor_return != EXIT_SUCCESS)
    {
        printf(RED("ERR: ") "%s\n", "Error with vim.");
        return;
    }

    FILE* read_file = fopen(EDIT_TEMP_FILE_NAME, "r");

    if (read_file == NULL)
    {
        int err = errno;
        errstr = strerror(err);

        remove(EDIT_TEMP_FILE_NAME);

        printf(RED("ERR: ") "%s\n", errstr);
        return;
    }

    fseek(read_file, 0, SEEK_END);
    size_t new_temp_sz = ftell(read_file);
    fseek(read_file, 0, SEEK_SET);

    byte_t tempbuf[new_temp_sz + 1];
    memset(tempbuf, 0, (new_temp_sz + 1) * sizeof(byte_t));

    size_t read_bytes = fread(tempbuf, sizeof(byte_t), new_temp_sz, read_file);

    if (read_bytes < new_temp_sz)
    {
        int errind = ferror(write_file);

        if (errind != 0)
        {
            printf(RED("ERR: ") "Error reading temporary detail file.\n");
            fclose(read_file);
            remove(EDIT_TEMP_FILE_NAME);
            return;
        }
    }

    const byte_t* save_err;
    STORAGE_ERR_CODE saved = storage_save_details(bs_id, tempbuf, new_temp_sz, &save_err);

    if (saved != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", save_err);
        fclose(read_file);
        remove(EDIT_TEMP_FILE_NAME);
        return;
    }

    int removed = remove(EDIT_TEMP_FILE_NAME);

    if (removed == -1)
    {
        int e = errno;

        fclose(read_file);

        printf(RED("ERR: ") "%s\n", strerror(e));
        return;
    }

    fclose(read_file);
}

/**
 * @brief Attaches a file to a todo entry.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_attach(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };
    wchar_t path[PATH_MAX] = { 0 };

    printf("Todo Id: ");
    cli_getline_discard(id, BUFLEN_ID);

    printf("File path: ");
    cli_getline_discard(path, PATH_MAX);

    if (CHAR_ARR_EMPTY(id))
    {
        printf(RED("ERR: ") "%s\n", "Please provide an id.");
        return;
    }

    if (CHAR_ARR_EMPTY(path))
    {
        printf(RED("ERR: ") "%s\n", "Please provide a file path.");
        return;
    }

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    byte_t bs_path[PATH_MAX * sizeof(wchar_t)] = { 0 };
    wstobs(path, bs_path, PATH_MAX * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_attach_file(bs_id, bs_path, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Removes attachment from the database.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_delete_attachment(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_remove_attachment(bs_id, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Shows all attachments for given todo id.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_show_attachments(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_print_attachments(bs_id, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Print the content of an attachment to stdout.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_print_attachment(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 1, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_print_attachment_content(bs_id, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Save an attachment to disk.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_save_attachment_to_disk(command_t* cmd, const wchar_t* cmdstr)
{
    wchar_t id[BUFLEN_ID] = { 0 };
    wchar_t save_path[PATH_MAX] = { 0 };

    wchar_t* args[] = {
        id
    };

    size_t lens[] = {
        BUFLEN_ID
    };

    int read = cli_parse_cmd(cmd, cmdstr, 2, args, lens);

    if (read == -1)
        return;

    const byte_t* err = NULL;

    byte_t bs_id[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(id, bs_id, BUFLEN_ID * sizeof(wchar_t));

    printf("Save path: ");
    cli_getline_discard(save_path, PATH_MAX);

    byte_t bs_save_path[BUFLEN_ID * sizeof(wchar_t)] = { 0 };
    wstobs(save_path, bs_save_path, BUFLEN_ID * sizeof(wchar_t));

    STORAGE_ERR_CODE error = storage_save_attachment_to_disk(bs_id, bs_save_path, &err);

    if (error != STORAGE_NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", err);
        return;
    }
}

/**
 * @brief Prints environment information for toodles.
 *
 * @param cmd The issued command.
 * @param cmdstr The issued command as a string.
 */
static void cli_env(command_t* cmd, const wchar_t* cmdstr)
{
    printf(CYAN("%-20s") GREEN("%-128s\n"), "App directory", env_app_dir());
    printf(CYAN("%-20s") GREEN("%-128s\n"), "Storage", storage_file());
}

/**
 * @brief Checks if the given command is valid.
 *
 * @param cmd The given command.
 * @param result Pointer to pointer of a command structure. This gets set to the associated command structure.
 * @return CLI_ERROR Error code.
 */
static CLI_ERROR cli_is_valid_cmd(const wchar_t* cmd, const command_t** result)
{
    if (!cmd)
    {
        return INVALID_CMD;
    }

    size_t len = sizeof(COMMANDS) / sizeof(COMMANDS[0]);
    CLI_ERROR error = 0;

    short found = 0;

    for (size_t i = 0; i < len; i++)
    {
        command_t* cmdp = &COMMANDS[i];

        size_t ind = 0;
        wchar_t buf[BUFLEN_CLI] = { 0 };
        for (size_t j = 0; j < wcslen(cmd); j++)
        {
            if ((cmd[j] == ' ' || cmd[j] == '\t') && found == 0) // Ignore whitespaces until the first none-whitespace is hit
            {
                continue;
            }

            if (cmd[j] == ' ' && found == 1)
            {
                break;
            }

            buf[ind++] = cmd[j];
            found = 1;
        }

        if (wcscmp(buf, cmdp->command) == 0)
        {
            error = NO_ERROR;
            cmdp->short_cmd_active = false;
            *result = cmdp;
            break;
        }

        if (cmdp->short_command != NULL)
        {
            if (wcscmp(buf, cmdp->short_command) == 0)
            {
                error = NO_ERROR;
                cmdp->short_cmd_active = true;
                *result = cmdp;
                break;
            }
        }

        found = 0;
    }

    return error;
}

static void cli_execute_cmdstr(const wchar_t* cmdstr)
{
    const command_t* issued = NULL;
    CLI_ERROR isValid = cli_is_valid_cmd(cmdstr, &issued);

    if (isValid != NO_ERROR)
    {
        printf(RED("ERR: ") "%s\n", cli_err_str(isValid));
        return;
    }

    if (!issued->func)
    {
        printf(RED("ERR: ") "%s\n", "No handler assigned to command.");
        return;
    }

    issued->func(issued, cmdstr);

    int inserted = history_insert(cmdstr);

    if (inserted == 0)
    {
        printf(RED("ERR: ") "%s\n", "Error inserting command into history.");
    }
}

void cli_prompt()
{
    while (1)
    {
        size_t pind = rand() % ARR_SIZE(prompts);
        printf("%s", prompts[pind]);

        wchar_t cmd_buffer[BUFLEN_CLI] = { 0 };

        cli_getline_discard(cmd_buffer, BUFLEN_CLI);

        cli_execute_cmdstr(cmd_buffer);
    }
}