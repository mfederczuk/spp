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

#ifndef SPP_OPTIONS_H
#define SPP_OPTIONS_H

#include <spp/arg_parser.h>

int spp_option_help                 (struct spp_option_func_input);
int spp_option_version_info         (struct spp_option_func_input);
int spp_option_output               (struct spp_option_func_input);
int spp_option_output_mode_truncate (struct spp_option_func_input);
int spp_option_output_mode_append   (struct spp_option_func_input);

/** Sentintel item has the `func` field set to `NULL` and all other fields uninitialized. */
extern struct spp_option* spp_options;

#endif /* SPP_OPTIONS_H */
