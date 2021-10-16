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

#ifndef SPP_SPP_H
#define SPP_SPP_H

#include <spp/types.h>
#include <stdio.h>

/**
 * State data of a single spp session.
 *
 * Since: v0.1.0 2019-05-26
 */
struct spp_stat {
	bool ignore;
	bool ignore_next;
	cstr_t pwd;
};

/**
 * Checks if the entered line contains a valid spp directive and saves the
 * directive command and the argument into the two dereferenced parameters CMD
 * and ARG.
 * Both dereferenced values of CMD and ARG will need to be freed if a directive
 * is found, regardless if the directive contains an argument.
 *
 * If the line is not a valid spp directive, the two dereferenced parameters
 * will be kept unchanged.
 *
 * Param cstr_t line:
 *     The line to check for a spp directive.
 *
 * Param cstr_t* cmd:
 *     Will be replaced with the directive command name.
 *     *cmd needs to be NULL.
 *
 * Param cstr_t* arg:
 *     Will be replaced with the directive command argument.
 *     *arg needs to be NULL.
 *
 * Return: int
 *     On success, zero is returned. On failure, a non-zero value is returned
 *     and errno is set appropriately.
 *
 * Errors:
 *     EINVAL  Arguments are invalid.
 *     ENOMEM  Not enough memory.
 *
 * Since: v0.1.0 2019-05-25
 */
int checkln(cstr_t line, cstr_t* cmd, cstr_t* arg);

/**
 * Processes a single line and writes it into the OUT stream.
 *
 * Param cstr_t line:
 *     Original line to process.
 *     Will be kept completely unchanged.
 *
 * Param FILE* out:
 *     Stream to write the processed line to.
 *
 * Param struct spp_stat* spp_stat:
 *     The data of the spp session.
 *
 * Return: int
 *     On success, zero is returned. On failure, a non-zero value is returned
 *     and errno is set appropriately.
 * 
 * Errors:
 *     Any errors specified in stat(2), fputs(3) or fopen(3).
 *     EINVAL  Arguments are invalid.
 *     ENOMEM  Not enough memory.
 *
 * Since: v0.1.0 2019-05-26
 */
int processln(cstr_t line, FILE* out, struct spp_stat* spp_stat);

/**
 * Reads and processes every line from the entered IN stream and writes the
 * final output to the OUT stream.
 *
 * Param FILE* in:
 *     The stream to read the input from until an EOF character is encountered.
 *
 * Param FILE* out:
 *     The stream to write the processed output.
 *     Note that the stream will not get flushed.
 *
 * Param cstr_t pwd:
 *     The directory to set the private working directory of the internal
 *     spp_stat structure to.
 *     Pass NULL to not change it.
 *
 * Return: int
 *     On success, zero is returned. On failure, a non-zero value is returned
 *     and errno is set appropriately.
 * 
 * Errors:
 *     Any errors specified in stat(2), fputs(3) or fopen(3).
 *     EINVAL  Arguments are invalid.
 *     ENOMEM  Not enough memory.
 *
 * Since: v0.1.0 2019-05-26
 */
int process(FILE* in, FILE* out, cstr_t pwd);

#endif /* SPP_SPP_H */
