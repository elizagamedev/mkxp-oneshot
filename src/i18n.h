#ifndef I18N_H
#define I18N_H

#include "trstr.h"

void unloadLocale();
void loadLocale(const char* locale);
void decodeEscapeChars(char* s);
const char* findtext(unsigned int msgid, const char* fallback);
int getLocaleFamily();

#define LOCALE_FAMILY_LATIN 1
#define LOCALE_FAMILY_ASIAN 2

#endif /* end of include guard: I18N_H */
