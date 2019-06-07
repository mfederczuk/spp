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
// src/main.c
/*
 * Main source file for spp.
 *
 * Since: v0.1.0 2019-05-25
 * LastEdit: 2019-06-07
 */

#include <string.h>
#include <stdio.h>
#include <spp/usage.h>
#include <spp/version-info.h>
#include <spp/utils.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <spp/spp.h>
#include <stdlib.h>
#include <libgen.h>

#define errprintf(msg, ...) fprintf(stderr, (msg), __VA_ARGS__)

/* exit codes
 * 48 - <path>: too many symbolic links encountered
 * 49 - <path>: path name too long
 */

int main(int argc, char** argv) {
	cstr_t file = NULL;

	if(argc == 2) {
		if(strcmp(argv[1], "--help") == 0) {
			printf(USAGE, argv[0]);
			return 0;
		} else if(strcmp(argv[1], "--version") == 0) {
			fputs(VERSION_INFO, stdout);
			return 0;
		} else if(strcmp(argv[1], "-") != 0) {
			file = argv[1];
		}
	} else if(argc == 3 && strcmp(argv[1], "--") == 0) {
		file = argv[2];
	} else if(argc > 2) {
		errprintf("%s: too many arguments: %d\n", argv[0], argc - 2);
		return 4;
	}

	FILE* ins = NULL;
	cstr_t pwd = NULL;

	if(file != NULL) {
		struct stat sb;
		if(stat(file, &sb) != 0) {
			switch(errno) {
			case EACCES: {
				errprintf("%s: permission denied\n", argv[0]);
				return 77;
			}
			case EBADF:
			case EFAULT:
			case EOVERFLOW: {
				errprintf("%s: input/output error\n", argv[0]);
				return 74;
			}
			case ELOOP: {
				errprintf("%s: %s: too many symbolic links encountered\n",
				          argv[0], file);
				return 48;
			}
			case ENAMETOOLONG: {
				errprintf("%s: %s: path name too long\n", argv[0], file);
				return 49;
			}
			case ENOENT:
			case ENOTDIR: {
				errprintf("%s: %s: no such file\n", argv[0], file);
				return 24;
			}
			case ENOMEM: {
				errprintf("%s: not enough memory\n", argv[0]);
				return 100;
			}

			default: {
				errprintf("%s: unknown error\n", argv[0]);
				return 125;
			}
			}
		}

		if(!S_ISREG(sb.st_mode)) {
			errprintf("%s: %s: not a file\n", argv[0], file);
			return 25;
		}

		ins = fopen(file, "r");
		if(ins == NULL) {
			if(errno == ENOMEM) {
				errprintf("%s: not enough memory\n", argv[0]);
				return 100;
			}
			// I don't want to create a case for every possibility that errno could
			// be, we're just gonna use the catchall exit code 1 and use perror to
			// show a message (this is rarely gonna happen anyway)
			perror(argv[0]);
			return 1;
		}

		errno = 0;
		cstr_t path = realpath(file, NULL);
		if(path == NULL) {
			switch(errno) {
			case EACCES: {
				errprintf("%s: permission denied\n", argv[0]);
				return 77;
			}
			case EIO: {
				errprintf("%s: input/output error\n", argv[0]);
				return 74;
			}
			case ELOOP: {
				errprintf("%s: %s: too many symbolic links encountered\n",
				          argv[0], file);
				return 48;
			}
			case ENAMETOOLONG: {
				errprintf("%s: %s: path name too long\n", argv[0], file);
				return 49;
			}
			case ENOENT:
			case ENOTDIR: {
				errprintf("%s: %s: no such file\n", argv[0], file);
				return 24;
			}
			case ENOMEM: {
				errprintf("%s: not enough memory\n", argv[0]);
				return 100;
			}
			}
			return 1;
		}

		cstr_t dir = dirname(path);
		pwd = malloc(CHAR_SIZE * (strlen(dir) + 1));
		strcpy(pwd, dir);

		free(path);
	} else {
		ins = stdin;
	}

	errno = 0;
	if(process(ins, stdout, pwd) != 0) {
		switch(errno) {
		case ENOMEM: {
			errprintf("%s: not enough memory\n", argv[0]);
			return 100;
		}
		case EACCES: {
			errprintf("%s: permission denied\n", argv[0]);
			return 77;
		}
		case EBADF:
		case EFAULT:
		case EOVERFLOW: {
			errprintf("%s: input/output error\n", argv[0]);
			return 74;
		}
		case ELOOP: {
			errprintf("%s: %s: too many symbolic links encountered\n",
			          argv[0], file);
			return 48;
		}
		default: {
			errprintf("%s: unknown error\n", argv[0]);
			return 125;
		}
		}
	}

	if(file != NULL && fclose(ins) == EOF) {
		// same thing as with fopen(); too many errno possibilies
		perror(argv[0]);
		return 1;
	}

	if(pwd != NULL) free(pwd);

	return 0;
}
