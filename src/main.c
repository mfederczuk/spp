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

#define _POSIX_C_SOURCE 200809L // for strdup(3)

#include <errno.h>
#include <libgen.h>
#include <spp/arg_parser.h>
#include <spp/compiler.h>
#include <spp/debug.h>
#include <spp/spp.h>
#include <spp/types.h>
#include <spp/utils.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <spp/memtrace.h>


#ifdef NDEBUG
	#define debug_perror(s) ((void)0)
#else
	#define debug_perror(s) perror(s)
#endif


static inline int main_open_output_file(
	const_cstr_t argv0,
	const_cstr_t output_file_path,
	enum spp_options_cli_output_mode output_mode,
	const_cstr_t* input_file_paths,
	size_t input_file_paths_size
);

static inline int main_open_input_files(
	const_cstr_t argv0,
	const_cstr_t* input_file_paths,
	size_t input_file_paths_size
);


static inline void print_invalid_option_identifier_message(
	const_cstr_t argv0,
	const struct spp_arg_parsing_result_invalid_option_identifier* invalid_option_identifier
);
static inline void print_missing_option_argument_message(
	const_cstr_t argv0,
	const struct spp_arg_parsing_result_missing_option_argument* missing_option_argument
);
static inline void print_excess_option_argument_message(
	const_cstr_t argv0,
	const struct spp_arg_parsing_result_excess_option_argument* excess_option_argument
);

static inline int print_stat_error_message(const_cstr_t argv0, const_cstr_t filename);
static inline int print_access_error_message(
	const_cstr_t argv0,
	const_cstr_t filename,
	const_cstr_t permission_denied_info
);
static inline int print_fopen_error_message(const_cstr_t argv0, cstr_t filename);


// using `spp_if_unlikely` in out of memory cases since this is actually *extremely* rare, and on Linux it doesn't
// happen at all anyway

int main(const int argc, const cstr_t* const argv) {
	struct spp_arg_parsing_result result;
	errno = 0;
	spp_if_unlikely(spp_parse_args(argc, argv, &result) != SPP_RET_STATUS_SUCCESS || errno != 0) {
		fprintf(stderr, "%s: not enough heap memory\n", argv[0]);
		return 101;
	}

	switch(result.type) {
		case(SPP_ARG_PARSING_RESULT_TYPE_HIGH_PRIO_EXECUTED): {
			return result.high_prio_executed.exc;
		}
		case(SPP_ARG_PARSING_RESULT_TYPE_INVALID_OPTION_IDENTIFIER): {
			print_invalid_option_identifier_message(argv[0], &result.invalid_option_identifier);
			return 5;
		}
		case(SPP_ARG_PARSING_RESULT_TYPE_MISSING_OPTION_ARGUMENT): {
			print_missing_option_argument_message(argv[0], &result.missing_option_argument);
			return 3;
		}
		case(SPP_ARG_PARSING_RESULT_TYPE_EXCESS_OPTION_ARGUMENT): {
			print_excess_option_argument_message(argv[0], &result.excess_option_argument);
			return 4;
		}
		case(SPP_ARG_PARSING_RESULT_TYPE_LOW_PRIO_FAILED): {
			return result.low_prio_failed.exc;
		}
		case(SPP_ARG_PARSING_RESULT_TYPE_NORMAL_EXECUTION): ;
	}


	const int exc = main_open_output_file(
		argv[0],
		result.normal_execution.cli.options_cli.output,
		result.normal_execution.cli.options_cli.output_mode,
		result.normal_execution.cli.input_files,
		result.normal_execution.cli.input_files_size
	);


	free(result.normal_execution.cli.input_files);


	return exc;
}

static inline int main_open_output_file(
	const const_cstr_t argv0,
	const const_cstr_t output_file_path,
	const enum spp_options_cli_output_mode output_mode,
	const_cstr_t* const input_file_paths,
	const size_t input_file_paths_size
) {
	FILE* output_stream;

	if(output_file_path == NULL) {
		debug_log("no output file specified; defaulting to stdout");
		output_stream = stdout;
	} else {
		struct stat statbuf;
		errno = 0;
		if(stat(output_file_path, &statbuf) == 0 && errno == 0) {
			// checking if the given output file is not a directory
			// we should be able work with everything except directories, so we just check for that
			if(S_ISDIR(statbuf.st_mode)) {
				fprintf(stderr, "%s: %s: not a file\n", argv0, output_file_path);
				return 26;
			}

			// checking if the current user has access to the given output file
			errno = 0;
			if(access(output_file_path, W_OK) != 0 || errno != 0) {
				return print_access_error_message(
					argv0,
					output_file_path,
					"write permission missing"
				);
			}
		} else if(errno == ENOENT) {
			// stat(2) fails with ENOENT when a component of the pathname doesn't exist, so we gotta check if just
			// the file itself doesn't exist or the directory that was given

			errno = 0;
			const cstr_t output_file_path_dup = strdup(output_file_path);
			spp_if_unlikely(output_file_path_dup == NULL || errno != 0) {
				fprintf(stderr, "%s: not enough heap memory\n", argv0);
				return 101;
			}

			const const_cstr_t output_file_parent_dir_path = dirname(output_file_path_dup);

			errno = 0;
			if(stat(output_file_parent_dir_path, &statbuf) != 0 || errno != 0) {
				const int exc = print_stat_error_message(argv0, output_file_parent_dir_path);
				free(output_file_path_dup);
				return exc;
			}

			if(!S_ISDIR(statbuf.st_mode)) {
				fprintf(stderr, "%s: %s: not a directory\n", argv0, output_file_parent_dir_path);
				free(output_file_path_dup);
				return 26;
			}

			errno = 0;
			if(access(output_file_parent_dir_path, W_OK | X_OK) != 0 || errno != 0) {
				const int exc = print_access_error_message(
					argv0,
					output_file_parent_dir_path,
					"write or search permission missing"
				);

				free(output_file_path_dup);

				return exc;
			}

			free(output_file_path_dup);

			debug_log("output file does not exist; will be created");
		} else {
			return print_stat_error_message(argv0, output_file_path);
		}

		// actually opening the output file
		errno = 0;
		switch(output_mode) {
			case(SPP_OPTIONS_CLI_OUTPUT_MODE_TRUNCATE): {
				debug_log("truncating output file");
				output_stream = fopen(output_file_path, "w");
				break;
			}
			case(SPP_OPTIONS_CLI_OUTPUT_MODE_APPEND): {
				debug_log("appending to output file");
				output_stream = fopen(output_file_path, "a");
				break;
			}
		}

		if(output_stream == NULL || errno != 0) {
			// casting `const_cstr_t` to `cstr_t` because fuck const-safety; exiting the program after this point anyway
			return print_fopen_error_message(argv0, (cstr_t)output_file_path);
		}
	}


	const int exc = main_open_input_files(
		argv0,
		input_file_paths,
		input_file_paths_size
	);


	if(output_stream != stdout) {
		fclose(output_stream);
	}


	return exc;
}

static inline int main_open_input_files(
	const const_cstr_t argv0,
	const_cstr_t* const input_file_paths,
	const size_t input_file_paths_size
) {
	struct spp_input_file* opened_input_files;

	if(input_file_paths_size == 0) {
		debug_log("no input files specified; defaulting to stdin");
		opened_input_files = NULL;
	} else {
		errno = 0;
		opened_input_files = malloc(sizeof(*opened_input_files) * (input_file_paths_size + 1));
		spp_if_unlikely(opened_input_files == NULL || errno == ENOMEM) {
			fprintf(stderr, "%s: not enough memory\n", argv0);
			return 101;
		}

		struct spp_input_file* opened_input_file_it = opened_input_files;
		bool has_stdin = false;
		const_cstr_t input_file;
		struct stat statbuf;

		for(size_t i = 0; i < input_file_paths_size; ++i) {
			input_file = input_file_paths[i];
			if(strcmp(input_file, "-") == 0) {
				if(!has_stdin) {
					debug_log("first '-' argument encountered; adding stdin as input file");
					// only the first "-" argument will be used, any subsequence ones will just be ignored
					has_stdin = true;
					*opened_input_file_it = (struct spp_input_file){ .stream = stdin, .filename = NULL };
					++opened_input_file_it;
				}
				#ifndef NDEBUG
				  else {
					debug_log("subsequent '-' argument ignored");
				}
				#endif
			} else {
				errno = 0;
				if(stat(input_file, &statbuf) != 0 || errno != 0) {
					errno_push();

					--opened_input_file_it;
					for(; opened_input_file_it >= opened_input_files; --opened_input_file_it) {
						if(opened_input_file_it->stream != stdin) {
							fclose(opened_input_file_it->stream);
						}
					}
					free(opened_input_files);

					errno_pop();

					return print_stat_error_message(argv0, input_file);
				}

				if(S_ISDIR(statbuf.st_mode)) {
					--opened_input_file_it;
					for(; opened_input_file_it >= opened_input_files; --opened_input_file_it) {
						if(opened_input_file_it->stream != stdin) {
							fclose(opened_input_file_it->stream);
						}
					}
					free(opened_input_files);

					fprintf(stderr, "%s: %s: not a file\n", argv0, input_file);

					return 26;
				}


				errno = 0;
				if(access(input_file, R_OK) != 0 || errno != 0) {
					errno_push();

					--opened_input_file_it;
					for(; opened_input_file_it >= opened_input_files; --opened_input_file_it) {
						if(opened_input_file_it->stream != stdin) {
							fclose(opened_input_file_it->stream);
						}
					}
					free(opened_input_files);

					errno_pop();

					return print_access_error_message(
						argv0,
						input_file,
						"read permission missing"
					);
				}


				errno = 0;
				*opened_input_file_it = (struct spp_input_file){
					.stream = fopen(input_file, "r"),
					.filename = input_file
				};

				if(opened_input_file_it->stream == NULL || errno != 0) {
					errno_push();

					--opened_input_file_it;
					for(; opened_input_file_it >= opened_input_files; --opened_input_file_it) {
						if(opened_input_file_it->stream != stdin) {
							fclose(opened_input_file_it->stream);
						}
					}
					free(opened_input_files);

					errno_pop();

					// once again; fuck const-safety
					return print_fopen_error_message(argv0, (cstr_t)input_file);
				}

				++opened_input_file_it;
			}
		}

		*opened_input_file_it = (struct spp_input_file){ .stream = NULL };

		#ifndef NDEBUG
		const size_t opened_input_files_size = (opened_input_file_it - opened_input_files);
		if(input_file_paths_size == opened_input_files_size) {
			debug_logf("%zu input file(s) opened", opened_input_files_size);
		} else {
			debug_logf(
				"%zu given input file(s);  %zu actually opened",
				input_file_paths_size,
				opened_input_files_size
			);
		}
		#endif
	}


	// TODO process files


	if(input_file_paths_size > 0) {
		for(struct spp_input_file* opened_input_file_it = opened_input_files; opened_input_file_it->stream != NULL; ++opened_input_file_it) {
			if(opened_input_file_it->stream != stdin) {
				fclose(opened_input_file_it->stream);
			}
		}

		free(opened_input_files);
	}

	return 0;
}


static inline void print_invalid_option_identifier_message(
	const const_cstr_t argv0,
	const struct spp_arg_parsing_result_invalid_option_identifier* const invalid_option_identifier
) {
	#define MESSAGE "invalid option"

	if(invalid_option_identifier->invalid_short_identifier != '\0') {
		fprintf(
			stderr,
			"%s: -%c: " MESSAGE "\n",
			argv0,
			invalid_option_identifier->invalid_short_identifier
		);
		return;
	}

	fprintf(
		stderr,
		"%s: --%s: " MESSAGE "\n",
		argv0,
		invalid_option_identifier->invalid_long_identifier
	);

	#undef MESSAGE
}

static inline void print_missing_option_argument_message(
	const const_cstr_t argv0,
	const struct spp_arg_parsing_result_missing_option_argument* const missing_option_argument
) {
	#define MESSAGE "missing argument: <%s>"

	if(missing_option_argument->short_identifier != '\0') {
		fprintf(
			stderr,
			"%s: -%c: " MESSAGE "\n",
			argv0,
			missing_option_argument->short_identifier,
			missing_option_argument->arg_name
		);
		return;
	}

	fprintf(
		stderr,
		"%s: --%s: " MESSAGE "\n",
		argv0,
		missing_option_argument->long_identifier,
		missing_option_argument->arg_name
	);

	#undef MESSAGE
}

static inline void print_excess_option_argument_message(
	const const_cstr_t argv0,
	const struct spp_arg_parsing_result_excess_option_argument* const excess_option_argument
) {
	#define MESSAGE "too many arguments: 1"

	if(excess_option_argument->short_identifier != '\0') {
		fprintf(
			stderr,
			"%s: -%c: " MESSAGE "\n",
			argv0,
			excess_option_argument->short_identifier
		);
		return;
	}

	fprintf(
		stderr,
		"%s: --%s: " MESSAGE "\n",
		argv0,
		excess_option_argument->long_identifier
	);

	#undef MESSAGE
}

static inline int print_stat_error_message(const const_cstr_t argv0, const const_cstr_t filename) {
	switch(errno) {
		case(EACCES): {
			// TODO should display *which* component of the pathname the user has insufficient permissions
			fprintf(
				stderr,
				"%s: %s: permission denied: search permission missing\n",
				argv0,
				filename
			);
			return 77;
		}
		case(ELOOP): {
			fprintf(
				stderr,
				"%s: %s: too many levels of symbolic links\n",
				argv0,
				filename
			);
			return 98;
		}
		case(ENAMETOOLONG): {
			fprintf(
				stderr,
				"%s: %s: filename too long\n",
				argv0,
				filename
			);
			return 99;
		}
		case(ENOENT): {
			// TODO should display *which* component of the pathname does not exist
			fprintf(
				stderr,
				"%s: %s: no such file or directory\n",
				argv0,
				filename
			);
			return 24;
		}
		case(ENOMEM): {
			fprintf(stderr, "%s: not enough kernel memory\n", argv0);
			return 101;
		}
		case(ENOTDIR): {
			// TODO should display *which* component of the pathname is not a directory
			fprintf(
				stderr,
				"%s: %s: not a directory\n",
				argv0,
				filename
			);
			return 26;
		}
		case(EOVERFLOW): {
			fprintf(
				stderr,
				"%s: %s: file too big\n",
				argv0,
				filename
			);
			return 48;
		}
		default: {
			debug_errno_push();

			fprintf(stderr, "%s: unknown error\n", argv0);

			debug_errno_pop();

			debug_perror("stat");

			return 125;
		}
	}
}

static inline int print_access_error_message(
	const const_cstr_t argv0,
	const const_cstr_t filename,
	const const_cstr_t permission_denied_info
) {
	switch(errno) {
		case(EACCES):
		case(EROFS): {
			fprintf(
				stderr,
				"%s: %s: permission denied: %s\n",
				argv0,
				filename,
				permission_denied_info
			);
			return 77;
		}
		case(ELOOP): {
			fprintf(
				stderr,
				"%s: %s: too many levels of symbolic links\n",
				argv0,
				filename
			);
			return 98;
		}
		case(ENAMETOOLONG): {
			fprintf(
				stderr,
				"%s: %s: filename too long\n",
				argv0,
				filename
			);
			return 98;
		}
		case(ENOENT):
		case(ENOTDIR): {
			fprintf(
				stderr,
				"%s: %s: no such file\n",
				argv0,
				filename
			);
			return 24;
		}
		case(EIO): {
			fprintf(stderr, "%s: input/output error\n", argv0);
			return 74;
		}
		case(ENOMEM): {
			fprintf(stderr, "%s: not enough kernel memory\n", argv0);
			return 101;
		}
		case(ETXTBSY): {
			fprintf(
				stderr,
				"%s: %s: executable file is busy\n",
				argv0,
				filename
			);
			return 49;
		}
		default: {
			debug_errno_push();

			fprintf(stderr, "%s: unknown error\n", argv0);

			debug_errno_pop();

			debug_perror("access");

			return 125;
		}
	}
}

static inline int print_fopen_error_message(const const_cstr_t argv0, const cstr_t filename) {
	switch(errno) {
		// no need to check EACCES - already tested for it before
		case(EFBIG):
		case(EOVERFLOW): {
			fprintf(
				stderr,
				"%s: %s: file too big\n",
				argv0,
				filename
			);
			return 48;
		}
		case(EINTR): {
			fprintf(stderr, "%s: interrupted by signal\n", argv0);
			return 50;
		}
		case(EINVAL): {
			fprintf(
				stderr,
				"%s: %s: invalid filename\n",
				argv0,
				basename(filename)
			);
			return 51;
		}
		case(ELOOP): {
			fprintf(
				stderr,
				"%s: %s: too many levels of symbolic links\n",
				argv0,
				filename
			);
			return 98;
		}
		case(ENAMETOOLONG): {
			fprintf(
				stderr,
				"%s: %s: filename too long\n",
				argv0,
				filename
			);
			return 98;
		}
		// no need to check ENOENT - already tested for it before
		case(ENOMEM): {
			fprintf(stderr, "%s: not enough memory\n", argv0);
			return 101;
		}
		case(ENOSPC): {
			fprintf(stderr, "%s: not enough disk space\n", argv0);
			return 52;
		}
		// no need to check ENOTDIR - already tested for it before
		case(EPERM): {
			fprintf(
				stderr,
				"%s: %s: permission denied\n",
				argv0,
				filename
			);
			return 77;
		}
		case(EROFS): {
			fprintf(
				stderr,
				"%s: %s: permission denied: read-only filesystem\n",
				argv0,
				filename
			);
			return 77;
		}
		case(ETXTBSY): {
			fprintf(
				stderr,
				"%s: %s: file is busy\n",
				argv0,
				filename
			);
			return 49;
		}
		default: {
			debug_errno_push();

			fprintf(stderr, "%s: unknown error\n", argv0);

			debug_errno_pop();

			debug_perror("open");

			return 125;
		}
	}
}
