/*
 * Script Preprocessor - a general-purpose preprocessor program.
 * Copyright (C) 2022  Michael Federczuk
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

#include <errno.h>
#include <spp/arg_parser.h>
#include <spp/compiler.h>
#include <spp/options.h>
#include <spp/types.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

enum {
	SAVED_LOW_PRIO_OPTIONS_INITIAL_CAPACITY = 1,
	SAVED_LOW_PRIO_OPTIONS_GROWTH_FACTOR = 2
};

struct option_to_execute {
	const struct spp_option* option;

	char used_short_identifier;
	const_cstr_t used_long_identifier;
	const_cstr_t arg;
};

spp_ret_status_t spp_parse_args(
	const int argc,
	const cstr_t* const argv,
	struct spp_arg_parsing_result* const result
) {
	struct spp_cli cli = {
		.input_files = NULL,
		.input_files_size = 0,

		.options_cli = {
			.output = NULL,
			.output_mode = SPP_OPTIONS_CLI_OUTPUT_MODE_TRUNCATE
		}
	};

	if(argc < 2) {
		result->type = SPP_ARG_PARSING_RESULT_TYPE_NORMAL_EXECUTION;
		result->normal_execution = (struct spp_arg_parsing_result_normal_execution){
			.cli = cli
		};

		return SPP_RET_STATUS_SUCCESS;
	}

	errno = 0;
	cli.input_files = malloc(sizeof(const_cstr_t) * (argc - 1));
	spp_if_unlikely(cli.input_files == NULL || errno != 0) {
		return SPP_RET_STATUS_FAILURE;
	}
	cli.input_files_size = 0;

	bool processing_opts = true;

	struct spp_arg_parsing_result_invalid_option_identifier invalid_option_identifier = {
		.invalid_short_identifier = '\0',
		.invalid_long_identifier = NULL
	};
	struct spp_arg_parsing_result_missing_option_argument missing_option_argument = {
		.arg_name = NULL
	};
	struct spp_arg_parsing_result_excess_option_argument excess_option_argument = {
		.short_identifier = '\0',
		.long_identifier = NULL
	};

	#define no_bad_options() \
		(invalid_option_identifier.invalid_short_identifier == '\0' && \
		 invalid_option_identifier.invalid_long_identifier == NULL) && \
		missing_option_argument.arg_name == NULL && \
		(excess_option_argument.short_identifier == '\0' && \
		 excess_option_argument.long_identifier == NULL)

	// any low priority options that are found is stored in this array
	struct option_to_execute* saved_low_prio_options = NULL;
	size_t saved_low_prio_options_size = 0;
	size_t saved_low_prio_options_capacity = 0;
	#define append_saved_low_prio_option(struct_option_to_execute) \
		do { \
			if(no_bad_options()) { \
				if((saved_low_prio_options_size + 1) > saved_low_prio_options_capacity) { \
					if(saved_low_prio_options_capacity == 0) { \
						saved_low_prio_options_capacity = SAVED_LOW_PRIO_OPTIONS_INITIAL_CAPACITY; \
						\
						errno = 0; \
						saved_low_prio_options = malloc(sizeof(*saved_low_prio_options) * saved_low_prio_options_capacity); \
						spp_if_unlikely(saved_low_prio_options == NULL || errno != 0) { \
							free(cli.input_files); \
							return SPP_RET_STATUS_FAILURE; \
						} \
					} else { \
						saved_low_prio_options_capacity *= SAVED_LOW_PRIO_OPTIONS_GROWTH_FACTOR; \
						\
						errno = 0; \
						struct option_to_execute* tmp = realloc( \
							saved_low_prio_options, \
							sizeof(*saved_low_prio_options) * saved_low_prio_options_capacity \
						); \
						spp_if_unlikely(tmp == NULL || errno != 0) { \
							free(saved_low_prio_options); \
							free(cli.input_files); \
							return SPP_RET_STATUS_FAILURE; \
						} \
						saved_low_prio_options = tmp; \
					} \
				} \
				\
				saved_low_prio_options[saved_low_prio_options_size] = struct_option_to_execute; \
				++saved_low_prio_options_size; \
			} \
		} while(0);

	size_t arg_len;
	int exc;

	for(cstr_t const* arg_it = argv + 1; *arg_it != NULL; ++arg_it) {
		if(processing_opts && (arg_len = strlen(*arg_it)) >= 2 && (*arg_it)[0] == '-') {
			if((*arg_it)[1] == '-') {
				if(arg_len == 2) {
					// "--" argument

					processing_opts = false;
					continue;
				}

				// long option

				cstr_t long_identifier = (*arg_it + 2); // incrementing 2 to skip the "--" prefix

				// searching for the '=' character in the argument
				cstr_t option_arg = NULL;
				for(cstr_t option_arg_it = long_identifier; *option_arg_it != '\0'; ++option_arg_it) {
					if(*option_arg_it == '=') {
						// By "splitting" the actual argument itself, by replacing the '=' with a nul-byte, the
						// `long_identifier` c-string """thinks""" that it ends where the '=' character used to be.
						// By doing this, there is no need to dynamically allocate a string just for the identifier of
						// the option.
						*option_arg_it = '\0';
						option_arg = (option_arg_it + 1);
						break;
					}
				}

				// searching for a defined option by the long identifier
				const struct spp_option* found_option = spp_options;
				for(; found_option->func != NULL; ++found_option) {
					if(strcmp(found_option->long_identifier, long_identifier) == 0) {
						break;
					}
				}

				if(found_option->func == NULL) {
					// no defined option found with the given identifier

					if(no_bad_options()) {
						invalid_option_identifier.invalid_long_identifier = long_identifier;
					}

					continue;
				}

				if(option_arg == NULL) {
					if(found_option->arg_name != NULL) {
						// when no argument was given along the option, but the option expects an argument

						if(*(arg_it + 1) != NULL) {
							++arg_it;
							option_arg = *arg_it;
						} else {
							switch(found_option->priority) {
								case(SPP_OPTION_PRIORITY_HIGH): {
									// immediately report the missing argument for high priority options

									missing_option_argument.short_identifier = '\0';
									missing_option_argument.long_identifier = found_option->long_identifier;
									missing_option_argument.arg_name = found_option->arg_name;

									goto missing_option_argument_result;
								}
								case(SPP_OPTION_PRIORITY_LOW): {
									if(no_bad_options()) {
										missing_option_argument.short_identifier = '\0';
										missing_option_argument.long_identifier = found_option->long_identifier;
										missing_option_argument.arg_name = found_option->arg_name;
									}

									continue;
								}
							}
						}
					}
				} else if(found_option->arg_name == NULL) {
					// when an argument was given along the option, but the option doesn't expect an argument

					switch(found_option->priority) {
						case(SPP_OPTION_PRIORITY_HIGH): {
							// immediately report the excessive argument for high priority options

							excess_option_argument.short_identifier = '\0';
							excess_option_argument.long_identifier = found_option->long_identifier;

							goto excess_option_argument_result;
						}
						case(SPP_OPTION_PRIORITY_LOW): {
							if(no_bad_options()) {
								excess_option_argument.short_identifier = '\0';
								excess_option_argument.long_identifier = found_option->long_identifier;
							}

							continue;
						}
					}
				}

				switch(found_option->priority) {
					case(SPP_OPTION_PRIORITY_HIGH): {
						exc = found_option->func((struct spp_option_func_input){
							.argv0 = argv[0],

							.short_identifier = '\0',
							.long_identifier = found_option->long_identifier,

							.arg = option_arg,

							.options_cli = &cli.options_cli
						});

						result->type = SPP_ARG_PARSING_RESULT_TYPE_HIGH_PRIO_EXECUTED;
						result->high_prio_executed = (struct spp_arg_parsing_result_high_prio_executed){
							.exc = exc
						};

						goto success_not_normal_execution;
					}
					case(SPP_OPTION_PRIORITY_LOW): {
						append_saved_low_prio_option(((struct option_to_execute){
							.option = found_option,

							.used_short_identifier = '\0',
							.used_long_identifier = found_option->long_identifier,
							.arg = option_arg
						}));
					}
				}

				continue;
			}

			// short option

			const char* short_identifiers = (*arg_it + 1);  // incrementing 1 to skip the "-" prefix

			for(const char* short_it = short_identifiers; *short_it != '\0'; ++short_it) {
				// searching for a defined option by the short identifier
				const struct spp_option* found_option = spp_options;
				for(; found_option->func != NULL; ++found_option) {
					if(found_option->short_identifier == *short_it) {
						break;
					}
				}

				if(found_option->func == NULL) {
					// no defined option found with the given identifier

					if(no_bad_options()) {
						invalid_option_identifier.invalid_short_identifier = *short_it;
					}

					continue;
				}

				const_cstr_t option_arg = NULL;
				if(found_option->arg_name != NULL) {
					if(*(short_it + 1) != '\0') {
						++short_it;
						option_arg = short_it;

						while(*(short_it + 1) != '\0') {
							++short_it;
						}
					} else if(*(arg_it + 1) != NULL) {
						++arg_it;
						option_arg = *arg_it;
					} else {
						switch(found_option->priority) {
							case(SPP_OPTION_PRIORITY_HIGH): {
								// immediately report the missing argument for high priority options

								missing_option_argument.short_identifier = found_option->short_identifier;
								missing_option_argument.arg_name = found_option->arg_name;

								goto missing_option_argument_result;
							}
							case(SPP_OPTION_PRIORITY_LOW): {
								if(no_bad_options()) {
									missing_option_argument.short_identifier = found_option->short_identifier;
									missing_option_argument.arg_name = found_option->arg_name;
								}

								continue;
							}
						}
					}
				}

				switch(found_option->priority) {
					case(SPP_OPTION_PRIORITY_HIGH): {
						exc = found_option->func((struct spp_option_func_input){
							.argv0 = argv[0],

							.short_identifier = found_option->short_identifier,

							.arg = option_arg,

							.options_cli = &cli.options_cli
						});

						result->type = SPP_ARG_PARSING_RESULT_TYPE_HIGH_PRIO_EXECUTED;
						result->high_prio_executed = (struct spp_arg_parsing_result_high_prio_executed){
							.exc = exc
						};

						goto success_not_normal_execution;
					}
					case(SPP_OPTION_PRIORITY_LOW): {
						append_saved_low_prio_option(((struct option_to_execute){
							.option = found_option,

							.used_short_identifier = found_option->short_identifier,
							.used_long_identifier = NULL,
							.arg = option_arg
						}));
					}
				}
			}

			continue;
		}

		if(no_bad_options()) {
			cli.input_files[cli.input_files_size] = *arg_it;
			++cli.input_files_size;
		}
	}

	#undef no_bad_options

	if(invalid_option_identifier.invalid_short_identifier != '\0' ||
	   invalid_option_identifier.invalid_long_identifier != NULL) {

		result->type = SPP_ARG_PARSING_RESULT_TYPE_INVALID_OPTION_IDENTIFIER;
		result->invalid_option_identifier = invalid_option_identifier;

		goto success_not_normal_execution;
	}

	if(missing_option_argument.arg_name != NULL) {
		missing_option_argument_result:

		result->type = SPP_ARG_PARSING_RESULT_TYPE_MISSING_OPTION_ARGUMENT;
		result->missing_option_argument = missing_option_argument;

		goto success_not_normal_execution;
	}

	if(excess_option_argument.short_identifier != '\0' ||
	   excess_option_argument.long_identifier != NULL) {
		excess_option_argument_result:

		result->type = SPP_ARG_PARSING_RESULT_TYPE_EXCESS_OPTION_ARGUMENT;
		result->excess_option_argument = excess_option_argument;

		goto success_not_normal_execution;
	}

	// executing all low priority options
	for(size_t i = 0; i < saved_low_prio_options_size; ++i) {
		exc = saved_low_prio_options[i].option->func((struct spp_option_func_input){
			.argv0 = argv[0],

			.short_identifier = saved_low_prio_options[i].used_short_identifier,
			.long_identifier = saved_low_prio_options[i].used_long_identifier,

			.arg = saved_low_prio_options[i].arg,

			.options_cli = &cli.options_cli
		});

		spp_if_unlikely(exc != 0) {
			result->type = SPP_ARG_PARSING_RESULT_TYPE_LOW_PRIO_FAILED;
			result->low_prio_failed = (struct spp_arg_parsing_result_low_prio_failed){
				.exc = exc
			};

			goto success_not_normal_execution;
		}
	}

	free(saved_low_prio_options);

	result->type = SPP_ARG_PARSING_RESULT_TYPE_NORMAL_EXECUTION;
	result->normal_execution = (struct spp_arg_parsing_result_normal_execution){
		.cli = cli
	};

	return SPP_RET_STATUS_SUCCESS;


	success_not_normal_execution:

	free(cli.input_files);
	free(saved_low_prio_options);

	return SPP_RET_STATUS_SUCCESS;
}
