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
// src/utils.c
/*
 * utils.h implementation.
 * Source file for utility functions.
 *
 * Since: v0.1.0 2019-05-24
 * LastEdit: 2019-05-25
 */

#include <spp/utils.h>
#include <stdlib.h>

bool isws(wchar_t wc) {
	return wc == L' ' || wc == '\r' || wc == L'\t';
}

#define LINE_BUF_GROW 1.5
#define WC_SIZE sizeof(wchar_t)

wcstr freadline(FILE* stream) {
	size_t size = 64, len = 0;
	wcstr line = malloc(WC_SIZE * size);

	for(wint_t wc = fgetwc(stream);
	        wc != WEOF && wc != L'\n'; wc = fgetwc(stream)) {
		// grow the buffer if the length of the line, the incoming new character
		// and the terminating null char are too big for the current buffer
		if(len + 2 > size) {
			wcstr tmp = realloc(line, WC_SIZE * (size *= LINE_BUF_GROW));
			if(tmp == NULL) {
				free(line);
				return NULL;
			}
			line = tmp;
		}
		line[len] = wc;
		++len;
	}

	// shrink the buffer if the length of the line and the terminating null char
	// are smaller than the current buffer
	if(len + 1 < size) {
		wcstr tmp = realloc(line, WC_SIZE * (len + 1));
		if(tmp == NULL) {
			free(line);
			return NULL;
		}
		line = tmp;
	}

	// make sure that the last element is terminating null char
	line[len] = L'\0';

	return line;
}

#undef WC_SIZE
#undef LINE_BUF_GROW
