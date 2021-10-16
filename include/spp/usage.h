/*
 * Script Preprocessor.
 * Copyright (C) 2019, 2021  Michael Federczuk
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
// include/spp/usage.h
/*
 * Macro definition for the --help usage summary.
 *
 * Since: v0.1.0 2019-06-07
 * LastEdit: 2021-10-16
 */

#ifndef _SPP_USAGE_H
#define _SPP_USAGE_H

#define USAGE \
	"usage: %s [<file>]\n" \
	"    Script preprocessor program.\n" \
	"    If FILE is omitted, read input from stdin.\n" \
	"\n" \
	"    Options:\n" \
	"      --help     display this summary and exit\n" \
	"      --version  display version and legal information and exit\n" \
	"\n" \
	"    Exit Status:\n" \
	"      (using CommonCodes v2 <https://mfederczuk.github.io/commoncodes/v2.html>)\n" \
	"      48  too many symbolic links encountered\n" \
	"      49  path name too long\n" \
	"\n" \
	"Report bugs to: <federczuk.michael@protonmail.com>\n" \
	"         or at: <https://github.com/mfederczuk/spp/issues>\n" \
	"GitHub repository: <https://github.com/mfederczuk/spp>\n"

#endif /* _SPP_USAGE_H */
