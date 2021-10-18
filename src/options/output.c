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

#include <spp/options.h>
#include <stdio.h>

#define MESSAGE "argument may not be empty"

int spp_option_output(struct spp_option_func_input input) {
	if(input.arg[0] == '\0') {
		if(input.short_identifier != '\0') {
			fprintf(stderr, "-%c: " MESSAGE "\n", input.short_identifier);
		} else {
			fprintf(stderr, "--%s: " MESSAGE "\n", input.long_identifier);
		}

		return 9;
	}

	input.options_cli->output = input.arg;

	return 0;
}

#undef MESSAGE
