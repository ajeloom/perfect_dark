#include "data.h"
#include "game/game_1531a0.h"
#include "lib/str.h"
#include "archipelago.h"
#include "console.h"
#include <stdio.h>
#include <io.h>

char consoleText[15000];
char ap_Text[50][300];
s32 commandPosition[50];

int front = 0;
int currentIndex = 0;
int tail = 0;
long charsReadPosition = 0;

FILE *openedFile = NULL;

void consoleReset(void)
{
    s32 i;

	openedFile = freopen("console_output.txt", "w", stdout);

	front = 0;
	currentIndex = 0;
	tail = 0;
	charsReadPosition = 0;

    for (i = 0; i < ARRAYCOUNT(ap_Text); i++) {
        snprintf(ap_Text[i], sizeof(ap_Text[i]), "\0");
		commandPosition[i] = -1;
    }

	snprintf(consoleText, sizeof(consoleText), "\0");

	printf("Version: %s\n", apGetVersion());
    printf("If you are using an incompatible version of the APWorld, then it will not work correctly.\n");
    PrintCommands();

    return;
}

void consoleAddText(char *text)
{
    s32 i;
    s32 index = 0;
	char temp[300];

	commandPosition[currentIndex] = tail;

	if (strlen(ap_Text[currentIndex]) != 0) {
		front++;
		if (front >= ARRAYCOUNT(ap_Text)) {
			front = 0;
		}
	}

	snprintf(temp, sizeof(temp), "%s", text);

	textWrap(250, temp, ap_Text[currentIndex], g_CharsHandelGothicSm, g_FontHandelGothicSm);

	if ((tail + strlen(ap_Text[currentIndex])) >= ARRAYCOUNT(consoleText)) {
		// Move the position of the string
		commandPosition[currentIndex] = 0;

		consoleText[tail] = '\0';

		// Remove any chars past the current end
		for (i = tail + 1; i < ARRAYCOUNT(consoleText); i++) {
			consoleText[i] = ' ';
		}

		for (i = 0; i < strlen(ap_Text[currentIndex]); i++) {
			consoleText[i] = ap_Text[currentIndex][i];
		}

		tail = strlen(ap_Text[currentIndex]);
	}
	else {
		index = 0;
		for (i = tail; i < tail + strlen(ap_Text[currentIndex]); i++) {
			consoleText[i] = ap_Text[currentIndex][index++];
		}

		tail += strlen(ap_Text[currentIndex]);
	}

	currentIndex++;
	if (currentIndex >= ARRAYCOUNT(ap_Text)) {
		currentIndex = 0;
	}

    return;
}

char* consoleGetText(void)
{
	char text[300];
	
	fflush(stdout);

	openedFile = fopen("console_output.txt", "r");

	fseek(openedFile, charsReadPosition, SEEK_SET);
	while (fgets(text, sizeof(text), openedFile) != NULL) {
		// Remove ESC char from text
		for (s32 i = 0; i < strlen(text); i++) {
			if (text[i] == '\033') {
				text[i] = ' ';
			}
		}

		consoleAddText(text);
	}

	charsReadPosition = ftell(openedFile);

	fclose(openedFile);

	return consoleText;
}

s32 consoleGetStart(void)
{
	return commandPosition[front];
}

s32 consoleGetTail(void)
{
	return tail;
}

void consoleTextMeasure(s32 *textheight, s32 *textwidth, struct fontchar *font1, struct font *font2, s32 lineheight)
{
	*textheight = 0;
	*textwidth = 0;

	if (lineheight == 0) {
		lineheight = font1['['].baseline + font1['['].height;
	}

	s32 start = consoleGetStart();
	s32 end = consoleGetTail();

	s32 length = end;

	if (end < start) {
		length = strlen(consoleText);
	}

	for (s32 i = start; i < length; i++) {
		if (consoleText[i] == '\n') {
			*textheight += lineheight;
		}
	}

	if (end < start) {
		for (s32 i = 0; i < end; i++) {
			if (consoleText[i] == '\n') {
				*textheight += lineheight;
			}
		}
	}

	*textwidth = 50;
}

struct menuitem g_ConsoleMenuItems[] = {
	{
		MENUITEMTYPE_TEXTOUTPUT,
		CONSOLE_TEXT,
		0,
		400, // width
		150, // height
		NULL,
	},
	{
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
    {
		MENUITEMTYPE_TEXTINPUT,
		100,
		0,
		0,
		0,
		NULL,
	},
    {
		MENUITEMTYPE_SEPARATOR,
		0,
		0,
		0,
		0,
		NULL,
	},
	{
		MENUITEMTYPE_SELECTABLE,
		0,
		MENUITEMFLAG_SELECTABLE_CLOSESDIALOG,
		L_OPTIONS_213, // "Back"
		0,
		NULL,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_ConsoleMenuDialog = {
	MENUDIALOGTYPE_DEFAULT,
	(uintptr_t)"Console",
	g_ConsoleMenuItems,
	NULL,
	MENUDIALOGFLAG_LITERAL_TEXT,
	NULL,
};
