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
// include/spp/util.h
/*
 * Header file for utility functions.
 *
 * Since: v0.1.0 2019-05-24
 * LastEdit: 2019-05-24
 */

#ifndef _SPP_UTILS_H
#define _SPP_UTILS_H

#include <spp/types.h>

/**
 * Checks if the entered wide char is a whitespace character.
 *
 * Param wchar_t wc:
 *     The wide char to check.
 *
 * Return: bool
 *     true if the entered character is a whitespace character, false if
 *     otherwise.
 *
 * Since: v0.1.0 2019-05-24
 */
bool isws(wchar_t wc);

#endif /* _SPP_UTILS_H */
