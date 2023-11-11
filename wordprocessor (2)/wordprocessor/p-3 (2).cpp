#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include <io.h>
#include <cstdio>
#include <windows.h>
#include <sstream>
#include <sys/stat.h>
#include "stack.h"
#include <direct.h>
#include "queue.h"
#include <mysql.h>
#include "msvcfix.h"
#include <cstdlib>
extern "C" {
#include "tui.h"
}
#pragma comment(lib, "\"C:\\Program Files\\MySQL\\MySQL Server 5.5\\lib\\libmysql.lib\"")
using namespace std;

struct Node {
    string data;
    Node* pre;
    Node() {
		data = '\0'; 
		pre = nullptr;
	}
    Node(Node* con, string v) {
		pre = con; data = v;
	}
};

MYSQL conn;
MYSQL* connPtr = nullptr;
MYSQL_RES* Result;
MYSQL_ROW Row;
Node* file[1010][1010];
vector<string> arr;
Queue<pair<int, string>> qu;
Stack<string> st;
int max_file;
int cnt;

DIALOG connError, fileCreated, fileCreateFail, fileNameInput, fileIDInput, fileIDInputU, fileOpenFail, selectEditMode, revInput, queued;
DIALOG dlgAbout, dlgMain;

void dummy() {}

void dosShell()
{
	setcolor(7, 0);
	cls();
	gotoxy(0, 1);
	system("cmd /k");
	init(dummy);
}

void wpExit()
{

	setcolor(7, 0);
	system("cls");
	gotoxy(0, 1);

	while (!qu.isEmpty()) {
		string query_check = "select * from word_processor where id = " + to_string(qu.Front().first);
		char cquery_check[100010]="";
		string query="";
		char cquery[100010]="";
		query_check.copy(cquery_check , query_check.size()+1);
		if (mysql_query(connPtr, cquery_check) == 0) {
			Result = mysql_store_result(connPtr);
			if(!(Row = mysql_fetch_row(Result))) {
				query = "insert into word_processor values(" + to_string(qu.Front().first) + ",'" + qu.Front().second + "');";
			} else {
				query = "update word_processor set content = '" + qu.Front().second + "' Where id = " + to_string(qu.Front().first) + ';';
			}
			cout << query << '\n';
			mysql_free_result(Result);
			query.copy(cquery, query.size() + 1);
			if (mysql_query(connPtr, cquery) == 0) {
				cout << qu.Front().first << " 파일을 업로드하였습니다." << '\n';
			}
		}
		qu.dequeue();
	}
	mysql_close(connPtr);

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

void showWelcomeDialog()
{
	drawFrame();
	static wchar_t msg[100];
	swprintf(msg, 90, L"(파일 %i개를 불러왔습니다)", cnt);
	dlgMain.labels[4].caption = msg;
	showDialog(&dlgMain);
}

void uploadWithSID(wchar_t *sid)
{
	int n;
	swscanf(sid, L"%d", &n);
	string buff;
    FILE* version_file;
    char file_router[100010] = "C:\\Users\\word_processor\\[";
    char file_router_number[10]; _itoa(n, file_router_number, 10);
    strcat(file_router, file_router_number); strcat(file_router, "]\\__version.txt");
    version_file = fopen(file_router, "r");
    if(!version_file) {
		showDialog(&fileOpenFail);
		return; 
	}
	int current = 1;
    while (1) {
        FILE* read_file;
        char read_version[100010];  
		if (!fgets(read_version, 100010, version_file))
			break;
        string temp = read_version;
        string one, two, three; istringstream iss(temp); iss >> one >> two >> three; // one 현재파일 버전 , two 어떤 연산자를 할지 , three 이전 참조 버전
        int a = stoi(one), b = stoi(three);
        if (two == "+") {
            char read_text[100010] = "C:\\Users\\word_processor\\[";
            char read_text_number[10];
            _itoa(a, read_text_number, 10);
            strcat(read_text, file_router_number); strcat(read_text, "]\\"); strcat(read_text, read_text_number); strcat(read_text, ".txt");
            read_file = fopen(read_text, "r");
            file[n][a] = new Node(file[n][a - 1], fgets(read_text, 100010, read_file)); current = max(a, current);
        }
        else if (two == "t") {
            file[n][a] = file[n][b]; current = max(a, current);
        }
        else if (two == "b") {
            file[n][a] = file[n][a - 2]; current = max(a, current);
        }
        else if (two == "base") {
            char read_text[100010] = "C:\\Users\\word_processor\\[";
            char read_text_number[100010];
            _itoa(a, read_text_number, 10);
            strcat(read_text, file_router_number);
            strcat(read_text, "]\\");
            strcat(read_text, read_text_number);
            strcat(read_text, ".txt");
            read_file = fopen(read_text, "r");
            file[n][a] = new Node(nullptr, fgets(read_text, 100010, read_file));
            current = max(a, current);
        }
    }
    // cout << current << '\n';
    Node* now = file[n][current];
    while (now) {
        st.push(now->data);
        now = now->pre;
    }
    while (!st.isEmpty()) {
        buff += st.Top();
        st.pop();
    }
    qu.enqueue(make_pair( n, buff ));
    showDialog(&queued);
}

void upload()
{
	wchar_t id[6];
	drawFrame();
	showDialog(&fileIDInputU);
	setFullStatus(L"<리턴=입력 완료>   <Esc=취소>");
	gotoxy(28, 11);
	setcolor(0, 7);
    getField(id, 5, 28, 11, 1, L"", uploadWithSID, showWelcomeDialog);
}

FILE* version_file;
int current;
char wrriter[100010];
FILE* write_new_file;
char file_router[100010];
char file_router_number[10];

void edit_addSave(wchar_t *w_wrriter)
{
	char make_new_file[100010] = "C:\\Users\\word_processor\\[";
    char new_file[100010];
	wcstombs(wrriter, w_wrriter, 100000);
    _itoa(++current, new_file, 10);
    strcat(make_new_file, file_router_number);
    strcat(make_new_file, "]\\");
    strcat(make_new_file, new_file);
    strcat(make_new_file, ".txt");
    write_new_file = fopen(make_new_file, "a+");
    fprintf(write_new_file, wrriter);
    fclose(write_new_file);
    version_file = fopen(file_router, "a");
    char plus[100010] = "";
    strcat(plus, new_file);
    strcat(plus, " + ");
    strcat(plus, "0\n");
    fprintf(version_file, plus);
    fclose(version_file);

	showDialog(&fileCreated);
}

void edit_add()
{
	drawFrame();
    wchar_t w_writter[66];
	showDialog(&fileNameInput);
	setFullStatus(L"<리턴=입력 완료>   <Esc=취소>");
	gotoxy(14, 11);
	setcolor(0, 7);
    getField(w_writter, 50, 14, 11, 0, L"", edit_addSave, showWelcomeDialog);
}

void edit_b(){
	char plus[100010] = "";
    char new_file[100010];
    _itoa(++current, new_file, 10);
    version_file = fopen(file_router, "a");
    strcat(plus, new_file);
    strcat(plus, " b ");
    strcat(plus, "0\n");
    fprintf(version_file, plus);
    fclose(version_file);
	
	showDialog(&fileCreated);
}

void edit_tSave(wchar_t *w_wrriter)
{
	wcstombs(wrriter, w_wrriter, 100000);
    char plus[100010] = "";
    char new_file[100010];
    _itoa(++current, new_file, 10);
    version_file = fopen(file_router, "a");
    strcat(plus, new_file);
    strcat(plus, " t ");
    strcat(plus, wrriter);
    strcat(plus, "\n");
    fprintf(version_file, plus);
    fclose(version_file);
	
	showDialog(&fileCreated);
}

void edit_t()
{
	drawFrame();
    wchar_t w_writter[7];
	showDialog(&revInput);
	setFullStatus(L"<리턴=입력 완료>   <Esc=취소>");
	gotoxy(28, 11);
	setcolor(0, 7);
    getField(w_writter, 5, 28, 11, 1, L"", edit_tSave, showWelcomeDialog);
}

void editWithSID(wchar_t *sid)
{
    int n;
	swscanf(sid, L"%d", &n);
    strcpy(file_router, "C:\\Users\\word_processor\\[");
    _itoa(n, file_router_number, 10);
    strcat(file_router, file_router_number); strcat(file_router, "]\\__version.txt");
    version_file = fopen(file_router, "r");
    if(!version_file) {
		showDialog(&fileOpenFail);
		return; 
	}
	current = 1;
    while (1) {
        FILE* read_file;
        char read_version[100010];  if (!fgets(read_version, 100010, version_file)) { break; }
        string temp = read_version;
        string one, two, three; istringstream iss(temp); iss >> one >> two >> three; // one 현재파일 버전 , two 어떤 연산자를 할지 , three 이전 참조 버전
        int a = stoi(one), b = stoi(three);
        if (two == "+") {
            char read_text[100010] = "C:\\Users\\word_processor\\[";
            char read_text_number[10];
            _itoa(a, read_text_number, 10);
            strcat(read_text, file_router_number); strcat(read_text, "]\\"); strcat(read_text, read_text_number); strcat(read_text, ".txt");
            read_file = fopen(read_text, "r");
            file[n][a] = new Node(file[n][a - 1], fgets(read_text, 100010, read_file)); current = max(a, current);
        }
        else if (two == "t") {
            file[n][a] = file[n][b]; current = max(a, current);
        }
        else if (two == "b") {
            file[n][a] = file[n][a - 2]; current = max(a, current);
        }
        else if (two == "base") {
            char read_text[100010] = "C:\\Users\\word_processor\\[";
            char read_text_number[100010];
            _itoa(a, read_text_number, 10);
            strcat(read_text, file_router_number);
            strcat(read_text, "]\\");
            strcat(read_text, read_text_number);
            strcat(read_text, ".txt");
            read_file = fopen(read_text, "r");
            file[n][a] = new Node(nullptr, fgets(read_text, 100010, read_file));
            current = max(a, current);
        }
    }
	static wchar_t verstr[12];
	swprintf(verstr, L"버 전: %i", current);
	selectEditMode.labels[1].caption = verstr;
	showDialog(&selectEditMode);
	
	setcolor(0, 7);
	gotoxy(7, 7);  _putws(L"┌ 내 용 ────────────-──────────────┐");
	gotoxy(7, 8);  _putws(L"│                                                            │");
	gotoxy(7, 9);  _putws(L"│                                                            │");
	gotoxy(7, 10); _putws(L"│                                                            │");
	gotoxy(7, 11); _putws(L"│                                                            │");
	gotoxy(7, 12); _putws(L"│                                                            │");
	gotoxy(7, 13); _putws(L"│                                                            │");
	gotoxy(7, 14); _putws(L"│                                                            │");
	gotoxy(7, 15); _putws(L"│                                                            │");
	gotoxy(7, 16); _putws(L"└──────────────────────────────┘");
	unsigned char yp = 8, pl = 0, i, len;
	char cdata[100010];
    Node* now = file[n][current];
	gotoxy(9, yp++);
    while(now) {
		strcpy(cdata, now->data.c_str());
		strcat(cdata, " ");
		len = strlen(cdata);
		for(i=0; i<len; i++) {
			putchar(cdata[i]);
			pl++;
			if(pl >= 60) {
				gotoxy(9, yp++);
				pl = 0;
			}
		}
        now = now->pre;
    }
	gotoxy(10, 18);
}

void edit()
{
	wchar_t id[6];
	drawFrame();
	showDialog(&fileIDInput);
	setFullStatus(L"<리턴=입력 완료>   <Esc=취소>");
	gotoxy(28, 11);
	setcolor(0, 7);
    getField(id, 5, 28, 11, 1, L"", editWithSID, showWelcomeDialog);
}

void createWithName(wchar_t *w_writter)
{
	FILE* mver = nullptr;
    FILE* make_base = nullptr;
    char writter[66];
	wcstombs(writter, w_writter, 60);
    char file_route[100010] = "C:\\Users\\word_processor\\[";
    char file_route_number[100010]; _itoa(++cnt, file_route_number, 10);
    strcat(file_route, file_route_number);
    strcat(file_route, "]");
	puts(file_route);
    if (_mkdir(file_route) == 0) {
        char file_make_version[100010]="";
        strcat(file_make_version,file_route);
        strcat(file_make_version, "\\__version.txt");
        // cout << file_make_version << '\n';
        mver = fopen(file_make_version, "a+");
        fprintf(mver, "1 base 0\n");
        fclose(mver);
        char file_make_base[100010] = "";
        strcat(file_make_base, file_route);
        strcat(file_make_base, "\\1.txt");
        make_base = fopen(file_make_base, "a+");
        fprintf(make_base, writter);
        fclose(make_base);
        showDialog(&fileCreated);
    }
    else {
        showDialog(&fileCreateFail);
    }
}

void create()
{
	drawFrame();
    wchar_t w_writter[66];
	showDialog(&fileNameInput);
	setFullStatus(L"<리턴=입력 완료>   <Esc=취소>");
	gotoxy(14, 11);
	setcolor(0, 7);
    getField(w_writter, 50, 14, 11, 0, L"", createWithName, showWelcomeDialog);
}

void setup()
{
	mysql_init(&conn);
    connPtr = mysql_real_connect(&conn, "127.0.0.1", "root", "1234", "word_processor", 3306, (char*) nullptr, 0);
    if(!connPtr) {
		showDialog(&connError);
		return;
    }
    while(1) {
        char file_route[10010] = "C:\\Users\\word_processor\\[";
        char file_route_number[22]; _itoa(++cnt, file_route_number, 10);
		static wchar_t msg[100];
        strcat(file_route, file_route_number);
        strcat(file_route, "]");
        if(_access(file_route, 0) == -1) {
            cnt -= 1;
			swprintf(msg, 90, L"(파일 %i개를 불러왔습니다)", cnt);
			addLabel(&dlgMain, -1, msg, -1, 8, 3);
            break;
        }
    }
	showDialog(&dlgMain);
}

int main(int argc, char *argv[])
{
	MENU *mnuFile = addMenu(0, L"F/화일", 0);
	MENU *mnuHelp = addMenu(2, L"H/도움말", 0);

	addSubMenu(mnuFile, 0, L"N/새 화일", 0, dummy, L"새 화일을 작성합니다");
	addSubMenu(mnuFile, 1, L"O/불러오기...", 0, wpStart, L"화일을 디스크에서 읽습니다");
	addSubMenu(mnuFile, 2, L"S/저장", 0, dummy, L"작성한 문서를 디스크에 씁니다");
	addSubMenu(mnuFile, 3, L"A/새 이름으로 저장...", 0, dummy, L"새 이름으로 문서를 디스크에 씁니다");
	addMenuSeparator(mnuFile, 4);
	addSubMenu(mnuFile, 5, L"D/나들이", 0, dosShell, L"도스 명령 셸을 실행합니다");
	addSubMenu(mnuFile, 6, L"X/종료", 0, wpExit, L"풀그림을 끝내고 도스로 나갑니다");
	
	addSubMenu(mnuHelp, 0, L"A/워드프로세서 정보", 0, wpAbout, L"풀그림 정보를 봅니다");

	createDialog(&connError, 1, L"초기화 오류", 40, 7);
	addButton(&connError, 0, L"Q/종료", 0, 16, 4, wpExit);
	addLabel(&connError, -1, L"[!] 데이타베이스를 열 수 없습니다.", -1, 4, 2);

	createDialog(&fileOpenFail, 1, L"오 류", 40, 7);
	addButton(&fileOpenFail, 0, L"확 인", 0, 16, 4, edit);
	addLabel(&fileOpenFail, -1, L"[!] 파일을 찾을 수 없습니다.", -1, 4, 2);

	createDialog(&fileCreated, 2, L"알 림", 40, 7);
	addButton(&fileCreated, 0, L"확 인", 0, 16, 4, showWelcomeDialog);
	addLabel(&fileCreated, -1, L"[i] 파일을 저장했습니다.", -1, 4, 2);

	createDialog(&queued, 2, L"알 림", 40, 7);
	addButton(&queued, 0, L"확 인", 0, 16, 4, showWelcomeDialog);
	addLabel(&queued, -1, L"[i] 업로드 대기 큐에 등록되었습니다.", -1, 4, 2);

	createDialog(&fileCreateFail, 3, L"오 류", 40, 7);
	addButton(&fileCreateFail, 0, L"확 인", 0, 16, 4, showWelcomeDialog);
	addLabel(&fileCreateFail, -1, L"[!] 파일을 만들 수 없습니다.", -1, 4, 2);

	createDialog(&fileNameInput, 3, L"문서 만들기", 60, 7);
	// addButton(&fileNameInput, -1, L"확 인", 0, 15, 6, showWelcomeDialog);
	// addButton(&fileNameInput, -1, L"취 소", 0, 37, 6, showWelcomeDialog);
	addLabel(&fileNameInput, -1, L"┌ 파일내용 ────────────────────┐", -1, 4, 2);
	addLabel(&fileNameInput, -1, L"│                                                  │", -1, 4, 3);
	addLabel(&fileNameInput, -1, L"└─────────────────────────┘", -1, 4, 4);

	createDialog(&fileIDInput, 3, L"문서 수정", 32, 7);
	addLabel(&fileIDInput, -1, L"┌ 파일번호 ──────┐", -1, 4, 2);
	addLabel(&fileIDInput, -1, L"│                      │", -1, 4, 3);
	addLabel(&fileIDInput, -1, L"└───────────┘", -1, 4, 4);

	createDialog(&revInput, 3, L"문서 수정", 32, 7);
	addLabel(&revInput, -1, L"┌ 파일 버전 번호 ───┐", -1, 4, 2);
	addLabel(&revInput, -1, L"│                      │", -1, 4, 3);
	addLabel(&revInput, -1, L"└───────────┘", -1, 4, 4);

	createDialog(&fileIDInputU, 3, L"문서 업로드", 32, 7);
	addLabel(&fileIDInputU, -1, L"┌ 파일번호 ──────┐", -1, 4, 2);
	addLabel(&fileIDInputU, -1, L"│                      │", -1, 4, 3);
	addLabel(&fileIDInputU, -1, L"└───────────┘", -1, 4, 4);
	
	createDialog(&dlgAbout, 4, L"워드프로세서 정보", 40, 8);
	addButton(&dlgAbout, 0, L"확 인", -1, 16, 5, drawFrame);
	addLabel(&dlgAbout, -1, L"워드프로세서 자료구조 수행용", -1, 4, 2);
	addLabel(&dlgAbout, -1, L"판번호 1,0", -1, 7, 3);
	
	createDialog(&dlgMain, 5, L" 워드프로세서 시작하기", 56, 15);
	addButton(&dlgMain, 0, L"C/만들기", 0, 4, 12, create);
	addButton(&dlgMain, 1, L"E/수정", 0, 18, 12, edit);
	addButton(&dlgMain, 2, L"U/업로드", 0, 30, 12, upload);
	addButton(&dlgMain, 3, L"종 료", 0, 44, 12, wpExit);
	addLabel(&dlgMain, -1, L"어떤 작업을 하시겠습니까?", -1, 5, 2);
	addLabel(&dlgMain, -1, L"- 만들기: 새 문서를 작성합니다.", -1, 6, 5);
	addLabel(&dlgMain, -1, L"- 수정: 문서를 수정합니다.", -1, 6, 6);
	addLabel(&dlgMain, -1, L"- 업로드: 문서를 데이타베이스에 올립니다.", -1, 6, 7);

	createDialog(&selectEditMode, 5, L"문서 수정", 70, 19);
	addButton(&selectEditMode, 0, L"+/내용추가", 0, 7, 16, edit_add);
	addButton(&selectEditMode, 1, L"B/실행취소", 0, 23, 16, edit_b);
	addButton(&selectEditMode, 2, L"T/버전선택", 0, 39, 16, edit_t);
	addButton(&selectEditMode, 3, L"취 소", 0, 55, 16, showWelcomeDialog);
	addLabel(&selectEditMode, -1, L"어떻게 수정하시겠습니까?", -1, 5, 2);
	addLabel(&selectEditMode, -1, L"버 전: 1", -1, 7, 4);

	fixWindows10(argc, argv);
	system("title MS-DOS 워드프로세서");
	init(setup);
}