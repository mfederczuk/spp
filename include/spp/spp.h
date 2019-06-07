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
 * LastEdit: 2019-06-07
 */

#ifndef _SPP_SPP_H
#define _SPP_SPP_H

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
 *     On success, zero is returned. On error, a non-zero value is returned and
 *     errno is set appropriately.
 *
 * Errors:
 *     EINVAL  Arguments are invalid.
 *     ENOMEM  Not enough memory.
 *
 * Since: v0.1.0 2019-05-25
 */
int checkln(cstr_t line, cstr_t* cmd, cstr_t* arg);

enum _spp_processln_ret {
	SPP_PROCESSLN_SUCCESS,
	SPP_PROCESSLN_ERR_INV_ARGS,
	SPP_PROCESSLN_ERR_NO_MEM,
	SPP_PROCESSLN_ERR_STAT,
	SPP_PROCESSLN_ERR_FPUTS,
	SPP_PROCESSLN_ERR_FOPEN
};

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
 * Param struct spp_stat* stat:
 *     The data of the spp session.
 *
 * Return: int
 *     SPP_PROCESSLN_SUCCESS       successfull execution
 *     SPP_PROCESSLN_ERR_INV_ARGS  error: OUT or STAT parameters are invalid
 *     SPP_PROCESSLN_ERR_NO_MEM    error: not enough memory to allocate buffer
 *     SPP_PROCESSLN_ERR_STAT      error: a call to stat() failed; errno is set
 *                                  and should be checked
 *     SPP_PROCESSLN_ERR_FPUTS     error: fputs() returned EOF
 *     SPP_PROCESSLN_ERR_FOPEN     error: a call to fopen() failed; errno is set
 *                                  and should be checked
 *
 * Since: v0.1.0 2019-05-26
 */
int processln(cstr_t line, FILE* out, struct spp_stat* spp_statbuf);

enum _spp_process_ret {
	SPP_PROCESS_SUCCESS,
	SPP_PROCESS_ERR_INV_ARGS,
	SPP_PROCESS_ERR_NO_MEM,
	SPP_PROCESS_ERR_STAT,
	SPP_PROCESS_ERR_FPUTS,
	SPP_PROCESS_ERR_FOPEN
};

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
 *     SPP_PROCESS_SUCCESS       successfull execution
 *     SPP_PROCESS_ERR_INV_ARGS  error: IN or OUT parameters are invalid
 *     SPP_PROCESS_ERR_NO_MEM    error: not enough memory to allocate buffer
 *     SPP_PROCESS_ERR_STAT      error: a call to stat() failed; errno is set
 *                                and should be checked
 *     SPP_PROCESS_ERR_FPUTS     error: fputs() returned EOF
 *     SPP_PROCESS_ERR_FOPEN     error: a call to fopen() failed; errno is set
 *                                and should be checked
 *
 * Since: v0.1.0 2019-05-26
 */
int process(FILE* in, FILE* out, cstr_t pwd);

#endif /* _SPP_SPP_H */
