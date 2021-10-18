/*
 * Script Preprocessor - a general-purpose preprocessor program.
 * Copyright (C) 2021  Michael Federczuk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef SPP_ARG_PARSER_H
#define SPP_ARG_PARSER_H

#include <spp/types.h>
#include <stdbool.h>
#include <stddef.h>


struct spp_options_cli {
	const_cstr_t output;
	enum spp_options_cli_output_mode {
		SPP_OPTIONS_CLI_OUTPUT_MODE_TRUNCATE,
		SPP_OPTIONS_CLI_OUTPUT_MODE_APPEND
	} output_mode;
};

struct spp_option_func_input {
	const_cstr_t argv0;

	/** is nul-byte when long identifier was used */
	char short_identifier;
	/** may be uninitialized when short identifier was used */
	const_cstr_t long_identifier;

	/** is uninitialized when option expects no argument */
	const_cstr_t arg;

	struct spp_options_cli* options_cli;
};

typedef int (*spp_option_func_t)(struct spp_option_func_input);


enum spp_option_priority {
	/** executed as soon as encountered, parsing stops afterwards. */
	SPP_OPTION_PRIORITY_HIGH,
	SPP_OPTION_PRIORITY_LOW
};

struct spp_option {
	/** nul-byte if no short identifier */
	char short_identifier;
	/** is always expected */
	const_cstr_t long_identifier;

	/** `NULL` if no arg is expected */
	const_cstr_t arg_name;

	enum spp_option_priority priority;

	/**
	 * Never `NULL` for valid values.
	 * When set to `NULL`, it is an invalid value and can be used for things like sentinels.
	 */
	spp_option_func_t func;
};


struct spp_cli {
	/** is `NULL` when empty */
	const_cstr_t* input_files;
	size_t input_files_size;

	struct spp_options_cli options_cli;
};


enum spp_arg_parsing_result_type {
	SPP_ARG_PARSING_RESULT_TYPE_HIGH_PRIO_EXECUTED,
	SPP_ARG_PARSING_RESULT_TYPE_INVALID_OPTION_IDENTIFIER,
	SPP_ARG_PARSING_RESULT_TYPE_MISSING_OPTION_ARGUMENT,
	SPP_ARG_PARSING_RESULT_TYPE_EXCESS_OPTION_ARGUMENT,
	SPP_ARG_PARSING_RESULT_TYPE_LOW_PRIO_FAILED,
	SPP_ARG_PARSING_RESULT_TYPE_NORMAL_EXECUTION
};

struct spp_arg_parsing_result {
	enum spp_arg_parsing_result_type type;

	union {
		struct spp_arg_parsing_result_high_prio_executed {
			int exc;
		} high_prio_executed;

		struct spp_arg_parsing_result_invalid_option_identifier {
			// is nul-byte when long identifier was used
			char invalid_short_identifier;
			// is `NULL` when short identifier was used
			const_cstr_t invalid_long_identifier;
		} invalid_option_identifier;

		struct spp_arg_parsing_result_missing_option_argument {
			// is nul-byte when long identifier was used
			char short_identifier;
			// is uninitialized when short identifier was used
			const_cstr_t long_identifier;

			const_cstr_t arg_name;
		} missing_option_argument;

		struct spp_arg_parsing_result_excess_option_argument {
			// is nul-byte when long identifier was used
			char short_identifier;
			// is `NULL` when short identifier was used
			const_cstr_t long_identifier;
		} excess_option_argument;

		struct spp_arg_parsing_result_low_prio_failed {
			int exc;
		} low_prio_failed;

		struct spp_arg_parsing_result_normal_execution {
			struct spp_cli cli;
		} normal_execution;
	};
};

extern spp_ret_status_t spp_parse_args(
	int argc,
	const cstr_t* argv,
	struct spp_arg_parsing_result* result
);


#endif /* SPP_ARG_PARSER_H */
