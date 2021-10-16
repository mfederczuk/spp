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
	"ignore", "end-ignore", "ignorenext"
};
spp_dir_func_t spp_dirs_funcs[SPP_DIRS_AMOUNT] = {
	spp_insert, spp_include,
	spp_ignore, spp_end_ignore, spp_ignore_next
};

int spp_insert(struct spp_stat* spp_stat, FILE* out, cstr_t arg) {
	if(spp_stat->ignore || spp_stat->ignore_next) {
		spp_stat->ignore_next = false;
		return 0;
	}

	cstr_t filep = NULL;
	// making sure the entered path is absolute and saving it into filep var
	if(strncmp(arg, "/", 1) != 0) { // if it is relative, add pwd to it
		size_t pwdlen = strlen(spp_stat->pwd),
		       arglen = strlen(arg);

		errno = 0;
		filep = malloc(CHAR_SIZE * (pwdlen + 1 + arglen + 1));
		if(filep == NULL || errno == ENOMEM) {
			errno = ENOMEM;
			return 1;
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
			return 1;
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
			return 1;
		}
		default: {
			int tmp = errno;
			free(filep);
			errno = tmp;
			return 1;
		}
		}
	} else { // file exists; we can work with it
		errno = 0;
		FILE* file = fopen(filep, "r");
		if(file == NULL) {
			int tmp = errno;
			free(filep);
			errno = tmp;
			return 1;
		}

		for(int ch = fgetc(file);
		        ch != EOF; ch = fgetc(file)) {

			fputc(ch, out);
		}

		fclose(file);
		free(filep);
		return 0;
	}
}

int spp_include(struct spp_stat* spp_stat, FILE* out, cstr_t arg) {
	if(spp_stat->ignore || spp_stat->ignore_next) {
		spp_stat->ignore_next = false;
		return 0;
	}

	cstr_t filep = NULL;
	// making sure the entered path is absolute and saving it into filep var
	if(strncmp(arg, "/", 1) != 0) { // if it is relative, add pwd to it
		size_t pwdlen = strlen(spp_stat->pwd),
		       arglen = strlen(arg);

		errno = 0;
		filep = malloc(CHAR_SIZE * (pwdlen + 1 + arglen + 1));
		if(filep == NULL || errno == ENOMEM) {
			errno = ENOMEM;
			return 1;
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
			return 1;
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
			return 1;
		}
		default: {
			int tmp = errno;
			free(filep);
			errno = tmp;
			return 1;
		}
		}
	} else { // file exists; we can work with it
		errno = 0;
		FILE* file = fopen(filep, "r");
		if(file == NULL) {
			int tmp = errno;
			free(filep);
			errno = tmp;
			return 1;
		}

		process(file, out, dirname(filep));
		// TODO: process() error handling

		fclose(file);
		free(filep);
		return 0;
	}
}

int spp_ignore(struct spp_stat* stat, FILE* out, cstr_t arg) {
	if(!stat->ignore_next) {
		stat->ignore = true;
		stat->ignore_next = false;
	}
	return 0;
}

int spp_end_ignore(struct spp_stat* stat, FILE* out, cstr_t arg) {
	if(!stat->ignore_next) {
		stat->ignore = false;
		stat->ignore_next = false;
	}
	return 0;
}

int spp_ignore_next(struct spp_stat* stat, FILE* out, cstr_t arg) {
	if(!stat->ignore) stat->ignore_next = true;
	return 0;
}
