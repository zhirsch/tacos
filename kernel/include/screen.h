#ifndef SCREEN_H
#define SCREEN_H

void screen_clear(void);
void screen_writech(char ch);
void screen_writef(const char* fmt, ...) __attribute__ ((format(printf, 1, 2)));

#endif /* SCREEN_H */
