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

#include <ctype.h>
#include <errno.h>
#include <spp/debug.h>
#include <spp/spp.h>
#include <spp/types.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <spp/memtrace.h>

enum {
	SPP_LINE_BUFFER_INITIAL_CAPACITY = 80
};
#define SPP_LINE_BUFFER_GROWTH_FACTOR 1.5

#define SPP_DIRECTIVE_PREFIX_CHAR '#'

/**
 * The internal state and settins of an `spp` process.
 * The same instance will be passed to every file processed.
 */
struct spp_state {
	size_t excluded_n;
};

struct spp_line {
	char* buf; // not *always* NUL-terminated!
	size_t cap; // the range of [buf, buf + cap + 1) is allocated data (+1 for the optional terminating NUL-byte)
	size_t size; // the range of [buf, buf + size) is initialized data
};

static inline spp_ret_status_t spp_line_grow_if_necessary(struct spp_line* line, size_t additional_length) {
	if((line->size + additional_length) <= line->cap) return SPP_RET_STATUS_SUCCESS;

	// TODO: there's no need to loop - this can be done with tHE POWER OF MATH!!!!!
	do {
		line->cap *= SPP_LINE_BUFFER_GROWTH_FACTOR;
	} while((line->size + additional_length) > line->cap);

	errno = 0;
	char* const tmp = realloc(line->buf, sizeof(*(line->buf)) * (line->cap + 1));
	if(__glibc_unlikely(tmp == NULL || errno != 0)) {
		return SPP_RET_STATUS_FAILURE;
	}

	line->buf = tmp;

	return SPP_RET_STATUS_SUCCESS;

}

static spp_ret_status_t spp_process_file(
	const struct spp_input_file* input_file,
	FILE* output_stream,
	struct spp_state* state,
	struct spp_line* line
);

spp_ret_status_t spp_process_files(const struct spp_input_file* input_files, FILE* const output_stream) {
	struct spp_state state = {
		.excluded_n = 0
	};
	errno = 0;
	struct spp_line line = {
		.buf = malloc(sizeof(*line.buf) * (SPP_LINE_BUFFER_INITIAL_CAPACITY + 1)),
		.cap = SPP_LINE_BUFFER_INITIAL_CAPACITY,
		.size = 0
	};

	if(__glibc_unlikely(line.buf == NULL || errno != 0)) {
		return SPP_RET_STATUS_FAILURE;
	}

	spp_ret_status_t s;
	for(const struct spp_input_file* input_file_it = input_files; input_file_it->stream != NULL; ++input_file_it) {
		s = spp_process_file(
			input_file_it,
			output_stream,
			&state,
			&line
		);

		if(s != SPP_RET_STATUS_SUCCESS) {
			free(line.buf);
			return SPP_RET_STATUS_FAILURE;
		}
	}

	free(line.buf);

	return SPP_RET_STATUS_SUCCESS;
}

static spp_ret_status_t spp_process_file(
	const struct spp_input_file* input_file,
	FILE* const output_stream,
	struct spp_state* const state,
	struct spp_line* const line
) {
	#ifndef NDEBUG
	if(input_file->filename != NULL) {
		debug_logf("processing file '%s'", input_file->filename);
	} else {
		debug_log("processing stdin");
	}
	#endif

	// TODO don't read one byte at a time; read into a buffer (fgets(3) ?)
	int c;
	size_t i;
	while((c = fgetc(input_file->stream)) != EOF) {
		if(c == '\n') {
			// TODO error handling
			if(line->size > 0) {
				// skips leading ws
				for(i = 0; i < line->size && isspace(line->buf[i]); ++i);

				// TODO cleanup, optimization, ,,,
				if(line->buf[i] == SPP_DIRECTIVE_PREFIX_CHAR && (++i, i < line->size)) {
					// line starts with prefix and is not at the end of line - could be a directive

					const size_t name_start = i;
					for(; i < line->size && isalpha(line->buf[i]); ++i);
					const size_t name_end = i;

					if(name_start != name_end) {
						// name is ok - could be a directive

						size_t arg_start = -1;

						if(i < line->size) {
							// skips ws
							for(; i < line->size && isspace(line->buf[i]); ++i);
							arg_start = i;
						}

						if(name_end == arg_start) {
							// not a directive - non-ws after directive name
						} else {
							size_t arg_end;
							if(arg_start != (size_t)-1) {
								if(arg_start == line->size) {
									arg_end = arg_start;
								} else {
									arg_end = line->size;
									for(; (arg_end - 1) > arg_start && isspace(line->buf[arg_end - 1]); --arg_end);
								}
							} else {
								arg_end = (arg_start = line->size);
							}

							line->buf[name_end] = '\0'; // line->buf[name_start] is now a NUL-terminated string!
							line->buf[arg_end] = '\0'; // line->buf[arg_start] is now a NUL-terminated string!
							debug_logf(
								"directive found!  name: '%s'  arg: '%s'",
								line->buf + name_start,
								line->buf + arg_start
							);
						}
					}
				}

				fwrite(line->buf, sizeof(*(line->buf)), line->size, output_stream);
				line->size = 0;
			}

			fputc('\n', output_stream);

			continue;
		}

		if(spp_line_grow_if_necessary(line, 1) != SPP_RET_STATUS_SUCCESS) {
			return SPP_RET_STATUS_FAILURE;
		}

		line->buf[line->size] = c;
		++(line->size);
	}

	if(ferror(input_file->stream) != 0) {
		errno = EIO;
		return SPP_RET_STATUS_FAILURE;
	}

	return SPP_RET_STATUS_SUCCESS;
}
