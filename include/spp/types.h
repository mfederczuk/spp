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

#ifndef SPP_TYPES_H
#define SPP_TYPES_H

typedef char* cstr_t;

typedef const char* const_cstr_t;

typedef enum {
	SPP_RET_STATUS_SUCCESS = 0,
	SPP_RET_STATUS_FAILURE = 1
} spp_ret_status_t;

#endif /* SPP_TYPES_H */
