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

int spp_option_help(struct spp_option_func_input input) {
	fprintf(
		stderr,
		"usage: %s [-o <output_file>] [<file>...]\n"
		"    Script Preprocessor - a general-purpose preprocessor program.\n"
		"\n"
		"    Process each FILE given and output them all, concatenated, to stdout.\n"
		"    If no FILEs are given, read from stdin instead.\n"
		"    The first '-' argument seen will read from stdin, any subsequent '-' arguments are ignored.\n"
		"    If an actual file named '-' needs to be read, use either an absolute path or './-'.\n"
		"\n"
		"    Errors/warnings/notes are written to stderr.\n"
		"\n"
		"    Options:\n"
		"      -o, --output=<file>  Redirect output to FILE.\n"
		"                           If it does not exist, it is created\n"
		"\n"
		"      -t, --truncate  Truncate the output file (default) (only sensible when used with --output)\n"
		"      -a, --append    Append to the output file instead of truncating (only sensible when used with --output)\n"
		"\n"
		"      -h, --help     Display this summary and exit\n"
		"      -V, --version  Display version and legal information and exit\n"
		"\n"
		"    Exit Status:\n"
		"      (using CommonCodes v2 <https://mfederczuk.github.io/commoncodes/v2.html>)\n"
		"      48  A file is too big\n"
		"      49  A file is busy\n"
		"      50  Interrupted by signal\n"
		"      51  Invalid filename\n"
		"      52  Not enough disk space available\n"
		"\n"
		"Report bugs at: <https://github.com/mfederczuk/spp/issues>\n"
		"         or to: <federczuk.michael@protonmail.com>\n"
		"GitHub repository: <https://github.com/mfederczuk/spp>\n",
		input.argv0
	);

	return 0;
}
