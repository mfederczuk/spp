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

#include <spp/options.h>
#include <stdio.h>

int spp_option_version_info(struct spp_option_func_input input) {
	(void)input;

	fputs(
		"spp v0.2.0\n"
		"Copyright (C) 2021 Michael Federczuk\n"
		"License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"
		"This is free software: you are free to change and redistribute it.\n"
		"There is NO WARRANTY, to the extent permitted by law.\n"
		"\n"
		"Written by Michael Federczuk.\n",
		stderr
	);

	return 0;
}