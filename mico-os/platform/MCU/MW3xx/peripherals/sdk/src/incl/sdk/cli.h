
/*! \file cli.h
 *  \brief CLI module
 *
 * \section cli_usage Usage
 * The CLI module lets you register commands with the CLI interface. Modules
 * that wish to register the commands should initialize the struct cli_command
 * structure and pass this to cli_register_command(). These commands will then
 * be available on the CLI.
 *
 */
/*  Copyright 2008-2015, Marvell International Ltd.
 *  All Rights Reserved.
 */

#ifndef __CLI_H__
#define __CLI_H__

/** Structure for registering CLI commands */
struct cli_command {
	/** The name of the CLI command */
	const char *name;
	/** The help text associated with the command */
	const char *help;
	/** The function that should be invoked for this command. */
	void (*function) (int argc, char **argv);
};

/** Register a CLI command
 *
 * This function registers a command with the command-line interface.
 * 
 * \param[in] command The structure to register one CLI command
 * \return 0 on success
 * \return 1 on failure
 */
int cli_register_command(const struct cli_command *command);

/** Unregister a CLI command
 *
 * This function unregisters a command from the command-line interface.
 * 
 * \param[in] command The structure to unregister one CLI command
 * \return 0 on success
 * \return 1 on failure
 */
int cli_unregister_command(const struct cli_command *command);

/** Initialize the CLI module
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise.
 */
int cli_init(void);

/** Stop the CLI thread and carry out the cleanup
 *
 * \return WM_SUCCESS on success
 * \return error code otherwise.
 *
 */
int cli_stop(void);

/** Register a batch of CLI commands
 *
 * Often, a module will want to register several commands.
 * 
 * \param[in] commands Pointer to an array of commands.
 * \param[in] num_commands Number of commands in the array.
 * \return 0 on success
 * \return 1 on failure
 */
int cli_register_commands(const struct cli_command *commands, int num_commands);

/** Unregister a batch of CLI commands
 *
 * \param[in] commands Pointer to an array of commands.
 * \param[in] num_commands Number of commands in the array.
 * \return 0 on success
 * \return 1 on failure
 */
int cli_unregister_commands(const struct cli_command *commands,
			    int num_commands);

/* Get a command buffer
 *
 * If an external input task wants to use the CLI, it can use
 * cli_get_cmd_buffer() to get a command buffer that it can then
 * submit to the CLI later using cli_submit_cmd_buffer().
 *
 * \param buff Pointer to a char * to place the buffer pointer in.
 * \return WM_SUCCESS on success
 * \return error code otherwise.
 */
int cli_get_cmd_buffer(char **buff);

/* Submit a command buffer to the CLI
 *
 * Sends the command buffer to the CLI for processing.
 *
 * \param buff Pointer to a char * buffer.
 * \return WM_SUCCESS on success
 * \return error code otherwise.
 */
int cli_submit_cmd_buffer(char **buff);

/*
 */
typedef int (*cli_name_val_get)(const char *name, char *value, int max_len);

/*
 */
typedef int (*cli_name_val_set)(const char *name, const char *value);

/*
 * @internal
 *
 * Hook registration function for cli history functionality
 */
int cli_add_history_hook(cli_name_val_get get_cb, cli_name_val_set set_cb);

#endif /* __CLI_H__ */
