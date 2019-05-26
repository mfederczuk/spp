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
 * LastEdit: 2019-05-26
 */

#include <spp/spp.h>
#include <spp/utils.h>
#include <stdlib.h>
#include <errno.h>

struct spp_stat {
	bool ignore;
};

#define STEP_PRE_DIR 0 // whitespace before directive
#define STEP_DIR_CMD 1 // directive command
#define STEP_DIR_PRE_ARG 2 // whitespace before directive command argument
#define STEP_DIR_ARG 3 // directive command argument

#define CMD_BUF_GROW 1.25
#define ARG_BUF_GROW 1.25

int checkln(wcstr line, wcstr* cmd, wcstr* arg) {
	if(cmd == NULL || arg == NULL) return SPP_CHECKLN_ERR_INV_ARGS;
	if(*cmd != NULL || *arg != NULL) return SPP_CHECKLN_ERR_INV_ARGS;

	size_t lcmd_size = 16, lcmd_len = 0;
	wcstr lcmd = malloc(WC_SIZE * lcmd_size);
	if(lcmd == NULL || errno == ENOMEM) return SPP_CHECKLN_ERR_NO_MEM;
	size_t larg_size = 16, larg_len = 0;
	wcstr larg = malloc(WC_SIZE * larg_size);
	if(larg == NULL || errno == ENOMEM) {
		free(lcmd);
		return SPP_CHECKLN_ERR_NO_MEM;
	}

	unsigned char step = STEP_PRE_DIR;
	for(size_t i = 0, l = wcslen(line); i < l; ++i) {
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
				if(lcmd_len + 2 > lcmd_size) { // grow buffer
					wcstr tmp = realloc(lcmd,
					                    WC_SIZE * (lcmd_size *= CMD_BUF_GROW));
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
			if(larg_len + 2 > larg_size) { // grow buffer
				wcstr tmp = realloc(larg,
				                    WC_SIZE * (larg_size *= ARG_BUF_GROW));
				if(tmp == NULL || errno == ENOMEM) {
					free(lcmd);
					free(larg);
					return SPP_CHECKLN_ERR_NO_MEM;
				}
				larg = tmp;
			}
			larg[larg_len] = line[i];
			++larg_len;
			break;
		}
		}
	}

	if(lcmd_len + 1 < lcmd_size) { // shorten buffer
		wcstr tmp = realloc(lcmd, WC_SIZE * (lcmd_len + 1));
		if(tmp == NULL || errno == ENOMEM) {
			free(lcmd);
			free(larg);
			return SPP_CHECKLN_ERR_NO_MEM;
		}
		lcmd = tmp;
	}

	lcmd[lcmd_len] = L'\0';

	if(larg_len + 1 < larg_size) { // shorten buffer
		wcstr tmp = realloc(larg, WC_SIZE * (larg_len + 1));
		if(tmp == NULL || errno == ENOMEM) {
			free(lcmd);
			free(larg);
			return SPP_CHECKLN_ERR_NO_MEM;
		}
		larg = tmp;
	}

	larg[larg_len] = L'\0';

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

#define LINE_BUF_GROW 1.25
#define LINE_BUF_INIT_SIZE 64

int process(FILE* in, FILE* out) {
	if(in == NULL || out == NULL) return SPP_PROCESS_ERR_INV_ARGS;

	size_t size = LINE_BUF_INIT_SIZE, len = 0;
	wcstr line = malloc(WC_SIZE * size);

	spp_stat stat = {};

	bool read = true;
	for(wint_t wc = fgetwc(in);
	        read; wc = fgetwc(in)) {

		if(wc != WEOF) {
			// build line
			if(len + 2 > size) { // grow buffer
				wcstr tmp = realloc(line, WC_SIZE * (size *= LINE_BUF_GROW));
				if(tmp == NULL || errno == ENOMEM) return SPP_PROCESS_ERR_NO_MEM;
				line = tmp;
			}
			line[len] = wc;
			++len;
		} else {
			read = false;
		}

		if(wc == WEOF || wc == L'\n') {
			// finish up building line
			if(len + 1 < size) { // shorten buffer
				wcstr tmp = realloc(line, WC_SIZE * (len + 1));
				if(tmp == NULL || errno == ENOMEM) return SPP_PROCESS_ERR_NO_MEM;
				line = tmp;
			}
			line[len] = L'\0';

			// work with line
			if(processln(line, out, &stat) == SPP_PROCESSLN_ERR_NO_MEM) {
				free(line);
				return SPP_PROCESS_ERR_NO_MEM;
			}

			// reset line
			if(size > LINE_BUF_INIT_SIZE) {
				// shrink buffer to init size if it was grown
				wcstr tmp = realloc(line, WC_SIZE * (size = LINE_BUF_INIT_SIZE));
				if(tmp == NULL || errno == ENOMEM) return SPP_PROCESS_ERR_NO_MEM;
				line = tmp;
			}
			len = 0;
		}
	} // end for

	free(line);
	return SPP_PROCESS_SUCCESS;
}

#undef LINE_BUF_GROW
#undef LINE_BUF_INIT_SIZE
