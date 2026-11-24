void consoleReset(void);
void consoleAddText(char *text);
char *consoleGetText(void);
s32 consoleGetStart(void);
s32 consoleGetTail(void);
void consoleTextMeasure(s32 *textheight, s32 *textwidth, struct fontchar *font1, struct font *font2, s32 lineheight);