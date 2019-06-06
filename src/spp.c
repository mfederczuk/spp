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
// src/spp.c
/*
 * spp.h implementation.
 * Source file for the core spp functions.
 *
 * Since: v0.1.0 2019-05-25
 * LastEdit: 2019-06-06
 */

#include <spp/spp.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <spp/utils.h>
#include <spp/directives.h>

#define STEP_PRE_DIR 0 // whitespace before directive
#define STEP_DIR_CMD 1 // directive command
#define STEP_DIR_PRE_ARG 2 // whitespace before directive command argument
#define STEP_DIR_ARG 3 // directive command argument

#define CMD_BUF_GROW 1.25
#define ARG_BUF_GROW 1.25

int checkln(cstr_t line, cstr_t* cmd, cstr_t* arg) {
	if(cmd == NULL || arg == NULL) return SPP_CHECKLN_ERR_INV_ARGS;
	if(*cmd != NULL || *arg != NULL) return SPP_CHECKLN_ERR_INV_ARGS;

	size_t lcmd_size = 16, lcmd_len = 0;
	errno = 0;
	cstr_t lcmd = malloc(CHAR_SIZE * lcmd_size);
	if(lcmd == NULL || errno == ENOMEM) return SPP_CHECKLN_ERR_NO_MEM;

	size_t larg_size = 16, larg_len = 0;
	errno = 0;
	cstr_t larg = malloc(CHAR_SIZE * larg_size);
	if(larg == NULL || errno == ENOMEM) {
		free(lcmd);
		return SPP_CHECKLN_ERR_NO_MEM;
	}

	unsigned char step = STEP_PRE_DIR;
	for(size_t i = 0, l = strlen(line); i < l; ++i) {
		switch(step) {
		case STEP_PRE_DIR: {
			if(isws(line[i])) { // pre directive whitespace
				continue;
			} else if(line[i] == '#') { // directive begins
				step = STEP_DIR_CMD;
			} else { // line is no directive
				free(lcmd);
				free(larg);
				return SPP_CHECKLN_NO_DIR;
			}
			break;
		}
		case STEP_DIR_CMD: {
			if(isws(line[i])) { // command name ends
				step = STEP_DIR_PRE_ARG;
			} else { // command name continues
				if(lcmd_len + (CHAR_SIZE * 2) > lcmd_size) { // grow buffer
					errno = 0;
					cstr_t tmp = realloc(lcmd,
					                     CHAR_SIZE * (lcmd_size *= CMD_BUF_GROW));
					if(tmp == NULL || errno == ENOMEM) {
						free(lcmd);
						free(larg);
						return SPP_CHECKLN_ERR_NO_MEM;
					}
					lcmd = tmp;
				}
				lcmd[lcmd_len] = line[i];
				++lcmd_len;
			}
			break;
		}
		case STEP_DIR_PRE_ARG: {
			if(isws(line[i])) { // pre argument whitespace
				continue;
			} else { // argument begins
				// no need to check if the buffer is too small, this is always
				// the first character i.e.: it will always have space
				larg[0] = line[i];
				larg_len = 1;
				step = STEP_DIR_ARG;
			}
			break;
		}
		case STEP_DIR_ARG: {
			if(i + 1 < l && line[i] != '\n') {
				// only add char if it isn't at the end of the string and it
				// isn't a newline character
				if(larg_len + (CHAR_SIZE * 2) > larg_size) { // grow buffer
					errno = 0;
					cstr_t tmp = realloc(larg,
					                     CHAR_SIZE * (larg_size *= ARG_BUF_GROW));
					if(tmp == NULL || errno == ENOMEM) {
						free(lcmd);
						free(larg);
						return SPP_CHECKLN_ERR_NO_MEM;
					}
					larg = tmp;
				}
				larg[larg_len] = line[i];
				++larg_len;
			}
			break;
		}
		}
	}

	if(step == STEP_PRE_DIR) {
		free(lcmd);
		free(larg);
		return SPP_CHECKLN_NO_DIR;
	}

	if(lcmd_len + CHAR_SIZE < lcmd_size) { // shorten buffer
		errno = 0;
		cstr_t tmp = realloc(lcmd, CHAR_SIZE * (lcmd_size = lcmd_len + CHAR_SIZE));
		if(tmp == NULL || errno == ENOMEM) {
			free(lcmd);
			free(larg);
			return SPP_CHECKLN_ERR_NO_MEM;
		}
		lcmd = tmp;
	}

	lcmd[lcmd_len] = '\0';

	if(larg_len + CHAR_SIZE < larg_size) { // shorten buffer
		errno = 0;
		cstr_t tmp = realloc(larg, CHAR_SIZE * (larg_size = larg_len + CHAR_SIZE));
		if(tmp == NULL || errno == ENOMEM) {
			free(lcmd);
			free(larg);
			return SPP_CHECKLN_ERR_NO_MEM;
		}
		larg = tmp;
	}

	larg[larg_len] = '\0';

	*cmd = lcmd;
	*arg = larg;
	return SPP_CHECKLN_DIR;
}

#undef STEP_PRE_DIR
#undef STEP_DIR_CMD
#undef STEP_DIR_PRE_ARG
#undef STEP_DIR_ARG

#undef CMD_BUF_GROW
#undef ARG_BUF_GROW

int processln(cstr_t line, FILE* out, struct spp_stat* spp_stat) {
	if(out == NULL || spp_stat == NULL) return SPP_PROCESSLN_ERR_INV_ARGS;

	cstr_t cmd = NULL, arg = NULL;
	switch(checkln(line, &cmd, &arg)) {
	case SPP_CHECKLN_DIR: {
		bool valid_cmd = true;

		bool dir_found = false;
		enum spp_dir_func_ret ret;
		for(size_t i = 0; i < SPP_DIRS_AMOUNT; ++i) {
			if(strcmp(cmd, spp_dirs_names[i]) == 0) {
				ret = spp_dirs_funcs[i](spp_stat, out, arg);
				dir_found = true;
				break;
			}
		}

		free(cmd);
		free(arg);

		if(dir_found && ret != SPP_DIR_FUNC_INVALID) {
			if(ret == SPP_DIR_FUNC_ERROR) {
				// TODO: directive error handling
			}
			break;
		}
	}
	case SPP_CHECKLN_NO_DIR: {
		if(!spp_stat->ignore) {
			errno = 0;
			int exc = fputs(line, out);
			if(exc < 0 || exc == EOF) {
				return SPP_PROCESSLN_ERR_FPUTS;
			}
		}
		break;
	}
	case SPP_CHECKLN_ERR_NO_MEM:
		return SPP_PROCESSLN_ERR_NO_MEM;
	}

	return SPP_PROCESSLN_SUCCESS;
}

#define LINE_BUF_GROW 1.25
#define LINE_BUF_INIT_SIZE 64

int process(FILE* in, FILE* out, cstr_t pwd) {
	if(in == NULL || out == NULL) return SPP_PROCESS_ERR_INV_ARGS;

	// initial allocation for the line buffer
	size_t size = LINE_BUF_INIT_SIZE, len = 0;
	errno = 0;
	cstr_t line = malloc(CHAR_SIZE * size);
	if(line == NULL || errno == ENOMEM) return SPP_PROCESS_ERR_NO_MEM;

	// creating the spp_stat struct
	struct spp_stat stat = {
		.ignore = false,
		.pwd = NULL
	};
	if(pwd == NULL) {
		pwd = getenv("PWD"); // default spp pwd is the program pwd
		// if for some reason PWD doesn't exist, set spp pwd to root
		if(pwd == NULL) pwd = "/";
	}
	errno = 0;
	stat.pwd = malloc(CHAR_SIZE * (strlen(pwd) + 1));
	if(stat.pwd == NULL || errno == ENOMEM) {
		free(line);
		return SPP_PROCESS_ERR_NO_MEM;
	}
	strcpy(stat.pwd, pwd);

	// read stream
	bool read = true;
	for(int ch = fgetc(in);
	        read; ch = fgetc(in)) {

		if(ch != EOF) {
			// build line
			if(len + (CHAR_SIZE * 2) > size) { // grow buffer
				errno = 0;
				cstr_t tmp = realloc(line, CHAR_SIZE * (size *= LINE_BUF_GROW));
				if(tmp == NULL || errno == ENOMEM) {
					free(line);
					free(stat.pwd);
					return SPP_PROCESS_ERR_NO_MEM;
				}
				line = tmp;
			}
			line[len] = ch;
			++len;
		} else {
			read = false;
		}

		if(ch == EOF || ch == '\n') {
			// finish up building line
			if(len + CHAR_SIZE < size) { // shorten buffer
				errno = 0;
				cstr_t tmp = realloc(line, CHAR_SIZE * (size = len + CHAR_SIZE));
				if(tmp == NULL || errno == ENOMEM) {
					free(line);
					free(stat.pwd);
					return SPP_PROCESS_ERR_NO_MEM;
				}
				line = tmp;
			}
			line[len] = '\0';

			// work with line
			errno = 0;
			switch(processln(line, out, &stat)) {
			case SPP_PROCESSLN_ERR_NO_MEM: {
				free(line);
				free(stat.pwd);
				return SPP_PROCESS_ERR_NO_MEM;
			}
			case SPP_PROCESSLN_ERR_STAT: {
				int tmp = errno;
				free(line);
				free(stat.pwd);
				errno = tmp;
				return SPP_PROCESS_ERR_STAT;
			}
			case SPP_PROCESSLN_ERR_FPUTS: {
				free(line);
				free(stat.pwd);
				return SPP_PROCESS_ERR_FPUTS;
			}
			case SPP_PROCESSLN_ERR_FOPEN: {
				int tmp = errno;
				free(line);
				free(stat.pwd);
				errno = tmp;
				return SPP_PROCESS_ERR_FOPEN;
			}
			}

			// reset line
			if(size != LINE_BUF_INIT_SIZE) {
				// shrink buffer to init size if it was grown
				errno = 0;
				cstr_t tmp = realloc(line, CHAR_SIZE * (size = LINE_BUF_INIT_SIZE));
				if(tmp == NULL || errno == ENOMEM) {
					free(line);
					free(stat.pwd);
					return SPP_PROCESS_ERR_NO_MEM;
				}
				line = tmp;
			}
			len = 0;
		}
	} // end for

	free(line);
	free(stat.pwd);
	return SPP_PROCESS_SUCCESS;
}

#undef LINE_BUF_GROW
#undef LINE_BUF_INIT_SIZE
