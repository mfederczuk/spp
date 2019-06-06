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
// src/directives.c
/*
 * Source file for the directive functions.
 *
 * Since: v0.1.0 2019-06-05
 * LastEdit: 2019-06-06
 */

#include <spp/directives.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>

cstr_t spp_dirs_names[SPP_DIRS_AMOUNT] = {
	"insert", "include",
	"ignore", "end-ignore"
};
spp_dir_func_t spp_dirs_funcs[SPP_DIRS_AMOUNT] = {
	spp_insert, spp_include,
	spp_ignore, spp_end_ignore
};

enum spp_dir_func_ret spp_insert(struct spp_stat* spp_stat,
                                 FILE* out,
                                 cstr_t arg) {
	cstr_t filep = NULL;
	// making sure the entered path is absolute and saving it into filep var
	if(strncmp(arg, "/", 1) != 0) { // if it is relative, add pwd to it
		size_t pwdlen = strlen(spp_stat->pwd),
		       arglen = strlen(arg);

		errno = 0;
		filep = malloc(CHAR_SIZE * (pwdlen + 1 + arglen + 1));
		if(filep == NULL || errno == ENOMEM) {
			errno = ENOMEM;
			return SPP_DIR_FUNC_ERROR;
		}

		size_t i = 0;
		for(; i < pwdlen; ++i) {
			filep[i] = spp_stat->pwd[i];
		}
		filep[i] = '/';
		++i;
		for(size_t j = 0; j < arglen; ++j, ++i) {
			filep[i] = arg[j];
		}
		filep[i] = '\0';
	} else {
		filep = malloc(CHAR_SIZE * (strlen(arg) + 1));
		if(filep == NULL || errno == ENOMEM) {
			errno = ENOMEM;
			return SPP_DIR_FUNC_ERROR;
		}
		strcpy(filep, arg);
	}

	struct stat sb;
	errno = 0;
	if(stat(filep, &sb) != 0) { // if file doesn't exist or some other error
		switch(errno) {
		case ENAMETOOLONG:
		case ENOENT:
		case ENOTDIR: {
			// when the path name is too long or the path doesn't exist
			// we ignore the directive
			free(filep);
			return SPP_DIR_FUNC_INVALID;
		}
		default: {
			free(filep);
			return SPP_DIR_FUNC_ERROR;
		}
		}
	} else { // file exists; we can work with it
		errno = 0;
		FILE* file = fopen(filep, "r");
		if(file == NULL) {
			free(filep);
			return SPP_DIR_FUNC_ERROR;
		}

		for(int ch = fgetc(file);
		        ch != EOF; ch = fgetc(file)) {

			fputc(ch, out);
		}

		fclose(file);
		free(filep);
		return SPP_DIR_FUNC_SUCCESS;
	}
}

enum spp_dir_func_ret spp_include(struct spp_stat* spp_stat,
                                  FILE* out,
                                  cstr_t arg) {
	cstr_t filep = NULL;
	// making sure the entered path is absolute and saving it into filep var
	if(strncmp(arg, "/", 1) != 0) { // if it is relative, add pwd to it
		size_t pwdlen = strlen(spp_stat->pwd),
		       arglen = strlen(arg);

		errno = 0;
		filep = malloc(CHAR_SIZE * (pwdlen + 1 + arglen + 1));
		if(filep == NULL || errno == ENOMEM) {
			errno = ENOMEM;
			return SPP_DIR_FUNC_ERROR;
		}

		size_t i = 0;
		for(; i < pwdlen; ++i) {
			filep[i] = spp_stat->pwd[i];
		}
		filep[i] = '/';
		++i;
		for(size_t j = 0; j < arglen; ++j, ++i) {
			filep[i] = arg[j];
		}
		filep[i] = '\0';
	} else {
		filep = malloc(CHAR_SIZE * (strlen(arg) + 1));
		if(filep == NULL || errno == ENOMEM) {
			errno = ENOMEM;
			return SPP_DIR_FUNC_ERROR;
		}
		strcpy(filep, arg);
	}

	struct stat sb;
	errno = 0;
	if(stat(filep, &sb) != 0) { // if file doesn't exist or some other error
		switch(errno) {
		case ENAMETOOLONG:
		case ENOENT:
		case ENOTDIR: {
			// when the path name is too long or the path doesn't exist
			// we ignore the directive
			free(filep);
			return SPP_DIR_FUNC_INVALID;
		}
		default: {
			free(filep);
			return SPP_DIR_FUNC_ERROR;
		}
		}
	} else { // file exists; we can work with it
		errno = 0;
		FILE* file = fopen(filep, "r");
		if(file == NULL) {
			free(filep);
			return SPP_DIR_FUNC_ERROR;
		}

		process(file, out, dirname(filep));
		// TODO: process() error handling

		fclose(file);
		free(filep);
		return SPP_DIR_FUNC_SUCCESS;
	}
}

enum spp_dir_func_ret spp_ignore(struct spp_stat* stat,
                                 FILE* out,
                                 cstr_t arg) {
	stat->ignore = true;
}

enum spp_dir_func_ret spp_end_ignore(struct spp_stat* stat,
                                     FILE* out,
                                     cstr_t arg) {
	stat->ignore = false;
}

