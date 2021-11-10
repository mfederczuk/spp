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

#ifndef SPP_SPP_H
#define SPP_SPP_H

#include <spp/types.h>
#include <stdio.h>

struct spp_input_file {
	FILE* stream;
	/** Will be ignored when the `stream` field is `stdin`. */
	const_cstr_t filename;
};

/**
 * Process each file in `input_files` and write the output to `output_stream`.
 *
 * @param input_files
 *        Sentinel item has the `stream` field set to `NULL` and all other fields uninitialized.
 */
spp_ret_status_t spp_process_files(const struct spp_input_file* input_files, FILE* output_stream);

#endif /* SPP_SPP_H */
