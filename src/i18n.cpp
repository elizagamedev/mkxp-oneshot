/*
	Really hacked-together naiive implementation of gettext
*/
#include "i18n.h"
#include "debugwriter.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

const char* LOC_HEADER = "ONELOC\x10";

char** strdict = 0;
unsigned int nStr = 0;
int family = LOCALE_FAMILY_LATIN;

int getLocaleFamily() {
	Debug() << "Get family:" << family;
	return family;
}

const char* findtext(unsigned int msgid, const char* fallback) {
	Debug() << "Looking for msg" << msgid << fallback;
	if (msgid >= nStr) {
		return fallback;
	} else {
		Debug() << "found" << strdict[msgid];
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

void loadLocale(const char* locale) {
	Debug() << "Load locale:" << locale;
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
			fread(header, 1, 8, locfile);
			if (!strcmp(header, LOC_HEADER)) {
				//read number of strs in this file
				fread(&nStr, 4, 1, locfile);
				strdict = (char**)malloc(sizeof(char*) * nStr);
				for (i = 0; i < nStr; i++) {
					//read the size of the next string
					fread(&strSize, 4, 1, locfile);
					strdict[i] = (char*)malloc(strSize+1);
					if (strSize > 0) {
						//read the contents of the next string
						fread(strdict[i], 1, strSize, locfile);
						strdict[i][strSize] = 0;
						Debug() << "localization #" << i << " : " << strdict[i] << "|" << strSize;
					}
					strdict[i][strSize] = 0;
				}
			}
			fclose(locfile);
	}
}
