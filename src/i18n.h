#ifndef I18N_H
#define I18N_H

#include "trstr.h"

void unloadLocale();
void loadLocale(const char* locale);
void decodeEscapeChars(char* s);
const char* findtext(unsigned int msgid, const char* fallback);
void loadLanguageMetadata();
void unloadLanguageMetadata();
int getFontSize();
char* getFontName();

#endif /* end of include guard: I18N_H */
