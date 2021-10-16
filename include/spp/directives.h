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
// include/spp/spp_directives.h
/*
 * Header file for the directive functions.
 *
 * Since: v0.1.0 2019-06-05
 * LastEdit: 2019-06-07
 */

#ifndef SPP_DIRECTIVES_H
#define SPP_DIRECTIVES_H

#include <spp/spp.h>

/*
 * These functions return zero on success and a non-zero value if they failed.
 * If an error occurs then errno needs to be appropriately set.
 * If no error occurred, but the directive was still invalid, errno should not
 * be changed. It is important to set errno to 0 before calling these functions.
 */

#define __tmp struct spp_stat* stat, \
              FILE* out, \
              cstr_t arg

typedef int (*spp_dir_func_t)(__tmp);
int spp_insert(__tmp);
int spp_include(__tmp);
int spp_ignore(__tmp);
int spp_end_ignore(__tmp);
int spp_ignore_next(__tmp);

#undef __tmp

enum { SPP_DIRS_AMOUNT = 5 };
extern cstr_t spp_dirs_names[SPP_DIRS_AMOUNT];
extern spp_dir_func_t spp_dirs_funcs[SPP_DIRS_AMOUNT];

#endif /* SPP_DIRECTIVES_H */
