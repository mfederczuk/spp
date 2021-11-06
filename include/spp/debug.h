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

#ifndef SPP_DEBUG_H
#define SPP_DEBUG_H

#ifndef NDEBUG
	#include <stdio.h>
	#define debug_log(message)      fputs(message "\n", stderr)
	#define debug_logf(format, ...) fprintf(stderr, format "\n", __VA_ARGS__)
#else
	#define debug_log(message)      ((void)0)
	#define debug_logf(format, ...) ((void)0)
#endif

#endif /* SPP_DEBUG_H */
