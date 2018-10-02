/*
	Really hacked-together naiive implementation of gettext
*/
#include "i18n.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* LOC_HEADER = "ONELOC\x10";

char** strdict = 0;
unsigned int nStr = 0;
int family = LOCALE_FAMILY_LATIN;
size_t readSize;

int getLocaleFamily() {
	return family;
}

const char* findtext(unsigned int msgid, const char* fallback) {
	if (msgid >= nStr) {
		return fallback;
	} else {
		return strdict[msgid];
	}
}

void unloadLocale() {
	for (unsigned int i = 0; i < nStr; i++) {
		free(strdict[i]);
	}
	free(strdict);
	strdict = 0;
	nStr = 0;
}

void _setLocaleFamily(const char* locale) {
	if (!strcmp(locale, "ja")
		|| !strcmp(locale, "ko")
		|| !strcmp(locale, "zh_CN")) {
		family = LOCALE_FAMILY_ASIAN;
	} else {
		family = LOCALE_FAMILY_LATIN;
	}
}

void _read(void * ptr, size_t size, size_t count, FILE * stream) {
	readSize = fread(ptr, size, count, stream);
	if (readSize != count) {
	}
}

void loadLocale(const char* locale) {
	char pathbuf[100];
	FILE* locfile;
	char header[8];
	unsigned int i;
	int strSize = 0;

	unloadLocale();

	_setLocaleFamily(locale);

	sprintf(pathbuf, "Languages/internal/%s.loc", locale);
	locfile = fopen(pathbuf, "rb");
	if (locfile) {
		_read(header, 1, 8, locfile);
		if (!strcmp(header, LOC_HEADER)) {
			// Read number of strs in this file
			_read(&nStr, 4, 1, locfile);
			strdict = (char**)malloc(sizeof(char*) * nStr);
			for (i = 0; i < nStr; i++) {
				// Read the size of the next string
				_read(&strSize, 4, 1, locfile);
				strdict[i] = (char*)malloc(strSize+1);
				if (strSize > 0) {
					// Read the contents of the next string
					_read(strdict[i], 1, strSize, locfile);
					strdict[i][strSize] = 0;
				}
				strdict[i][strSize] = 0;
			}
		}
		fclose(locfile);
	}
}
