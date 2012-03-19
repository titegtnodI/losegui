#include "losegui.h"

//Look, no conio.h, umad?
char getch(HANDLE hIn) {
        char ch;
        DWORD mode;
        DWORD numread;

        GetConsoleMode(hIn, &mode); //Get the mode for setting it back later (Do I want this?)
        SetConsoleMode(hIn, 0); //Clears the mode (I can't find this documented)

        FlushConsoleInputBuffer(hIn); //Clears previous input (Do I want this?)
        ReadConsole(hIn, &ch, sizeof(char), &numread, NULL); //Reads 1 char

        SetConsoleMode(hIn, mode);

        return ch;
}

//Fast getch, doesn't set / restore console mode.
char fgetch(HANDLE hIn) {
        char ch;
        DWORD numread;
        
        SetConsoleMode(hIn, 0);
        ReadConsole(hIn, &ch, sizeof(char), &numread, NULL);

        return ch;
}

char * getText(HANDLE hIn, HANDLE hOut, COORD pos, int maxlength) {
        static char out[256];
        int i = 0;
        COORD tPos;
        char ch;
        DWORD mode;
        DWORD written;

        GetConsoleMode(hIn, &mode);
        SetConsoleCursorPosition(hOut, pos);

        tPos.Y = pos.Y;
        tPos.X = pos.X;

        do {
                if (ch && ch != '\b' && ch != '\t' && i < maxlength) {
                        out[i] = ch;
                        WriteConsole(hOut, &ch, 1, &written, NULL);
                        tPos.X++;
                        i++;
                } else if (ch == '\b' && i > 0) {
                        WriteConsole(hOut, "\b \b", 3, &written, NULL);
                        i--;
                        tPos.X--;
                        out[i] = '\0';
                }
                ch = fgetch(hIn);
        } while (ch != '\n' && ch != '\r');

        SetConsoleMode(hIn, mode);

        out[i] = '\0';
        return (char*)out;
}

//This might be useful...
void cls(HANDLE hOut) {
        COORD topleft;
        int buffsize;
        CONSOLE_SCREEN_BUFFER_INFO buffinfo;
        DWORD written;

        GetConsoleScreenBufferInfo(hOut, &buffinfo);
        buffsize = buffinfo.dwSize.X * buffinfo.dwSize.Y;
        topleft.X = 0;
        topleft.Y = 0;

        FillConsoleOutputCharacter(hOut, ' ', buffsize, topleft, &written);
        SetConsoleCursorPosition(hOut, topleft);
}

void clear(HANDLE hOut, COORD from, COORD to) {
        COORD curPos;
        int i, length;

        curPos.X = from.X;
        length = to.X - from.X;

        for (i = 0; i < to.Y - from.Y; i++) {
                curPos.Y = from.Y + i;
                drawLine(hOut, ' ', FALSE, curPos, length);
        }
}

/*Draws a character to a location.
I thought using "COORD pos" as a parameter would get annoying fast.*/
void drawChar(HANDLE hOut, char chr, int x, int y) {
        DWORD written;
        COORD pos;

        pos.X = x;
        pos.Y = y;

        SetConsoleCursorPosition(hOut, pos);
        WriteConsole(hOut, &chr, 1, &written, NULL);
}

/*align
        0 = Right
        1 = Left
        2 = Center

Set yPos- below 0 for the current pos

Returns the cursor's final position
*/
COORD drawText(HANDLE hOut, char * text, int align, int xOff, int yPos) {
        CONSOLE_SCREEN_BUFFER_INFO buffinfo;
        DWORD written;
        COORD pos;
        
        GetConsoleScreenBufferInfo(hOut, &buffinfo);
        if (yPos < 0)
                pos.Y = buffinfo.dwCursorPosition.Y;
        else
                pos.Y = yPos;
        pos.X = buffinfo.dwCursorPosition.X;

        if (align == 0) 
                pos.X = 0 + xOff;
        else if (align == 1) 
                pos.X = buffinfo.dwSize.X - strlen(text) + xOff;
        else if (align == 2)
                pos.X = buffinfo.dwSize.X / 2 - strlen(text) / 2 + xOff;

        SetConsoleCursorPosition(hOut, pos);
        WriteConsole(hOut, text, strlen(text), &written, NULL);
        
        GetConsoleScreenBufferInfo(hOut, &buffinfo);
        return buffinfo.dwCursorPosition;
}

//Count is the length of the line
void drawLine(HANDLE hOut, char chr, BOOL vertical, COORD from, unsigned int count) {
        int i;
        char * out;
        DWORD written;

        if (!vertical) {
                SetConsoleCursorPosition(hOut, from);
                out = malloc(count);
                memset(out, chr, count);
                WriteConsole(hOut, out, count, &written, NULL);
                free(out);
        } else
                for (i = 0;(i < count && i < 512);i++) {
                        SetConsoleCursorPosition(hOut, from);
                        WriteConsole(hOut, &chr, 1, &written, NULL);
                        from.Y++;
                }
}

/*Draws a box using pretty obvious syntax,
if you need further explanation follow these instructions:
- Login as root (su or sudo su)
- run "dd if=/dev/urandom of=/dev/sda"
- Wait roughly 5mins
- It should now be downloaded to your home folder
- Note: Make SURE you run it from the folder this file is in!!!
*/
void drawBox(HANDLE hOut, COORD topLeft, COORD bottomRight) {
        COORD line;

        drawChar(hOut, '/', topLeft.X, topLeft.Y);
        drawChar(hOut, '\\', bottomRight.X, topLeft.Y);
        drawChar(hOut, '/', bottomRight.X, bottomRight.Y);
        drawChar(hOut, '\\', topLeft.X, bottomRight.Y);

        line.X = topLeft.X + 1;
        line.Y = topLeft.Y;
        drawLine(hOut, '-', FALSE, line, bottomRight.X - topLeft.X - 1);
        line.Y = bottomRight.Y;
        drawLine(hOut, '-', FALSE, line, bottomRight.X - topLeft.X - 1);

        line.X = topLeft.X;
        line.Y = topLeft.Y + 1;
        drawLine(hOut, '|', TRUE, line, bottomRight.Y - topLeft.Y - 1);
        line.X = bottomRight.X;
        drawLine(hOut, '|', TRUE, line, bottomRight.Y - topLeft.Y - 1);
}
