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

#include <errno.h>
#include <spp/arg_parser.h>
#include <spp/compiler.h>
#include <spp/types.h>
#include <stdio.h>
#include <stdlib.h>


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

	// TODO

	free(result.normal_execution.cli.input_files);

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
