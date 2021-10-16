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

#include <spp/spp.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <spp/utils.h>
#include <spp/directives.h>

enum {
	STEP_PRE_DIR, // whitespace before directive
	STEP_DIR_CMD, // directive command
	STEP_DIR_PRE_ARG, // whitespace before directive command argument
	STEP_DIR_ARG // directive command argument
};

#define CMD_BUF_GROW 1.25
#define ARG_BUF_GROW 1.25

int checkln(cstr_t line, cstr_t* cmd, cstr_t* arg) {
	if(cmd == NULL || arg == NULL
	        || *cmd != NULL || *arg != NULL) {
		errno = EINVAL;
		return 1;
	}

	size_t lcmd_size = 16, lcmd_len = 0;
	errno = 0;
	cstr_t lcmd = malloc(CHAR_SIZE * lcmd_size);
	if(lcmd == NULL || errno == ENOMEM) {
		errno = ENOMEM;
		return 1;
	}

	size_t larg_size = 16, larg_len = 0;
	errno = 0;
	cstr_t larg = malloc(CHAR_SIZE * larg_size);
	if(larg == NULL || errno == ENOMEM) {
		free(lcmd);
		errno = ENOMEM;
		return 1;
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
				return 0; // no directive
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
						errno = ENOMEM;
						return 1;
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
						errno = ENOMEM;
						return 1;
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
		return 0; // no directive
	}

	if(lcmd_len + CHAR_SIZE < lcmd_size) { // shorten buffer
		errno = 0;
		cstr_t tmp = realloc(lcmd, CHAR_SIZE * (lcmd_size = lcmd_len + CHAR_SIZE));
		if(tmp == NULL || errno == ENOMEM) {
			free(lcmd);
			free(larg);
			errno = ENOMEM;
			return 1;
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
			errno = ENOMEM;
			return 1;
		}
		larg = tmp;
	}

	larg[larg_len] = '\0';

	*cmd = lcmd;
	*arg = larg;
	return 0; // is directive
}

int processln(cstr_t line, FILE* out, struct spp_stat* spp_stat) {
	if(out == NULL || spp_stat == NULL) {
		errno = EINVAL;
		return 1;
	}

	cstr_t cmd = NULL, arg = NULL;
	if(checkln(line, &cmd, &arg) != 0) return 1;

	bool valid_dir = false;
	if(cmd != NULL) { // line is valid directive
		// search for directive function
		spp_dir_func_t dir_func = NULL;
		for(size_t i = 0; i < SPP_DIRS_AMOUNT && dir_func == NULL; ++i) {
			if(strcmp(cmd, spp_dirs_names[i]) == 0) {
				dir_func = spp_dirs_funcs[i];
			}
		}

		// if a function was found; call it
		if(dir_func != NULL) {
			errno = 0;
			valid_dir = (dir_func(spp_stat, out, arg) == 0);
			int tmp = errno;
			free(cmd);
			free(arg);
			errno = tmp;

			// function failed and error happened
			if(!valid_dir && errno != 0) return 1;
		} else {
			free(cmd);
			free(arg);
		} // end if(dir_func != NULL)
	} // end if(cmd != NULL)

	if(!valid_dir) { // line is not a valid directive
		if(!spp_stat->ignore && !spp_stat->ignore_next) {
			errno = 0;
			int exc = fputs(line, out);
			if(exc < 0 || exc == EOF) return 1;
		}
		spp_stat->ignore_next = false;
	}

	return 0;
}

#define LINE_BUF_GROW 1.25
#define LINE_BUF_INIT_SIZE 64

int process(FILE* in, FILE* out, cstr_t pwd) {
	if(in == NULL || out == NULL) {
		errno = EINVAL;
		return 1;
	}

	// initial allocation for the line buffer
	size_t size = LINE_BUF_INIT_SIZE, len = 0;
	errno = 0;
	cstr_t line = malloc(CHAR_SIZE * size);
	if(line == NULL || errno == ENOMEM) {
		errno = ENOMEM;
		return 1;
	}

	// creating the spp_stat struct
	struct spp_stat stat = {
		.ignore = false,
		.ignore_next = false,
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
		errno = ENOMEM;
		return 1;
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
					errno = ENOMEM;
					return 1;
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
					errno = ENOMEM;
					return 1;
				}
				line = tmp;
			}
			line[len] = '\0';

			// work with line
			errno = 0;
			if(processln(line, out, &stat) != 0) {
				int tmp = errno;
				free(line);
				free(stat.pwd);
				errno = tmp;
				return 1;
			}

			// reset line
			if(size != LINE_BUF_INIT_SIZE) {
				// shrink buffer to init size if it was grown
				errno = 0;
				cstr_t tmp = realloc(line,
				                     CHAR_SIZE * (size = LINE_BUF_INIT_SIZE));
				if(tmp == NULL || errno == ENOMEM) {
					free(line);
					free(stat.pwd);
					errno = ENOMEM;
					return 1;
				}
				line = tmp;
			}
			len = 0;
		}
	} // end for

	free(line);
	free(stat.pwd);
	return 0;
}
