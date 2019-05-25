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
// include/spp/spp.h
/*
 * Header file for the core spp functions.
 *
 * Since: v0.1.0 2019-05-25
 * LastEdit: 2019-05-25
 */

#ifndef _SPP_SPP_H
#define _SPP_SPP_H

#include <spp/types.h>
#include <stdio.h>

/**
 * Checks if the entered line contains a valid spp directive and saves the
 * directive command and the argument into the two parameters CMD and ARG.
 *
 * If the line is not a valid spp directive, the two parameters will be kept
 * unchanged.
 *
 * The function will cancel before doing anything if CMD or ARG are NULL
 * pointers or if CMD or ARG are not pointing to NULL pointers.
 *
 * Param wcstr line:
 *     The line to check for a spp directive.
 *
 * Param wcstr* cmd:
 *     Will be replaced with the directive command name.
 *
 * Param wcstr* arg:
 *     Will be replaced with the directive command argument.
 * 
 * Return: int
 *     0  successfull execution
 *     1  unsucessfull execution
 *
 * Since: v0.1.0 2019-05-25
 */
int checkln(wcstr line, wcstr* cmd, wcstr* arg);

#endif /* _SPP_SPP_H */
