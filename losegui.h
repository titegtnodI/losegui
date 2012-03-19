#ifndef _LOSEGUI_
#define _LOSEGUI_

#include <windows.h>

char getch(HANDLE hIn);
char fgetch(HANDLE hIn);
char * getText(HANDLE hIn, HANDLE hOut, COORD pos, int maxlength);
void cls(HANDLE hOut);
void clear(HANDLE hOut, COORD from, COORD to);
void drawChar(HANDLE hOut, char chr, int x, int y);
COORD drawText(HANDLE hOut, char * text, int align, int xOff, int yPos);
void drawLine(HANDLE hOut, char chr, BOOL vertical, COORD from, unsigned int count);
void drawBox(HANDLE hOut, COORD topLeft, COORD bottomRight);

#endif
