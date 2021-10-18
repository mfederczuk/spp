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

static struct spp_option spp_options_array[] = {
	{ 'h', "help",     NULL,   SPP_OPTION_PRIORITY_HIGH, spp_option_help                 },
	{ 'V', "version",  NULL,   SPP_OPTION_PRIORITY_HIGH, spp_option_version_info         },
	{ 'o', "output",   "file", SPP_OPTION_PRIORITY_LOW,  spp_option_output               },
	{ 't', "truncate", NULL,   SPP_OPTION_PRIORITY_LOW,  spp_option_output_mode_truncate },
	{ 'a', "append",   NULL,   SPP_OPTION_PRIORITY_LOW,  spp_option_output_mode_append   },
	{ .func = NULL }
};

struct spp_option* spp_options = spp_options_array;
