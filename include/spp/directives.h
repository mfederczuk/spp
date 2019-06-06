/*
 * Script Preprocessor.
 * Copyright (C) 2019 Michael Federczuk
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
// include/spp/spp_directives.h
/*
 * Header file for the directive functions.
 *
 * Since: v0.1.0 2019-06-05
 * LastEdit: 2019-06-06
 */

#ifndef _SPP_DIRECTIVES_H
#define _SPP_DIRECTIVES_H

#include <spp/spp.h>

enum spp_dir_func_ret {
	SPP_DIR_FUNC_SUCCESS,
	SPP_DIR_FUNC_INVALID,
	SPP_DIR_FUNC_ERROR
};

#define __tmp struct spp_stat* stat, FILE* out, cstr_t arg
typedef enum spp_dir_func_ret(*spp_dir_func_t)(__tmp);

enum spp_dir_func_ret spp_insert(__tmp);
enum spp_dir_func_ret spp_include(__tmp);
enum spp_dir_func_ret spp_ignore(__tmp);
enum spp_dir_func_ret spp_end_ignore(__tmp);
#undef __tmp

#define SPP_DIRS_AMOUNT 4
extern cstr_t spp_dirs_names[SPP_DIRS_AMOUNT];
extern spp_dir_func_t spp_dirs_funcs[SPP_DIRS_AMOUNT];

#endif /* _SPP_DIRECTIVES_H */
