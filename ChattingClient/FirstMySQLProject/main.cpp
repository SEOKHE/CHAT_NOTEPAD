#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock ������� include. WSADATA �������.
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <sstream>
#include <mysql/jdbc.h>
#include <conio.h>
#include <thread>
#include <vector>
#include <windows.h>

#define MAX_SIZE 1024

using std::cout;
using std::cin;
using std::endl;
using std::string;

// MySQL Connector/C++ �ʱ�ȭ
sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ ��
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ� (3306: MySQL �����ͺ��̽� �⺻ ��Ʈ ��ȣ)
const string username = "root"; // �����ͺ��̽� �����
const string password = "1234"; // �����ͺ��̽� ���� ��й�ȣ
sql::ResultSet* result; //����� ������ ���� �ʿ��� �κ�

SOCKET client_sock;
string npw, spw, sid, sname, sbirth, sphone; // SELECT�� �ǹ��ϴ� s�� ������ �տ� ����

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            msg = buf;
            //�г��� : �޽���
            std::stringstream ss(msg);  // ���ڿ��� ��Ʈ��ȭ
            string user;
            ss >> user; // ��Ʈ���� ����, ���ڿ��� ���� �и��� ������ �Ҵ�
            if (user != sname) cout << buf << endl; // ���� ���� �� �ƴ� ��쿡�� ����ϵ���.
        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}


void login() {

    int button = 0;
    int count = 5;
    string okay;

    while (1) {
        system("cls");
        cout << "�� id�� �Է����ּ���." << endl << endl;
        cout << "�� ";
        cin >> sid;
        cout << endl << endl;
        pstmt = con->prepareStatement("SELECT id FROM user WHERE id = ?;");
        pstmt->setString(1, sid);
        result = pstmt->executeQuery();
        count = 5;

        if (result->next()) { //if���� ���� ���� �߰�
            while (count > 0) {
                cout << "��� ��ȣ 5ȸ �Է� ���н� ���� ȭ������ ���ư��ϴ�. (���� " << count << "ȸ ����)" << endl << endl;
                cout << "�� pw�� �Է����ּ���." << endl << endl;
                cout << "�� ";
                spw = "";
                char ch = ' ';
                while (ch != 13) { // Enter Ű�� ������ �Է� ����
                    ch = _getch();
                    if (ch == 13) break; // Enter Ű�� ������ �Է� ����
                    if (ch == 8) { // Backspace Ű�� ���
                        if (!spw.empty()) { // �Էµ� ���ڰ� ������
                            spw.pop_back(); // ������ ���ڸ� ����
                            cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                        }
                    }
                    else {
                        spw.push_back(ch);
                        cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                    }
                }
                cout << endl << endl;
                pstmt = con->prepareStatement("SELECT name, pw FROM user WHERE id = ? AND pw = ?;");
                pstmt->setString(1, sid);
                pstmt->setString(2, spw);
                result = pstmt->executeQuery();

                if (result->next()) { //if���� ���� ���� �߰�
                    system("cls");
                    cout << "�α����� �Ϸ�Ǿ����ϴ�." << endl << endl;
                    sname = result->getString("name");
                    cout << sname << "�� ȯ���մϴ�!";
                    Sleep(3000);
                    count = 5;
                    button = 1;
                    break;
                }
                else {
                    system("cls");
                    cout << "�߸��� PW�Դϴ�. �ٽ� �Է����ּ���." << endl << endl;
                    count--;
                }
                if (button == 1) {
                    break;
                }
            }
        }
        else {
            cout << "�������� �ʴ� ID�Դϴ�. �ٽ� �Է����ּ���." << endl << endl;
            Sleep(1000);
        }
        if (button == 1) {
            break;
        }
    }
}

void id_search() {
    system("cls");
    cout << "�� �̸��� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sname;
    cout << endl << endl;
    cout << "�� '-' ���� ��ȭ��ȣ�� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sphone;
    cout << endl << endl;
    cout << "�� ���� 8�ڸ��� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sbirth;
    cout << endl << endl;

    pstmt = con->prepareStatement("SELECT id FROM user WHERE name = ? AND phone = ? AND birth = ?;");
    pstmt->setString(1, sname);
    pstmt->setString(2, sphone);
    pstmt->setString(3, sbirth);
    result = pstmt->executeQuery();//SELECT�� ��� ����

    if (result->next()) { //if���� ���� result�ȿ� �ִ� ������� ��� ��½�Ű�� ��º�
        system("cls");
        cout << endl << sname << "���� ID�� " << result->getString(1) << " �Դϴ�." << endl << endl;
        Sleep(3000);
    }
    else { 
        system("cls");
        cout << "�������� �ʴ� ȸ���Դϴ�." << endl << endl; 
        Sleep(1000);
    }
}

void pw_search() {
    system("cls");
    cout << "�� ID�� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sid;
    cout << endl << endl;
    cout << "�� �̸��� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sname;
    cout << endl << endl;
    cout << "�� '-' ���� ��ȭ��ȣ�� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sphone;
    cout << endl << endl;
    cout << "�� ���� 8�ڸ��� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sbirth;
    cout << endl << endl;

    pstmt = con->prepareStatement("SELECT pw FROM user WHERE id = ? AND name = ? AND birth = ? AND phone = ?;");
    pstmt->setString(1, sid);
    pstmt->setString(2, sname);
    pstmt->setString(3, sbirth);
    pstmt->setString(4, sphone);
    result = pstmt->executeQuery();

    if (result->next()) { //if���� ���� ���� �߰�

        while (1) {
            system("cls");
            cout << "�� ������ pw�� �Է����ּ���" << endl << endl;
            cout << "�� ";
            spw = "";
            char ch = ' ';
            while (ch != 13) { // Enter Ű�� ������ �Է� ����
                ch = _getch();
                if (ch == 13) break; // Enter Ű�� ������ �Է� ����
                if (ch == 8) { // Backspace Ű�� ���
                    if (!spw.empty()) { // �Էµ� ���ڰ� ������
                        spw.pop_back(); // ������ ���ڸ� ����
                        cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                    }
                }
                else {
                    spw.push_back(ch);
                    cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                }
            }
            cout << endl << endl;

            cout << "�� �ѹ� �� �Է����ּ���" << endl << endl;
            cout << "�� ";

            npw = "";
            char ch2 = ' ';
            while (ch2 != 13) { // Enter Ű�� ������ �Է� ����
                ch2 = _getch();
                if (ch2 == 13) break; // Enter Ű�� ������ �Է� ����
                if (ch2 == 8) { // Backspace Ű�� ���
                    if (!npw.empty()) { // �Էµ� ���ڰ� ������
                        npw.pop_back(); // ������ ���ڸ� ����, Ȥ�� ���Ŀ� �迭�� ���� �����ؼ� ������ �߻��� �ܿ� clear()�� ����ؼ� �ذ��� �� ����
                        cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                    }
                }
                else {
                    npw.push_back(ch2);
                    cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                }
            }

            cout << endl << endl;
            if (spw != npw) { //if���� ���� ��ġ���� �˻�
                cout << "��й�ȣ�� ��ġ���� �ʽ��ϴ�. �ٽ� �Է����ּ���." << endl << endl;
                Sleep(1000);
            }
            else break;
        }

        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?;");
        pstmt->setString(1, npw);
        pstmt->setString(2, sid);
        result = pstmt->executeQuery();

        cout << "PW ������ �Ϸ�Ǿ����ϴ�." << endl << endl;
        Sleep(1000);
    }
    else {
        cout << "�������� �ʴ� ȸ���Դϴ�." << endl << endl;
        Sleep(1000);
    }
}

void sign_up() {
    system("cls");
    cout << "ȸ�� ������ �����ϰڽ��ϴ�." << endl;
    //�̸�, ����, ��ȭ��ȣ, ���̵�, ��� ������ �Է¹ޱ�
    //INSERT INTO ����Ұ�.

    cout << endl << "�� �̸��� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sname;
    cout << endl << endl;
    cout << "�� ���� 8�ڸ��� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sbirth;
    cout << endl << endl;
    cout << "�� '-' ���� ��ȭ��ȣ�� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sphone;
    cout << endl << endl;
    cout << "�� ID�� �Է����ּ���" << endl << endl;
    cout << "�� ";
    cin >> sid;
    cout << endl << endl;
    cout << "�� PW�� �Է����ּ���" << endl << endl;
    cout << "�� ";
    spw = "";
    char ch = ' ';
    while (ch != 13) { // Enter Ű�� ������ �Է� ����
        ch = _getch();
        if (ch == 13) break; // Enter Ű�� ������ �Է� ����
        if (ch == 8) { // Backspace Ű�� ���
            if (!spw.empty()) { // �Էµ� ���ڰ� ������
                spw.pop_back(); // ������ ���ڸ� ����
                cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
            }
        }
        else {
            spw.push_back(ch);
            cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
        }
    }
    cout << endl << endl;
    
    pstmt = con->prepareStatement("SELECT name, birth, phone, id, pw FROM user WHERE name = ? OR phone = ? OR id = ?");
    pstmt->setString(1, sname);
    pstmt->setString(2, sphone);
    pstmt->setString(3, sid);
    result = pstmt->executeQuery();

    if (result->next()) { //if���� ���� ���� �߰�
        system("cls");
        cout << "�̹� �����ϴ� ȸ���Դϴ�." << endl;
    }
    else {
        pstmt = con->prepareStatement("INSERT INTO user(name, birth, phone, id, pw) VALUES(?, ?, ?, ?, ?);");
        pstmt->setString(1, sname);
        pstmt->setString(2, sbirth);
        pstmt->setString(3, sphone);
        pstmt->setString(4, sid);
        pstmt->setString(5, spw);
        result = pstmt->executeQuery();

        system("cls");
        cout << "ȸ�� ������ �Ϸ�Ǿ����ϴ�." << endl;
        cout << sname << "�� ȯ���մϴ�!" << endl;
        Sleep(3000);

    }
}

void main_My_profile() {
    int  c_num, button = 0;
    string instatus, insongs, okay;
    while (1) {
        okay = 1;
        system("cls");
        cout << ">> �� ���� <<" << endl << endl;
        cout << "�� 1. �� ���� ����" << endl << endl << "�� 2. ��� ���� / ����" << endl << endl <<  "�� 3. �뷡 ���� / ����"  << endl << endl << "�� 0. �ڷ� ����" << endl << endl;
        cout << "�� ";
        cin >> c_num;
        cout << endl << endl;

        switch (c_num) {
            case 1:
                system("cls");
                cout << ">> �� ���� <<" << endl << endl;
                pstmt = con->prepareStatement("SELECT status, songs, RIGHT(birth, 4) AS 'birth' FROM user WHERE id = ? AND pw = ?;");
                pstmt->setString(1, sid);
                pstmt->setString(2, spw);
                result = pstmt->executeQuery();

                while (result->next()) {
                    cout << "���� �޽��� : " << result->getString("status") << endl;
                    cout << "���� ���� ���� �뷡 : " << result->getString("songs") << endl;
                    cout << "���� : " << result->getString("birth") << endl;
                }
                cout << endl << endl;

                cout << "�� ���� �������� ������ �ƹ� Ű�� �Է����ֽʽÿ�." << endl << endl;
                cout << "�� ";
                cin >> okay;

                if (okay == "0") {
                    break;
                }
                else continue;

            case 2:
                system("cls");
                cout << ">> ���� �޽��� ����/���� <<" << endl << endl;
                cout << "������ ���� �޽����� �Է����ּ���" << endl;
                cin.ignore();

                getline(cin, instatus);
                cout << endl << endl;

                pstmt = con->prepareStatement("UPDATE user SET status = ? WHERE id = ? AND pw = ?;");
                pstmt->setString(1, instatus);
                pstmt->setString(2, sid);
                pstmt->setString(3, spw);
                result = pstmt->executeQuery();
                cout << endl << endl;

                cout << "�� ���� �޽��� ������ �Ϸ�Ǿ����ϴ�." << endl << endl;
                Sleep(1000);
                break;

            case 3:
                system("cls");
                cout << ">> �뷡 ����/���� <<" << endl;
                cout << "������ �뷡 ������ �Է����ּ���" << endl;
                cin.ignore();
                getline(cin, insongs);
                cout << endl << endl;

                pstmt = con->prepareStatement("UPDATE user SET songs = ? WHERE id = ? AND pw = ?;");
                pstmt->setString(1, insongs);
                pstmt->setString(2, sid);
                pstmt->setString(3, spw);
                result = pstmt->executeQuery();
                cout << endl << endl;
                
                cout << "�� �뷡 ������ �Ϸ�Ǿ����ϴ�." << endl << endl;
                Sleep(1000);
                break;

            case 0:
                system("cls");
                button = 1;
                break;

            default:
                
                cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
                Sleep(1000);
                break;
        }
        if (button == 1) {
            break;
        }
    }

}

void main_Friends() {
    int  c_num, button = 0;
    string okay,instatus, insongs, inbirth1, inbirth2;
    
    while (1) {
        system("cls");
        cout << ">> ģ�� <<" << endl << endl;
        cout << "�� 1. ģ�� ��� ����" << endl << endl << "�� 2. ���� �˻�" << endl << endl << "�� 0. �ڷ� ����" << endl << endl;
        cout << "�� ";
        cin >> c_num;
        cout << endl << endl;

        switch (c_num) {
        case 1:
            system("cls");
            cout << ">> ģ�� ��� <<" << endl << endl;
            pstmt = con->prepareStatement("SELECT name, status, songs, RIGHT(birth, 4) AS 'birth' FROM user WHERE id != ? AND pw != ? ORDER BY name;");
            pstmt->setString(1, sid);
            pstmt->setString(2, spw);
            result = pstmt->executeQuery();

            while (result->next()) {
                cout << endl << "------------------------------" << endl;
                cout << "�̸� : " << result->getString("name") << endl;
                cout << "���� �޽��� : " << result->getString("status") << endl;
                cout << "���� ���� ���� �뷡 : " << result->getString("songs") << endl;
                cout << "���� : " << result->getString("birth") << endl;
                cout << endl << "------------------------------" << endl;
            }
            cout << endl << endl;

            cout << "�� ���� �������� ������ �ƹ� Ű�� �Է����ֽʽÿ�." << endl << endl;
            cout << "�� ";
            cin >> okay;

            if (okay == "0") {
                break;
            }
            else continue;

        case 2:
            
            system("cls");
            cout << ">> ���� �˻��ϱ� <<" << endl << endl;
            cout << "�� ��ȸ�� ������ ���� 4�ڸ��� �Է����ּ���. (ex. 0101)" << endl << endl;
            cout << "�� ";
            cin >> inbirth1;
            cout << endl << endl;

            cout << "�� ��ȸ�� ���� ���� 4�ڸ��� �Է����ּ���. (ex. 0101)" << endl << endl;
            cout << "�� ";
            cin >> inbirth2;
            
            pstmt = con->prepareStatement("SELECT name, RIGHT(birth, 4) AS 'birth' FROM user WHERE RIGHT(birth, 4) > ? AND RIGHT(birth, 4) < ? ORDER BY birth;");
            pstmt->setString(1, inbirth1);
            pstmt->setString(2, inbirth2);
            result = pstmt->executeQuery();
            cout << endl;

            if (result->next()) { //if���� ���� result�ȿ� �ִ� ������� ��� ��½�Ű�� ��º�
                while (1) {
                    cout << endl << "------------------------------" << endl;
                    cout << "�̸� : " << result->getString("name") << endl;
                    cout << "���� : " << result->getString("birth") << endl;
                    cout << endl << "------------------------------" << endl;
                    if (!result->next()) {
                        break;
                    }
                }
            }
            else {
                cout << "��ȸ�Ͻ� �Ⱓ ���� ������ ģ���� �����ϴ�." << endl << endl;
                cout << "1�� �� ���� �������� �̵��մϴ�." << endl << endl;
                Sleep(1000);
                break;
            }
            
            cout << "�� ���� �������� ������ �ƹ� Ű�� �Է����ֽʽÿ�." << endl << endl;
            cout << "�� ";
            cin >> okay;

            if (okay == "0") {
                break;
            }
            else continue;

        case 0:
            button = 1;
            break;

        default:
            cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
            cout << "1�� �� ���� �������� �̵��մϴ�." << endl;
            Sleep(1000);
            break;
        }
        if (button == 1) {
            break;
        }
    }
}

void chatting() {
    system("cls");
    cout << "ä�ù濡 �����մϴ�." << endl;
    WSADATA wsa;

    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {

        client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  

        SOCKADDR_IN client_addr = {};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(7777);
        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
                cout << "Server Connect" << endl;
                send(client_sock, sname.c_str(), sname.length(), 0);
                break;
            }
            cout << "Connecting..." << endl;
        }

        std::thread th2(chat_recv);

        pstmt = con->prepareStatement("SELECT name, time, chat FROM chat;");
        result = pstmt->executeQuery();
        while (result->next()) {
            cout << endl << "---------------------------------------------------------" << endl;
            cout << "�̸� : " << result->getString("name") << endl;
            cout << "ä�� : " << result->getString("chat") << endl;
            cout << "���� �ð� : " << result->getString("time") << endl;
            cout << "---------------------------------------------------------" << endl;
        }

        while (1) {
            string text;
            cout << "  ";
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string���� char* Ÿ������ ��ȯ
            send(client_sock, buffer, strlen(buffer), 0);
            if (text == "/quit") {
                closesocket(client_sock);
                break;
            }
        }
        th2.join();
        //closesocket(client_sock);
    }
}

int setting() {
    int num, c_num, button = 0, o_button = 0;
    string hpw;
    char ch = ' ';
    npw = "";
    char ch2 = ' ';
    
    while (1) {
        system("cls");
        cout << endl << ">> ���� <<" << endl << endl;
        cout << "�� 1. ��й�ȣ ����" << endl << endl << "�� 2. ȸ�� Ż��" << endl << endl << "�� 0. �ڷ� ����" << endl << endl;
        cout << "�� ";
        cin >> num;
        cout << endl << endl;

        switch (num) {
            case 1:
                system("cls");
                cout << "���� pw�� �Է����ּ���" << endl;
                cout << "�� ";
                ch = ' ';
                ch2 = ' ';

                hpw = "";

                while (ch != 13) { // Enter Ű�� ������ �Է� ����
                    ch = _getch();
                    if (ch == 13) break; // Enter Ű�� ������ �Է� ����
                    if (ch == 8) { // Backspace Ű�� ���
                        if (!hpw.empty()) { // �Էµ� ���ڰ� ������
                            hpw.pop_back(); // ������ ���ڸ� ����
                            cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                        }
                    }
                    else {
                        hpw.push_back(ch);
                        cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                    }
                }
                cout << endl << endl;

                pstmt = con->prepareStatement("SELECT pw FROM user WHERE pw = ?;");
                pstmt->setString(1, hpw);
                result = pstmt->executeQuery();

                if (result->next()) { //if���� ���� ���� �߰�

                    while (1) {
                        system("cls");
                        cout << "�� ������ pw�� �Է����ּ���" << endl << endl;
                        cout << "�� ";
                        spw = "";
                        ch = ' ';
                        while (ch != 13) { // Enter Ű�� ������ �Է� ����
                            ch = _getch();
                            if (ch == 13) break; // Enter Ű�� ������ �Է� ����
                            if (ch == 8) { // Backspace Ű�� ���
                                if (!spw.empty()) { // �Էµ� ���ڰ� ������
                                    spw.pop_back(); // ������ ���ڸ� ����
                                    cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                                }
                            }
                            else {
                                spw.push_back(ch);
                                cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                            }
                        }
                        cout << endl << endl;

                        cout << "�� �ѹ� �� �Է����ּ���" << endl << endl;
                        cout << "�� ";

                        npw = "";
                        ch2 = ' ';
                        while (ch2 != 13) { // Enter Ű�� ������ �Է� ����
                            ch2 = _getch();
                            if (ch2 == 13) break; // Enter Ű�� ������ �Է� ����
                            if (ch2 == 8) { // Backspace Ű�� ���
                                if (!npw.empty()) { // �Էµ� ���ڰ� ������
                                    npw.pop_back(); // ������ ���ڸ� ����, Ȥ�� ���Ŀ� �迭�� ���� �����ؼ� ������ �߻��� �ܿ� clear()�� ����ؼ� �ذ��� �� ����
                                    cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                                }
                            }
                            else {
                                npw.push_back(ch2);
                                cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                            }
                        }

                        cout << endl << endl;
                        if (spw != npw) { //if���� ���� ��ġ���� �˻�
                            cout << "��й�ȣ�� ��ġ���� �ʽ��ϴ�. �ٽ� �Է����ּ���." << endl << endl;
                            Sleep(1000);
                        }
                        else break;
                    }

                    pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?;");
                    pstmt->setString(1, npw);
                    pstmt->setString(2, sid);
                    result = pstmt->executeQuery();

                    cout << "PW ������ �Ϸ�Ǿ����ϴ�." << endl << endl;
                    cout << "1�� �� ���� �������� �̵��մϴ�." << endl;
                    Sleep(1000);
                }
                break;

            case 2:
                system("cls");
                cout << "�������� ȸ��Ż�� ���Ͻʴϱ�?" << endl;
                cout << "�� 1. ��" << endl << endl << "�� 2. �ƴϿ�" << endl << endl;
                cin >> c_num;
                cout << endl << endl;
                switch (c_num) {
                    case 1:
                        system("cls");
                        pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?;");
                        pstmt->setString(1, sid);
                        result = pstmt->executeQuery();

                        cout << "ȸ�� Ż�� ���������� �Ϸ�Ǿ����ϴ�." << endl;
                        cout << "�̿����ּż� �����߽��ϴ�." << endl;
                        cout << "2�� �� �ڵ����� ����˴ϴ�." << endl;
                        Sleep(2000);
                        button = 1;
                        o_button = 1;
                        break;

                    case 2:
                        cout << "ȸ�� Ż�� ��ҵǾ����ϴ�." << endl;
                        Sleep(1000);
                        break;

                    default:
                        cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
                        Sleep(1000);
                        break;
                }

            case 0:
                system("cls");
                button = 1;
                break;

            default:
                cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
                cout << "1�� �� ���� �������� �̵��մϴ�." << endl;
                Sleep(1000);
                break;
        }

        if (button == 1) {
            break;
        }
    }
    
    return o_button;
}

int main()
{

    // MySQL ����
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(server, username, password); // MySQL �����ͺ��̽� ���� ��ü
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        exit(1);
    }

    // �����ͺ��̽� ����
    con->setSchema("chatdb");

    // db �ѱ� ������ ���� ���� 
    stmt = con->createStatement();
    stmt->execute("set names euckr");
    if (stmt) { delete stmt; stmt = nullptr; }

    int num, button = 0;

    while (1) {
        system("cls");
        cout << ">> ��ȣ�� �Է����ּ��� <<" << endl << endl;
        cout << "�� 1. �α���" << endl << endl << "�� 2. ID ã��" << endl << endl << "�� 3. PW ã��" << endl << endl << "�� 4. ȸ������" << endl << endl << "�� 0. ����" << endl << endl;
        cin >> num;
        switch (num) {
            case 1:
                login();
                button = 2;
                break;

            case 2:
                id_search();
                break;

            case 3:
                pw_search();
                break;

            case 4:
                sign_up();
                break;

            case 0:
                button = 1;
                break;

            default:
                cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
                cout << "1�� �� ���� �������� �̵��մϴ�." << endl;
                Sleep(1000);
                break;
            }
        if (button == 1) {
            break;
        }
        else if (button == 2) {
            break;
        }
    }

    if (button == 2) {
        while (1) {
            system("cls");
            cout << ">> ��ȣ�� �Է����ּ��� <<" << endl << endl;
            cout << "�� 1. �� ���� ����" << endl << endl << "�� 2. ģ��" << endl << endl << "�� 3. ä��" << endl << endl << "�� 4. ����" << endl << endl << "�� 0. ����" << endl << endl;
            cin >> num;
            cout << endl;
            switch (num) {
            case 1:
                main_My_profile();
                break;

            case 2:
                main_Friends();
                break;

            case 3:
                chatting();
                break;

            case 4:
                button = setting();
                break;

            case 0:
                system("cls");
                
                break;

            default:
                system("cls");
                cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
                cout << "1�� �� ���� �������� �̵��մϴ�." << endl;
                Sleep(1000);
                break;
            }

            if (button == 1) {
                break;
            }
        }
    }
    

    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;

    WSACleanup();
    return 0;
}

