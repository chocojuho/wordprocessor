#include "tui.h"
/*
DIALOG dlgAbout, dlgMain;

void dummy() {}

void dosShell()
{
	setcolor(7, 0);
	cls();
	gotoxy(0, 1);
	system("cmd /k");
	init();
}

void wpExit()
{
	setcolor(7, 0);
	system("cls");
	gotoxy(0, 1);
	exit(0);
}

void wpAbout()
{
	showDialog(&dlgAbout);
}

void wpStart()
{
	showDialog(&dlgMain);
}

xxxxx(argc, argv)
	unsigned short argc;
	char *argv[];
{
	MENU *mnuFile = addMenu(0, L"F/화일", 0);
	MENU *mnuEdit = addMenu(1, L"E/편집", 0);
	MENU *mnuHelp = addMenu(2, L"H/도움말", 0);

	addSubMenu(mnuFile, 0, L"N/새 화일", 0, dummy, L"새 화일을 작성합니다");
	addSubMenu(mnuFile, 1, L"O/불러오기...", 0, wpStart, L"화일을 디스크에서 읽습니다");
	addSubMenu(mnuFile, 2, L"S/저장", 0, dummy, L"작성한 문서를 디스크에 씁니다");
	addSubMenu(mnuFile, 3, L"A/새 이름으로 저장...", 0, dummy, L"새 이름으로 문서를 디스크에 씁니다");
	addMenuSeparator(mnuFile, 4);
	addSubMenu(mnuFile, 5, L"D/나들이", 0, dosShell, L"도스 명령 셸을 실행합니다");
	addSubMenu(mnuFile, 6, L"X/종료", 0, wpExit, L"풀그림을 끝내고 도스로 나갑니다");
	
	addSubMenu(mnuEdit, 0, L"U/실행 취소     Ctrl+Z", 0, dummy, L"방금 한 편집을 취소합니다");
	addSubMenu(mnuEdit, 1, L"R/재실행        Ctrl+Y", 0, dummy, L"실행취소한 편집을 재실행합니다");
	addMenuSeparator(mnuEdit, 2);
	addSubMenu(mnuEdit, 3, L"L/내용 싹 비우기", 0, dummy, L"문서를 비웁니다");
	addSubMenu(mnuEdit, 4, L"O/전체 복사", 0, dummy, L"문서 전체 내용을 복사합니다");
	addMenuSeparator(mnuEdit, 5);
	addSubMenu(mnuEdit, 6, L"X/잘라내기   Shift+Del", 0, dummy, L"선택한 내용을 지우고 버퍼로 옮깁니다");
	addSubMenu(mnuEdit, 7, L"C/복사하기    Ctrl+Ins", 0, dummy, L"선택한 내용을 버퍼에 복사합니다");
	addSubMenu(mnuEdit, 8, L"P/붙이기     Shift+Ins", 0, dummy, L"버퍼의 내용을 붙여넣습니다");
	
	addSubMenu(mnuHelp, 0, L"A/워드프로세서 정보", 0, wpAbout, L"풀그림 정보를 봅니다");

	createDialog(&dlgAbout, 1, L"워드프로세서 정보", 40, 8);
	addButton(&dlgAbout, 0, L"확 인", -1, 16, 5, drawFrame);
	addLabel(&dlgAbout, -1, L"워드프로세서 자료구조 수행용", -1, 4, 2);
	addLabel(&dlgAbout, -1, L"판번호 1,0", -1, 7, 3);
	
	createDialog(&dlgMain, 1, L" 워드프로세서 시작하기", 52, 15);
	addButton(&dlgMain, 0, L"U/업로드", 0, 6, 12, dummy);
	addButton(&dlgMain, 1, L"E/수정하기", 0, 21, 12, dummy);
	addButton(&dlgMain, 2, L"취 소", 0, 38, 12, drawFrame);
	addLabel(&dlgMain, -1, L"어떤 작업을 하시겠습니까?", -1, 5, 2);
	addLabel(&dlgMain, -1, L"(파일 2개를 불러왔습니다)", -1, 8, 3);

	fixWindows10(argc, argv);
	system("title MS-DOS 워드프로세서");
	init();
	return 0;
}
*/