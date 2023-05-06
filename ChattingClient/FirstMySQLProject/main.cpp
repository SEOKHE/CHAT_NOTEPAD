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

//ä�� ������ �������� �ذ��Ұ�//

void login() {
    cout << "�α����� �����մϴ�." << endl << endl;
    int button = 0;
    int count = 5;
    while (1) {
        cout << "id�� �Է����ּ���." << endl;
        cin >> sid;
        cout << endl << endl;
        pstmt = con->prepareStatement("SELECT id FROM user WHERE id = ?;");
        pstmt->setString(1, sid);
        result = pstmt->executeQuery();
        count = 5;

        if (result->next()) { //if���� ���� ���� �߰�
            while (count > 0) {
                cout << "��� ��ȣ 5ȸ �Է� ���н� ���� ȭ������ ���ư��ϴ�. (���� " << count << "ȸ ����)" << endl << endl;
                cout << "pw�� �Է����ּ���." << endl;
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
                    cout << "�α����� �Ϸ�Ǿ����ϴ�." << endl << endl;
                    sname = result->getString("name");
                    count = 5;
                    button = 1;
                    break;
                }
                else {
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
        }
        if (button == 1) {
            break;
        }
    }
}

void id_search() {

    cout << "�̸��� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sname;
    cout << endl << endl;
    cout << "'-' ���� ��ȭ��ȣ�� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sphone;
    cout << endl << endl;
    cout << "���� 8�ڸ��� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sbirth;
    cout << endl << endl;

    pstmt = con->prepareStatement("SELECT id FROM user WHERE name = ? AND phone = ? AND birth = ?;");
    pstmt->setString(1, sname);
    pstmt->setString(2, sphone);
    pstmt->setString(3, sbirth);
    result = pstmt->executeQuery();//SELECT�� ��� ����

    if (result->next()) { //if���� ���� result�ȿ� �ִ� ������� ��� ��½�Ű�� ��º�
        cout << endl << sname << "���� ID�� " << result->getString(1) << " �Դϴ�." << endl << endl;
    }
    else cout << "�������� �ʴ� ȸ���Դϴ�." << endl << endl;
}

void pw_search() {
    cout << "ID�� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sid;
    cout << endl << endl;
    cout << "�̸��� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sname;
    cout << endl << endl;
    cout << "'-' ���� ��ȭ��ȣ�� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sphone;
    cout << endl << endl;
    cout << "���� 8�ڸ��� �Է����ּ���" << endl;
    cout << ": ";
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
            cout << "������ pw�� �Է����ּ���" << endl;
            cout << ": ";
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

            cout << "�ѹ� �� �Է����ּ���" << endl;
            cout << ": ";

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
            }
            else break;
        }

        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?;");
        pstmt->setString(1, npw);
        pstmt->setString(2, sid);
        result = pstmt->executeQuery();

        cout << "PW ������ �Ϸ�Ǿ����ϴ�." << endl << endl;
    }
    else cout << "�������� �ʴ� ȸ���Դϴ�." << endl << endl;
}

void sign_up() {
    cout << "ȸ�� ������ �����ϰڽ��ϴ�." << endl;
    //�̸�, ����, ��ȭ��ȣ, ���̵�, ��� ������ �Է¹ޱ�
    //INSERT INTO ����Ұ�.

    cout << endl << "�̸��� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sname;
    cout << endl << endl;
    cout << "���� 8�ڸ��� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sbirth;
    cout << endl << endl;
    cout << "'-' ���� ��ȭ��ȣ�� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sphone;
    cout << endl << endl;
    cout << "ID�� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sid;
    cout << endl << endl;
    cout << "PW�� �Է����ּ���" << endl;
    cout << ": ";
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

        cout << "ȸ�� ������ �Ϸ�Ǿ����ϴ�." << endl;
        cout << sid << "�� ȯ���մϴ�!" << endl;
    }
}

void main_My_profile() {
    int c_num, button = 0;
    string instatus, insongs;
    
    while (1) {
        cout << ">> �� ���� <<" << endl << endl;
        cout << "1. �� ���� ����, 2. ��� ����/����, 3. �뷡 ����/���� 0. �ڷ� ����" << endl;
        cin >> c_num;
        cout << endl << endl;

        switch (c_num) {
            case 1:
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
                break;

            case 2:
                cout << ">> ���� �޽��� ����/���� <<" << endl << endl;
                cout << "������ ���� �޽����� �Է����ּ���" << endl;
                cin >> instatus;
                cout << endl << endl;

                pstmt = con->prepareStatement("UPDATE user SET status = ? WHERE id = ? AND pw = ?;");
                pstmt->setString(1, instatus);
                pstmt->setString(2, sid);
                pstmt->setString(3, spw);
                result = pstmt->executeQuery();
                cout << endl << endl;
                break;

            case 3:
                cout << ">> �뷡 ����/���� <<" << endl;
                cout << "������ �뷡 ������ �Է����ּ���" << endl;
                cin >> insongs;
                cout << endl << endl;

                pstmt = con->prepareStatement("UPDATE user SET songs = ? WHERE id = ? AND pw = ?;");
                pstmt->setString(1, insongs);
                pstmt->setString(2, sid);
                pstmt->setString(3, spw);
                result = pstmt->executeQuery();
                cout << endl << endl;
                break;

            case 0:
                button = 1;
                break;

            default:
                cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
        }
        if (button == 1) {
            break;
        }
    }

}

void main_Friends() {
    int c_num, button = 0;
    string instatus, insongs, inbirth1, inbirth2;

    while (1) {
        cout << ">> ģ�� <<" << endl << endl;
        cout << "1. ģ�� ��� ����, 2. ���� �˻� 0. �ڷ� ����" << endl;
        cin >> c_num;
        cout << endl << endl;

        switch (c_num) {
        case 1:
            cout << ">> ģ�� ��� <<" << endl << endl;
            pstmt = con->prepareStatement("SELECT name, status, songs, RIGHT(birth, 4) AS 'birth' FROM user WHERE id != ? AND pw != ?;");
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
            break;

        case 2:
            cout << ">> ���� �˻��ϱ� <<" << endl << endl;
            cout << "��ȸ�� ������ ���� 4�ڸ��� �Է����ּ���. (ex. 0101)" << endl;
            cin >> inbirth1;
            cout << endl << endl;

            cout << "��ȸ�� ���� ���� 4�ڸ��� �Է����ּ���. (ex. 0101)" << endl;
            cin >> inbirth2;

            pstmt = con->prepareStatement("SELECT name, RIGHT(birth, 4) AS 'birth' FROM user WHERE RIGHT(birth, 4) > ? AND RIGHT(birth, 4) < ?;");
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
            else cout << "��ȸ�Ͻ� �Ⱓ ���� ������ ģ���� �����ϴ�." << endl << endl;

            break;

        case 0:
            button = 1;
            break;

        default:
            cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
        }
        if (button == 1) {
            break;
        }
    }
}

void chatting() {
    cout << "ä�ù濡 �����մϴ�." << endl;
    WSADATA wsa;

    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {

        client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 

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

        while (1) {
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string���� char* Ÿ������ ��ȯ
            send(client_sock, buffer, strlen(buffer), 0);
            if (buffer == "/exit") {
                break;
            }
        }
        th2.join();
        closesocket(client_sock);
    }
}

void setting() {
    int num, c_num, button = 0;
    string hpw;
    char ch = ' ';
    npw = "";
    char ch2 = ' ';
    while (1) {
        cout << endl << ">> ���� <<" << endl << endl;
        cout << "1. ��й�ȣ ���� 2. ȸ�� Ż�� 0. �ڷ� ����" << endl;
        cin >> num;
        cout << endl << endl;

        switch (num) {
            case 1:
                cout << "���� pw�� �Է����ּ���" << endl;
                cout << ": ";

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
                        cout << "������ pw�� �Է����ּ���" << endl;
                        cout << ": ";
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

                        cout << "�ѹ� �� �Է����ּ���" << endl;
                        cout << ": ";


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
                        }
                        else break;
                    }

                    pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?;");
                    pstmt->setString(1, npw);
                    pstmt->setString(2, sid);
                    result = pstmt->executeQuery();

                    cout << "PW ������ �Ϸ�Ǿ����ϴ�." << endl << endl;
                }
                break;

            case 2:
                cout << "�������� ȸ��Ż�� ���Ͻʴϱ�?" << endl;
                cout << "1. �׷��� 2. �ƴϿ� 0. �ڷ� ����" << endl;
                cin >> c_num;
                cout << endl << endl;
                switch (c_num) {
                    case 1:
                        pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?;");
                        pstmt->setString(1, sid);
                        result = pstmt->executeQuery();

                        cout << "ȸ�� Ż�� ���������� �Ϸ�Ǿ����ϴ�." << endl;
                        cout << "�̿����ּż� �����߽��ϴ�." << endl;
                        button = 1;
                        break;

                    case 2:
                        cout << "ȸ�� Ż�� ��ҵǾ����ϴ�." << endl;
                        break;

                    case 0:
                        break;

                    default:
                        cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
                        break;
                    }
            case 0:
                button = 1;
                break;
            default:
                cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
                break;
            }

        if (button == 1) {
            break;
        }
    }
    
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
        cout << "��ȣ�� �Է����ּ��� 1.�α���, 2. id ã��, 3. pw ã��, 4. ȸ������ 0. ����" << endl;
        cin >> num;
        switch (num) {
            case 1:
                login();
                button = 1;
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
                break;
            }
        if (button == 1) {
            break;
        }
    }
    button = 0;

    while (1) {
        cout << "��ȣ�� �Է����ּ��� 1.�� ���� ����, 2. ģ��, 3. ä�� 4. ���� 0. ����" << endl;
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
                setting();
                break;

            case 0:
                button = 1;
                break;

            default:
                cout << "�߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
                break;
            }

        if (button == 1) {
            break;
        }
   }

    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;

    WSACleanup();
    return 0;
}