#ifdef __cplusplus
extern "C" {

#endif

#define _WIN32_WINNT 0x0501
#include <windows.h>

typedef struct _SUBMENU {
	unsigned char id;
	wchar_t* label;
	unsigned char accelerator;
	void (*callback)();
	wchar_t* description;
} SUBMENU;

typedef struct _MENU {
	unsigned char id;
	wchar_t* label;
	unsigned char accelerator;
	SUBMENU subMenu[11];
	unsigned char lastSubMenuID;
} MENU;

typedef struct _BUTTON {
	unsigned char id;
	wchar_t* label;
	unsigned char accelerator;
	COORD pos;
	void (*callback)();
	unsigned char ry;
	unsigned char rx1, rx2;
} BUTTON;

typedef struct _LABEL {
	unsigned char id;
	wchar_t* caption;
	unsigned char accelerator;
	COORD pos;
} LABEL;

typedef struct _DIALOG {
	unsigned char id;
	wchar_t* title;
	unsigned char width;
	unsigned char height;
	BUTTON buttons[6];
	unsigned char buttonCount;
	LABEL labels[14];
	unsigned char labelCount;
	COORD tabPos[10];
	unsigned char tabStops;
} DIALOG;

void gotoxy(unsigned char x, unsigned char y);

void setcolor(unsigned char fore, unsigned char back);

void init(void (*callback)());

MENU* addMenu(unsigned char id, wchar_t* label, unsigned char accelerator);

SUBMENU* addSubMenu(MENU* parent, unsigned char id, wchar_t* label, unsigned char accelerator, void (*callback)(), wchar_t* description);

SUBMENU* addMenuSeparator(MENU* parent, unsigned char id);

DIALOG* createDialog(DIALOG* dialog, unsigned char id, wchar_t* title, unsigned char width, unsigned char height);

BUTTON* addButton(const DIALOG *parent, const unsigned char id, const wchar_t *label, const unsigned char accelerator, const unsigned char xPos, const unsigned char yPos, void (*callback)());

LABEL* addLabel(const DIALOG *parent, const unsigned char id, const wchar_t *caption, const unsigned char accelerator, const unsigned char xPos, const unsigned char yPos);

void showDialog(DIALOG* dialog);

void cls();

void setStatus(const wchar_t* msg);
void setFullStatus(const wchar_t* msg);

void drawFrame();

void fixWindows10(unsigned short argc, char *argv[]);

unsigned short getField(wchar_t* s, int l, int x, int y, int flag, const wchar_t* def, void (*cbSuccess)(wchar_t*), void (*cbFail)());

#ifdef __cplusplus
}

#endif
