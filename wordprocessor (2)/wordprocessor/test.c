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
	MENU *mnuFile = addMenu(0, L"F/ȭ��", 0);
	MENU *mnuEdit = addMenu(1, L"E/����", 0);
	MENU *mnuHelp = addMenu(2, L"H/����", 0);

	addSubMenu(mnuFile, 0, L"N/�� ȭ��", 0, dummy, L"�� ȭ���� �ۼ��մϴ�");
	addSubMenu(mnuFile, 1, L"O/�ҷ�����...", 0, wpStart, L"ȭ���� ��ũ���� �н��ϴ�");
	addSubMenu(mnuFile, 2, L"S/����", 0, dummy, L"�ۼ��� ������ ��ũ�� ���ϴ�");
	addSubMenu(mnuFile, 3, L"A/�� �̸����� ����...", 0, dummy, L"�� �̸����� ������ ��ũ�� ���ϴ�");
	addMenuSeparator(mnuFile, 4);
	addSubMenu(mnuFile, 5, L"D/������", 0, dosShell, L"���� ��� ���� �����մϴ�");
	addSubMenu(mnuFile, 6, L"X/����", 0, wpExit, L"Ǯ�׸��� ������ ������ �����ϴ�");
	
	addSubMenu(mnuEdit, 0, L"U/���� ���     Ctrl+Z", 0, dummy, L"��� �� ������ ����մϴ�");
	addSubMenu(mnuEdit, 1, L"R/�����        Ctrl+Y", 0, dummy, L"��������� ������ ������մϴ�");
	addMenuSeparator(mnuEdit, 2);
	addSubMenu(mnuEdit, 3, L"L/���� �� ����", 0, dummy, L"������ ���ϴ�");
	addSubMenu(mnuEdit, 4, L"O/��ü ����", 0, dummy, L"���� ��ü ������ �����մϴ�");
	addMenuSeparator(mnuEdit, 5);
	addSubMenu(mnuEdit, 6, L"X/�߶󳻱�   Shift+Del", 0, dummy, L"������ ������ ����� ���۷� �ű�ϴ�");
	addSubMenu(mnuEdit, 7, L"C/�����ϱ�    Ctrl+Ins", 0, dummy, L"������ ������ ���ۿ� �����մϴ�");
	addSubMenu(mnuEdit, 8, L"P/���̱�     Shift+Ins", 0, dummy, L"������ ������ �ٿ��ֽ��ϴ�");
	
	addSubMenu(mnuHelp, 0, L"A/�������μ��� ����", 0, wpAbout, L"Ǯ�׸� ������ ���ϴ�");

	createDialog(&dlgAbout, 1, L"�������μ��� ����", 40, 8);
	addButton(&dlgAbout, 0, L"Ȯ ��", -1, 16, 5, drawFrame);
	addLabel(&dlgAbout, -1, L"�������μ��� �ڷᱸ�� �����", -1, 4, 2);
	addLabel(&dlgAbout, -1, L"�ǹ�ȣ 1,0", -1, 7, 3);
	
	createDialog(&dlgMain, 1, L" �������μ��� �����ϱ�", 52, 15);
	addButton(&dlgMain, 0, L"U/���ε�", 0, 6, 12, dummy);
	addButton(&dlgMain, 1, L"E/�����ϱ�", 0, 21, 12, dummy);
	addButton(&dlgMain, 2, L"�� ��", 0, 38, 12, drawFrame);
	addLabel(&dlgMain, -1, L"� �۾��� �Ͻðڽ��ϱ�?", -1, 5, 2);
	addLabel(&dlgMain, -1, L"(���� 2���� �ҷ��Խ��ϴ�)", -1, 8, 3);

	fixWindows10(argc, argv);
	system("title MS-DOS �������μ���");
	init();
	return 0;
}
*/