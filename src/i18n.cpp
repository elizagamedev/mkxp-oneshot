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
	char line[1024];
	char pathbuf[100];
	FILE* locfile;

	unloadLocale();

	_setLocaleFamily(locale);

	int dictSize = 100;
	// currently there are 52, but 100 should be plenty if we ever do add more
	strdict = (char**)malloc(sizeof(char*) * dictSize);

	sprintf(pathbuf, "Languages/internal/%s.po", locale);
	locfile = fopen(pathbuf, "r");
	if (locfile) {
		while (fgets(line, 1024, locfile)) {
			if (strncmp("msgstr \"", line, 8) == 0) {
				char* lineWithoutMsgid = line + 8;
				
				char* endQuoteAddress = strrchr(lineWithoutMsgid, '"');

				// end string at last quotation mark
				if (endQuoteAddress != 0) {
					endQuoteAddress[0] = 0;
				}

				decodeEscapeChars(lineWithoutMsgid);

				int lineLen = strlen(lineWithoutMsgid);

				strdict[nStr] = (char*)malloc(lineLen + 1);
				strcpy(strdict[nStr], lineWithoutMsgid);

				nStr++;
			}
		}

		fclose(locfile);
	}
}

// replaces escape characters with their actual values in-place in a string
// can do it in-place because the replacement values are always smaller than the original value
// it only handles \\ and \", but this can be expanded to include more if needed
void decodeEscapeChars(char* s) {
	char* dest = s;
	char* src = s;
	while (src[0] != 0) {
		switch (src[0]) {
			// might be an escape character?
			case '\\':
				switch (src[1]) {
					case '\\' :
						dest[0] = '\\';
						dest++;
						src += 2;
						break;
					case '\"':
						dest[0] = '\"';
						dest++;
						src += 2;
						break;
					// end of the string?
				    // so only advance forward 1 character
					case 0:
						dest[0] = '\\';
						dest++;
						src++;
						break;
					// not escape, handle normally
					default:
						dest[0] = '\\';
						dest[1] = src[1];
						dest+=2;
						src+=2;
						break;
				}
				break;
			default:
				dest[0] = src[0];
				dest++;
				src++;
		}
	}
	// cap the end of the string
	dest[0] = 0;
}

