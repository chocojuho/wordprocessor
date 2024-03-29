#include "tui.h"

HWND hwnd;
HANDLE input;
HANDLE output;

MENU menu[8];
unsigned char _lastMenuID = 255;
#define menuCount (_lastMenuID + 1)
#define subMenuCount lastSubMenuID + 1
unsigned char selectedMenuID;

const unsigned char bgColor = 1;
const unsigned char frColor = 7;
const unsigned char statusBarBackground = 9;

unsigned char x = 0, y = 0;
unsigned char col = 1, line = 1;

#define MAIN 0
#define MENU_OPEN 1
#define SUBMENU_OPEN 2
#define DIALOG_OPEN 4
unsigned char state = MAIN;
unsigned char subMenuPos;
unsigned char mnuID[85];
DIALOG *openedDialog;
BUTTON *pressedBtn;
unsigned char tabPos = 0;

#define MOUSEUP 0
#define MOUSEDOWN 1
unsigned char mouseState = MOUSEUP;
#define INSERT 0
#define MODIFY 1;
unsigned char inputMode = INSERT;

unsigned char wcssz(str)
	const wchar_t* str;
{
	unsigned char ret = 0;
	while(*str) {
		ret++;
		str++;
	}
	return ret;
}

unsigned char wcsbytesz(str)
	const wchar_t* str;
{
	unsigned char ret = 0;
	while(*str) {
		ret += 1 + (*str > 255);
		str++;
	}
	return ret;
}

void gotoxy(x, y)
	unsigned char x;	/* X좌표 */
	unsigned char y;	/* Y좌표 */
{
	static COORD pos;
	pos.X = x;
	pos.Y = y;
	SetConsoleCursorPosition(output, pos);
}

void setcolor(fore, back)
	unsigned char fore;	/* 전경색 */
	unsigned char back;	/* 배경색 */
{
	SetConsoleTextAttribute(output, (back << 4) | fore);
}

MENU* addMenu(id, label, accelerator)
	unsigned char id;				/* 내부 번호 */
	wchar_t* label;				/* 표시 이름 */
	unsigned char accelerator;		/* 바로 가기 키 위치 */
{
	MENU *mnu = &menu[++_lastMenuID];
	mnu->id = id;
	mnu->label = label;
	mnu->lastSubMenuID = 255;
	mnu->accelerator = accelerator;
	return mnu;
}

SUBMENU* addSubMenu(parent, id, label, accelerator, callback, description)
	MENU *parent;						/* 상위 메뉴 */
	unsigned char id;			/* 내부 번호 */
	wchar_t* label;				/* 표시 이름 */
	unsigned char accelerator;	/* 바로 가기 키 위치 */
	void (*callback)();				/* 콜백 함수 */
	wchar_t* description;		/* 설명 */
{
	(parent->lastSubMenuID)++;
	parent->subMenu[parent->lastSubMenuID].id = id;
	parent->subMenu[parent->lastSubMenuID].label = label;
	parent->subMenu[parent->lastSubMenuID].accelerator = accelerator;
	parent->subMenu[parent->lastSubMenuID].callback = callback;
	parent->subMenu[parent->lastSubMenuID].description = description;
	return &(parent->subMenu[parent->lastSubMenuID]);
}

SUBMENU* addMenuSeparator(parent, id)
	MENU *parent;						/* 상위 메뉴 */
	const unsigned char id;			/* 내부 번호 */
{
	return addSubMenu(parent, id, L"-", 0, NULL, L"");
}

DIALOG* createDialog(dialog, id, title, width, height)
	DIALOG *dialog;
	unsigned char id;
	wchar_t *title;
	unsigned char width;
	unsigned char height;
{
	dialog->id = id;
	dialog->title = title;
	dialog->width = width;
	dialog->height = height;
	dialog->buttonCount = 0;
	dialog->labelCount = 0;
	dialog->tabStops = 0;
	return dialog;
}

BUTTON* addButton(parent, id, label, accelerator, xPos, yPos, callback)
	DIALOG *parent;
	unsigned char id;
	wchar_t *label;
	unsigned char accelerator;
	unsigned char xPos;
	unsigned char yPos;
	void (*callback)();
{
	BUTTON *button = &parent->buttons[(parent->buttonCount)++];
	COORD pos;
	button->id = id;
	button->label = label;
	button->accelerator = accelerator;
	pos.X = xPos;
	pos.Y = yPos;
	button->pos = pos;
	button->callback = callback;
	if(id < 255)
		parent->tabStops++;
	return button;
}

LABEL* addLabel(parent, id, caption, accelerator, xPos, yPos)
	DIALOG *parent;
	unsigned char id;
	wchar_t *caption;
	unsigned char accelerator;
	unsigned char xPos;
	unsigned char yPos;
{
	LABEL *label = &parent->labels[(parent->labelCount)++];
	COORD pos;
	label->id = id;
	label->caption = caption;
	label->accelerator = accelerator;
	pos.X = xPos;
	pos.Y = yPos;
	label->pos = pos;
	return label;
}

#define NORMAL	1
#define PRESSED	2
void renderButton(btn, btnState)
	BUTTON *btn;
	unsigned char btnState;
{
	static unsigned char x1, y1, i, j, btnSize;
	x1 = 39 - openedDialog->width / 2;
	y1 = 11 - openedDialog->height / 2;

	btnSize = wcsbytesz(btn->label) + 4;
	if(btnState == PRESSED) {
		setcolor(0, 7);
		gotoxy(x1 - 1 + btn->pos.X, y1 + btn->pos.Y);
		_putwch(L' ');
	}
	gotoxy(x1 - 1 + btn->pos.X + (btnState == PRESSED), y1 + btn->pos.Y);
	setcolor(0, 15);
	for(j=0; j<btnSize; j++)
		_putwch(L' ');
	setcolor(0, 7);
	if(btnState != PRESSED) {
		_putwch(L'▄');
		gotoxy(x1 + btn->pos.X, y1 + btn->pos.Y + 1);
		setcolor(0, 7);
		for(j=0; j<btnSize; j++)
			_putwch(L'▀');
	} else {
		gotoxy(x1 + btn->pos.X, y1 + btn->pos.Y + 1);
		setcolor(0, 7);
		for(j=0; j<btnSize; j++)
			_putwch(L' ');
	}
	setcolor(0, 15);
	gotoxy(x1 - 1 + btn->pos.X + btnSize / 2 - (btnSize - 4) / 2 + (btnState == PRESSED), y1 + btn->pos.Y);
	_putws(btn->label);
	btn->ry = y1 + btn->pos.Y;
	btn->rx1 = x1 - 1 + btn->pos.X;
	btn->rx2 = x1 - 1 + btn->pos.X + btnSize - 1;
	openedDialog->tabPos[btn->id].X = btn->rx1;
	openedDialog->tabPos[btn->id].Y = btn->ry;
	if(btnState == PRESSED)
		pressedBtn = btn;
	else if(pressedBtn && btn == pressedBtn)
		pressedBtn = NULL;
	gotoxy(btn->rx1, btn->ry);
}

void showDialog(dialog)
	DIALOG *dialog;
{
	static unsigned char x1, y1, i, j, titleLength, length;
	static LABEL *label;
	x1 = 39 - dialog->width / 2;
	y1 = 11 - dialog->height / 2;
	gotoxy(x1, y1);
	for(i=y1; i<y1+dialog->height; i++) {
		setcolor(8, i == y1 ? 8 : 7);
		gotoxy(x1, i);
		if(i == y1)
			setcolor(8, bgColor), _putwch(L'▄');
		else
			_putwch(L'█');
		if(i == y1)
			setcolor(8, 8);
		else if(i == y1+dialog->height - 1)
			setcolor(8, 7);
		else
			setcolor(0, 7);
		for(j=x1+1; j<x1+dialog->width-1; j++)
			if(i == y1+dialog->height - 1)
				_putwch(L'▄');
			else if(i == y1 + 1 && (j == x1 + 1 || j == x1+dialog->width-2))
				setcolor(8, 7), _putwch(L'▀');
			else
				_putwch(L' ');
		if(i == y1)
			setcolor(8, bgColor), _putwch(L'▄');
		else
			setcolor(8, 7), _putwch(L'█');
		if(i > y1 + 1)	{
			setcolor(0, 0);
			wprintf(L"  ");
		} else if(i > y1) {
			setcolor(0, bgColor);
			_putwch(L'█');
			_putwch(L'▄');
		}
	}
	setcolor(0, 0);
	gotoxy(x1 + 2, y1+dialog->height);
	for(i=x1+1; i<x1+dialog->width+1; i++)
		_putwch(L' ');
	titleLength = wcsbytesz(dialog->title);
	setcolor(15, 8);
	gotoxy(39 - titleLength / 2 - 2, y1);
	wprintf(L" %ls ", dialog->title);

	openedDialog = dialog;
	for(i=0; i<dialog->buttonCount; i++)
		renderButton(&(dialog->buttons[i]), NORMAL);
	for(i=0; i<dialog->labelCount; i++) {
		label = &dialog->labels[i];
		length = wcsbytesz(label->caption);
		gotoxy(x1 - 1 + label->pos.X, y1 + label->pos.Y);
		setcolor(0, 7);
		_putws(label->caption);
	}

	setFullStatus(L"<리턴=실행>   <Esc=취소>   <탭=다음 항목>   <화살표=선택>");
	state = DIALOG_OPEN;

	tabPos = 0;
	if(dialog->tabStops)
		gotoxy(dialog->tabPos[0].X, dialog->tabPos[0].Y);
}

void cls()
{
	COORD pos = { 0, 0 };
	DWORD dw;
	FillConsoleOutputCharacter(output, ' ', 80 * 25, pos, &dw);
	gotoxy(0, 0);
} 

void drawContents()
{
	static unsigned char i, j;
	gotoxy(0, 1);
	setcolor(frColor, bgColor);
	wprintf(L" ");
	for(i=0; i<20; i++) {
		setcolor(frColor, bgColor);
		wprintf(L"                                                                             ");
		if(!i)
			setcolor(0, 7), _putwch('');
		else if(i == 19)
			setcolor(0, 7), _putwch('');
		else
			setcolor(7, 0), _putwch('*');
	}
	setcolor(frColor, bgColor);
	wprintf(L" ");
}

void drawMenu(flag)
	unsigned char flag;
{
	unsigned char highlightMenus = flag == 1;
	unsigned char openMenu = flag >= 2;
	static unsigned char ln, i, j;
	unsigned char openPos;
	unsigned char ml;
	unsigned char sz;
	unsigned char mpos = 2;
	MENU *selectedMenu;
	memset(mnuID, 255, 81);
	if(flag && flag < 3) drawContents();
	if(highlightMenus && state == MAIN)
		setStatus(L"<리턴=메뉴표시>   <Esc=취소>   <화살표=선택>");
	ln = 2;
	setcolor(0, 7);
	gotoxy(0, 0);
	wprintf(L"  ");
	for(i=0; i<menuCount; i++) {
		if(i == selectedMenuID && flag)
			setcolor(7, 0);
		else
			setcolor(0, 7);
		if(i == selectedMenuID)
			openPos = ln - 1;
		wprintf(L" ");
		sz = wcssz(menu[i].label);
		for(j=0; j<sz; j++) {
			if(j == menu[i].accelerator && highlightMenus) {
				if(i == selectedMenuID)
					setcolor(15, 0);
				else
					setcolor(15, 7);
			} else {
				if(i == selectedMenuID && flag)
					setcolor(7, 0);
				else
					setcolor(0, 7);
			}
			_putwch(menu[i].label[j]);
			setcolor(0, 7);
			mnuID[mpos++] = i;
			if(menu[i].label[j] > 255) {
				ln += 2;
				mnuID[mpos++] = i;
			} else {
				ln++;
			}
		}
		if(i == selectedMenuID && flag)
			setcolor(7, 0);
		else
			setcolor(0, 7);
		_putwch(L' ');
		setcolor(0, 7);
		ln += 2;
		mnuID[mpos++] = i;
		mnuID[mpos++] = i;
	}
	for(i=0; i<80-ln; i++)
		_putwch(L' ');
	if(highlightMenus)
		state = MENU_OPEN;
	if(openMenu) {
		if(flag == 2) {
			gotoxy(openPos, 1);
			setcolor(0, 7);
			wprintf(L"");
			ml = menu[selectedMenuID].subMenuCount;
			for(i=2; i<ml+2; i++) {
				setcolor(0, 7);
				gotoxy(openPos, i);
				wprintf(L"                        ");
				setcolor(0, 0);
				wprintf(L"  ");
			}
			setcolor(0, 7);
			gotoxy(openPos, i++);
			wprintf(L"");
			setcolor(0, 0);
			wprintf(L"  ");
			gotoxy(openPos + 2, i);
			wprintf(L"                          ");
			setcolor(0, 7);
			gotoxy(openPos + 1, 2);
			subMenuPos = 0;
		}
		selectedMenu = &menu[selectedMenuID];
		for(i=0; i<=selectedMenu->lastSubMenuID; i++) {
			if(selectedMenu->subMenu[i].label[0] == '-' && !(selectedMenu->subMenu[i].label[1])) {
				gotoxy(openPos, 2 + i);
				setcolor(0, 7);
				wprintf(L"");
				continue;
			}
			gotoxy(openPos + 1, 2 + i);
			ln = 1;
			if(i == subMenuPos)
				setcolor(7, 0);
			else
				setcolor(0, 7);
			wprintf(L" ");
			sz = wcssz(selectedMenu->subMenu[i].label);
			for(j=0; j<sz; j++) {
				if(j == selectedMenu->subMenu[i].accelerator) {
					if(i == subMenuPos)
						setcolor(15, 0);
					else
						setcolor(15, 7);
				} else {
					if(i == subMenuPos)
						setcolor(7, 0);
					else
						setcolor(0, 7);
				}
				_putwch(selectedMenu->subMenu[i].label[j]);
				setcolor(0, 7);
				if(selectedMenu->subMenu[i].label[j] > 255)
					ln += 2;
				else
					ln++;
			}
			if(i == subMenuPos && flag)
				setcolor(7, 0);
			else
				setcolor(0, 7);
			for(j=0; j<24-ln; j++)
				_putwch(' ');
		}
		setStatus(selectedMenu->subMenu[subMenuPos].description);
		state = SUBMENU_OPEN;
	}
	gotoxy(0, 0);
}

void _setStatus(msg, showInfo)
	const wchar_t *msg;
	unsigned char showInfo;
{
	gotoxy(0, 23);
	setcolor(15, statusBarBackground);
	if(showInfo)
		wprintf(L"                                                     줄 %05d, 칸 %03d  [삽입] ", line, col);
	else
		wprintf(L"                                                                               ");
	gotoxy(1, 23);
	_putws(msg);
	if(showInfo) {
		gotoxy(73, 23);
		if(inputMode == INSERT)
			wprintf(L"삽입");
		else
			wprintf(L"수정");
	}
	gotoxy(x+1, y+2);
}

void setStatus(msg)
	const wchar_t *msg;
{
	_setStatus(msg, 1);
}

void setFullStatus(msg)
	const wchar_t *msg;
{
	_setStatus(msg, 0);
}

void drawFrame()
{
	static unsigned char i, j;
	drawMenu(0);
	drawContents();
	setStatus(L"<Alt=메뉴>   <Ins=삽입/수정 전환>");
	gotoxy(x+1, y+2);

	state = MAIN;
	openedDialog = NULL;
}

void fixWindows10(argc, argv)
	unsigned short argc;
	char *argv[];
{
	HKEY hKey;
	DWORD regbuf;
	LONG hr;
	DWORD regset = 0x0;
	unsigned long regtyp = REG_DWORD, regszz = 1024;
    hr = RegOpenKeyEx(HKEY_CURRENT_USER, L"Console", 0, KEY_ALL_ACCESS, &hKey);
    if(hr == ERROR_SUCCESS) {
		hr = RegQueryValueEx(hKey, L"ForceV2", 0, &regtyp, (LPBYTE) &regbuf, &regszz);
		if(hr == ERROR_SUCCESS && regbuf == (DWORD) 1) {
			hr = RegSetValueEx(hKey, L"ForceV2", 0, REG_DWORD, (const BYTE*) &regset, sizeof(regset));
			RegCloseKey(hKey);
            if(hr == ERROR_SUCCESS) {
            	char cmd[1024] = "start ";
				strcat(cmd, argv[0]);
				system(cmd);
                exit(0);
				return;
            } else {
				printf(L"제목 표시줄 마우스 우클릭 후, 속성에 들어가서 밑에 있는 '레거시 콘솔 사용'에 체크하고 다시 실행해 주세요.\n");
				getch();
                exit(0);
				return;
            }
		} else {
			RegCloseKey(hKey);
		}
    }
}

unsigned short getField(s, l, x, y, flag, def, cbSuccess, cbFail)
    wchar_t* s;
    int l;
    int x;
    int y;
    int flag;
    const wchar_t* def;
	void (*cbSuccess)(wchar_t*);
	void (*cbFail)();
{
    wchar_t c;
    int cnt = 0;
    char sdef[300] = "";
    int lc = 0;
    int i, scnt, n;
    s[0] = 0;

    if(def[0]) {
		wcstombs(sdef, def, sizeof sdef);
        wcscpy(s, def);
        cnt = wcslen(def);
        wprintf(L"%s", def);
		lc = def[cnt - 1];
    }

    scnt = strlen(sdef);
    while(1) {
        c = _getwch();
		
		/* 리턴키 */
        if(c == 10 || c == 13) break;

        /* 특수키 */
        if(c == 224) {
            do c = _getwch();
            while(c == 224);
            continue;
        }

        /* ESC키 */
        if(c == 27) {
			s[0] = 0;
			(*cbFail)();
			return 0;
        }

        /* 지우기 */
        if(c == 8) {
			if(!scnt) continue;
			if(lc >= 128) scnt -= 2;
			else scnt--;
			s[cnt] = 0;
			cnt--;
			gotoxy(x + scnt, y);
			if(lc >= 128) printf("  ");
			else printf(" ");
			gotoxy(x + scnt, y);
			lc = s[cnt-1];
			continue;
        }

        if(scnt >= (c >= 128 ? l - 1 : l)) {
            _beep(950, 100);
            continue;
        }
        if(flag == 0) {
            if(!(c > 123 || (c >= 48 && c <= 57) || c == 32 || (c >= 65 && c <= 'Z') || (c >= 97 && c <= 'z'))) {
                _beep(950, 100);
                continue;
            }
        } else if(flag == 2) {
            if(!(c > 123 || (c >= 65 && c <= 'Z') || c == 32 || (c >= 97 && c <= 'z') || (c >= 48 && c <= 57))) {
                _beep(950, 100);
                continue;
            }
        } else if(flag == 1) {
            if(!(c == 32 || (c >= 48 && c <= 57))) {
                _beep(950, 100);
                continue;
            }
        }
        s[cnt] = c;
        _putwch(c);
        cnt++;
        if(c >= 255) scnt += 2;
        else scnt++;
        lc = c;
    }
    s[cnt] = 0;
	(*cbSuccess)(s);
    return cnt;
}

void init(callback)
	void (*callback)();
{
	INPUT_RECORD rec;
	DWORD dwNOER;
	DWORD mode;
	unsigned char mx, my;
	char key;
	unsigned char mnuStartPos;
	BUTTON *curbtn;

	setlocale(0, "");
	hwnd = GetConsoleWindow();
	input = GetStdHandle(STD_INPUT_HANDLE);
	output = GetStdHandle(STD_OUTPUT_HANDLE);
	system("mode con cols=80 lines=25");
	drawFrame();
	SetConsoleMode(output, ENABLE_PROCESSED_INPUT | ENABLE_MOUSE_INPUT);
	GetConsoleMode(input, &mode);
	SetConsoleMode(input, mode | ENABLE_MOUSE_INPUT);
	
	(*callback)();

	do {
		ReadConsoleInput(input, &rec, 1, &dwNOER);
		GetConsoleMode(input, &mode);
		SetConsoleMode(input, mode | ENABLE_MOUSE_INPUT);
		if(rec.EventType == MOUSE_EVENT) {
			if(!rec.Event.MouseEvent.dwButtonState && !rec.Event.MouseEvent.dwEventFlags)
				mouseState = MOUSEUP;
			else if(rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED)
				mouseState = MOUSEDOWN;
		}
		if(rec.EventType == MOUSE_EVENT && ((rec.Event.MouseEvent.dwButtonState & FROM_LEFT_1ST_BUTTON_PRESSED) || (!rec.Event.MouseEvent.dwButtonState && !rec.Event.MouseEvent.dwEventFlags))) {
			mx = rec.Event.MouseEvent.dwMousePosition.X;
			my = rec.Event.MouseEvent.dwMousePosition.Y;
			if(mouseState == MOUSEDOWN && my == 0 && state != DIALOG_OPEN && (state != SUBMENU_OPEN || (state == SUBMENU_OPEN && selectedMenuID != mnuID[mx]))) {  // 메뉴
				if(mnuID[mx] != 255) {
					selectedMenuID = mnuID[mx];
					drawMenu(2);
				}
			} else if(state == SUBMENU_OPEN) {
				unsigned char i;
				for(i=2; i<=80; i++)
					if(mnuID[i] == selectedMenuID) {
						mnuStartPos = i;
						break;
					}
				if(mx >= mnuStartPos && mx < mnuStartPos + 24 && my >= 2 && my < 2 + menu[selectedMenuID].lastSubMenuID + 1) {
					if(mouseState == MOUSEUP) {
						drawFrame();
						if(menu[selectedMenuID].subMenu[my - 2].label[0] != '-' || menu[selectedMenuID].subMenu[my - 2].label[1])
							(*(menu[selectedMenuID].subMenu[my - 2].callback))();
					} else if(my - 2 != subMenuPos) {
						subMenuPos = my - 2;
						drawMenu(3);
					}
				} else if(my) {
					drawFrame();
				}
			} else if(state == DIALOG_OPEN && mouseState == MOUSEDOWN && !pressedBtn) {
				unsigned char i;
				BUTTON *btn;
				for(i=0; i<openedDialog->buttonCount; i++) {
					btn = &(openedDialog->buttons[i]);
					if(btn->ry == my && mx >= btn->rx1 && mx <= btn->rx2) {
						renderButton(btn, PRESSED);
						break;
					}
				}
			} else if(state == DIALOG_OPEN && mouseState == MOUSEUP && pressedBtn) {
				curbtn = pressedBtn;
				renderButton(pressedBtn, NORMAL);
				(*(curbtn->callback))();
			}
		} else if(rec.EventType == KEY_EVENT) {
			key = rec.Event.KeyEvent.wVirtualKeyCode;
			// printf("(%d) ", key);
			if(state != DIALOG_OPEN && (key == 18 || (key == 121 && !rec.Event.KeyEvent.bKeyDown))) {
				if(rec.Event.KeyEvent.bKeyDown)
					selectedMenuID = -1;
				else
					selectedMenuID = 0;
				drawMenu(1);
			} else if(key == 27 && (state == MENU_OPEN || state == SUBMENU_OPEN) && rec.Event.KeyEvent.bKeyDown) {
				drawFrame();
			} else if(key == 37 && (state == MENU_OPEN || state == SUBMENU_OPEN) && rec.Event.KeyEvent.bKeyDown) {
				if(!selectedMenuID)
					selectedMenuID = menuCount - 1;
				else
					selectedMenuID--;
				drawMenu(state);
			} else if(key == 39 && (state == MENU_OPEN || state == SUBMENU_OPEN) && rec.Event.KeyEvent.bKeyDown) {
				if(selectedMenuID == menuCount - 1)
					selectedMenuID = 0;
				else
					selectedMenuID++;
				drawMenu(state);
			} else if((key == 40 || key == 13) && state == MENU_OPEN && rec.Event.KeyEvent.bKeyDown) {
				drawMenu(2);
			} else if(key == 38 && state == SUBMENU_OPEN && rec.Event.KeyEvent.bKeyDown) {
				if(!subMenuPos)
					subMenuPos = menu[selectedMenuID].lastSubMenuID;
				else
					subMenuPos--;
				if(menu[selectedMenuID].subMenu[subMenuPos].label[0] == '-' && !(menu[selectedMenuID].subMenu[subMenuPos].label[1]))
					subMenuPos--;
				drawMenu(3);
			} else if(key == 40 && state == SUBMENU_OPEN && rec.Event.KeyEvent.bKeyDown) {
				if(subMenuPos == menu[selectedMenuID].lastSubMenuID)
					subMenuPos = 0;
				else
					subMenuPos++;
				if(menu[selectedMenuID].subMenu[subMenuPos].label[0] == '-' && !(menu[selectedMenuID].subMenu[subMenuPos].label[1]))
					subMenuPos++;
				drawMenu(3);
			} else if(key >= 65 && key <= 'Z' && state == MENU_OPEN && rec.Event.KeyEvent.bKeyDown) {
				unsigned char i;
				for(i=0; i<menuCount; i++)
					if(key == menu[i].label[menu[i].accelerator]) {
						selectedMenuID = i;
						drawMenu(2);
						break;
					}
			} else if(key >= 65 && key <= 'Z' && state == SUBMENU_OPEN && rec.Event.KeyEvent.bKeyDown) {
				unsigned char i;
				for(i=0; i<=menu[selectedMenuID].lastSubMenuID; i++)
					if(key == menu[selectedMenuID].subMenu[i].label[menu[i].subMenu[i].accelerator]) {
						drawFrame();
						(*(menu[selectedMenuID].subMenu[i].callback))();
						break;
					}
			} else if(key == 13 && state == SUBMENU_OPEN && rec.Event.KeyEvent.bKeyDown) {
				drawFrame();
				(*(menu[selectedMenuID].subMenu[subMenuPos].callback))();
			} else if(key == 45 && state == MAIN && rec.Event.KeyEvent.bKeyDown) {
				inputMode = !inputMode;
				gotoxy(73, 23);
				if(inputMode == INSERT)
					wprintf(L"삽입");
				else
					wprintf(L"수정");
			} else if((key == 9 || key == 39) && state == DIALOG_OPEN && rec.Event.KeyEvent.bKeyDown) {
				tabPos++;
				if(tabPos >= openedDialog->tabStops)
					tabPos = 0;
				if(openedDialog->tabStops)
					gotoxy(openedDialog->tabPos[tabPos].X, openedDialog->tabPos[tabPos].Y);
			} else if(key == 37 && state == DIALOG_OPEN && rec.Event.KeyEvent.bKeyDown) {
				tabPos--;
				if(tabPos == 255)
					tabPos = openedDialog->tabStops - 1;
				if(tabPos == 255)
					tabPos = 0;
				if(openedDialog->tabStops)
					gotoxy(openedDialog->tabPos[tabPos].X, openedDialog->tabPos[tabPos].Y);
			} else if(state == DIALOG_OPEN && (key == 32 || key == 13) && rec.Event.KeyEvent.bKeyDown && !pressedBtn) {
				unsigned char i;
				BUTTON *btn;
				for(i=0; i<openedDialog->buttonCount; i++) {
					btn = &(openedDialog->buttons[i]);
					if(btn->ry == openedDialog->tabPos[tabPos].Y && btn->rx1 == openedDialog->tabPos[tabPos].X) {
						if(key == 13)
							(*(btn->callback))();
						else
							renderButton(btn, PRESSED);
						break;
					}
				}
			} else if(state == DIALOG_OPEN && key == 32 && !rec.Event.KeyEvent.bKeyDown && pressedBtn) {
				curbtn = pressedBtn;
				renderButton(pressedBtn, NORMAL);
				(*(curbtn->callback))();
			} else if(key >= 65 && key <= 'Z' && state == DIALOG_OPEN && rec.Event.KeyEvent.bKeyDown) {
				unsigned char i;
				BUTTON *btn;
				for(i=0; i<openedDialog->buttonCount; i++) {
					btn = &openedDialog->buttons[i];
					if(btn->accelerator < 255 && btn->label[btn->accelerator] == key) {
						(*(btn->callback))();
						break;
					}
				}
			}/* else if(key == 27 && state == DIALOG_OPEN && rec.Event.KeyEvent.bKeyDown) {
				drawFrame();
			}*/
		}
	} while(1);
}

