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

#define SPP_CHECKLN_DIR    0
#define SPP_CHECKLN_NO_DIR 1
#define SPP_CHECKLN_ERR_INV_ARGS 2
#define SPP_CHECKLN_ERR_NO_MEM   3

/**
 * Checks if the entered line contains a valid spp directive and saves the
 * directive command and the argument into the two parameters CMD and ARG.
 * Both CMD and ARG will need to be freed if a directive is found, regardless if
 * the directive contains an argument.
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
 *     0  entered line is a spp directive, CMD and ARG are allocated
 *         macro: SPP_CHECKLN_DIR
 *     1  entered line is not a spp directive, CMD and ARG are unchanged
 *         macro: SPP_CHECKLN_NO_DIR
 *     2  error: CMD and ARG parameters are invalid
 *         macro: SPP_CHECKLN_ERR_INV_ARGS
 *     3  error: not enough memory to allocated for buffers
 *         macro: SPP_CHECKLN_ERR_NO_MEM
 *
 * Since: v0.1.0 2019-05-25
 */
int checkln(wcstr line, wcstr* cmd, wcstr* arg);

/**
 * Reads and processes every line from the entered IN stream and writes the
 * final output to the OUT stream.
 *
 * Param FILE* in:
 *     The stream to read the input from until an WEOF character is encountered.
 *
 * Param FILE* out:
 *     The stream to write the processed output.
 *     Note that the stream will not get flushed.
 *
 * Since: v0.1.0 2019-05-26
 */
void process(FILE* in, FILE* out);

#endif /* _SPP_SPP_H */
