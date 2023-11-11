#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <vector>
#include <string.h>
#include <io.h>
#include <stdio.h>
#include <Windows.h>
#include <sstream>
using namespace std;

#if (_MSC_VER < 1500)
#define nullptr NULL

int stoi(string s)
{
	return atoi(s.c_str());
}

#endif


struct Node {
    string data;
    Node* pre;
    Node() {data = "\0"; pre = nullptr;}
    Node(Node* con, string v) {pre = con;data = v;}
};

Node* file[1010][1010];
vector<string> arr;
int cnt;

int main()
{
    cout << "워드 프로세서" << '\n';
    while(1) {
        char file_route[100010] = "C:\\Users\\word_processor\\["; 
        char file_route_number[100010]; _itoa(++cnt, file_route_number, 10);
        strcat(file_route, file_route_number);
        strcat(file_route, "]");
        if (_access(file_route, 0) == -1) {
            cout << cnt - 1 << " were founded" << '\n';
            break;
        }
    }

    while (1) {
        string str;
        cout << "어떤것을 할까요?" << '\n';
        cin >> str;
        if (str == "업로드" || str == "넣기") {
            cout << "Upload in queue" << '\n';
        }
        else if (str == "수정" || str == "변경") {
            FILE *version_file;
            int n;
            cin >> n;
            char file_router[100010] = "C:\\Users\\word_processor\\[";
            char file_router_number[10]; _itoa(n, file_router_number, 10);
            Node* now = file[99999][999];
            char wrriter[100010];
            string alpha;
            FILE* fp3;
            strcat(file_router, file_router_number);strcat(file_router, "]\\__version.txt");
            version_file = fopen(file_router, "r");
            if (!version_file) {cout << "해당 파일이 존재하지 않습니다." << '\n';    continue;}  int current = 1;
            while (1) {
                FILE* read_file;
                char read_version[100010];  
                string temp = read_version; 
                string one, two, three; istringstream iss(temp); 
				if (!fgets(read_version, 100010, version_file)) {break;}
				iss >> one >> two >> three; // one 현재파일 버전 , two 어떤 연산자를 할지 , three 이전 참조 버전
                int a = stoi(one), b = stoi(three);
                if (two == "+") {
                    char read_text[100010] = "C:\\Users\\word_processor\\[";
                    char read_text_number[10];
                    _itoa(a, read_text_number, 10);
                    strcat(read_text, file_router_number);strcat(read_text, "]\\");strcat(read_text, read_text_number);strcat(read_text, ".txt");
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
                    file[n][a] = new Node(nullptr, fgets(read_text, 100010, read_text));
                    current = max(a, current);
                }
            }
            cout << current << '\n';
            while (now) {
                cout << now->data << ' ';
                now = now->pre;
            }
            cin >> alpha >> wrriter;
            if (alpha == "+") {
                char temp3[100010] = "C:\\Users\\word_processor\\[";
                char now3[100010];
                _itoa(++current, now3, 10);
                strcat(temp3, nower);
                strcat(temp3, "]\\");
                strcat(temp3, now3);
                strcat(temp3, ".txt");
                fp3 = fopen(temp3, "a+");
                fprintf(fp3, wrriter);
                fp = fopen(temp, "a");
                char plus[100010];
                strcat(plus, now3);
                strcat(plus, " + ");
                strcat(plus, "0\n");
                fprintf(fp, plus);
            }
            else if (alpha == "b") {
            }
            else {
                break;
            }
            cout << '\n';
        }
    }
}
