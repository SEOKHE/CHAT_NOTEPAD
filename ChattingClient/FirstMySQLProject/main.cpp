#include <iostream>
#include <string>
#include <mysql/jdbc.h>
#include <conio.h>

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

string npw, spw, sid, sname, sbirth, sphone; // SELECT�� �ǹ��ϴ� s�� ������ �տ� ����

void login() {
    cout << "�α����� �����մϴ�." << endl;
    int button = 0;
    while (1) {
        cout << "id�� �Է����ּ���." << endl;
        cin >> sid;
        cout << endl;
        pstmt = con->prepareStatement("SELECT id FROM user WHERE id = ?;");
        pstmt->setString(1, sid);
        result = pstmt->executeQuery();

        if (result->next()) { //if���� ���� ���� �߰�
            while (1) {
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
                cout << endl;
                pstmt = con->prepareStatement("SELECT pw FROM user WHERE pw = ?;");
                pstmt->setString(1, spw);
                result = pstmt->executeQuery();

                if (result->next()) { //if���� ���� ���� �߰�
                    cout << "�α����� �Ϸ�Ǿ����ϴ�." << endl;
                    button = 1;
                    break;
                }
                else {
                    cout << "�������� �ʴ� PW�Դϴ�. �ٽ� �Է����ּ���." << endl << endl;
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
    cout << endl;
    cout << "'-' ���� ��ȭ��ȣ�� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sphone;
    cout << endl;
    cout << "���� 8�ڸ��� �Է����ּ���" << endl;
    cout << ": ";
    cin >> sbirth;
    cout << endl;


    pstmt = con->prepareStatement("SELECT id FROM user WHERE name = ? AND phone = ? AND birth = ?;");
    pstmt->setString(1, sname);
    pstmt->setString(2, sphone);
    pstmt->setString(3, sbirth);
    result = pstmt->executeQuery();//SELECT�� ��� ����

    if (result->next()) { //if���� ���� result�ȿ� �ִ� ������� ��� ��½�Ű�� ��º�
        cout << endl << sname << "���� ID�� " << result->getString(1) << " �Դϴ�." << endl << endl;
    }
    else cout << "�������� �ʴ� ȸ���Դϴ�." << endl;

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
    cout << endl;

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
            cout << endl;

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

            cout << endl;
            if (spw != npw) { //if���� ���� ��ġ���� �˻�
                cout << "��й�ȣ�� ��ġ���� �ʽ��ϴ�. �ٽ� �Է����ּ���." << endl << endl;
            }
            else break;
        }

        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?;");
        pstmt->setString(1, npw);
        pstmt->setString(2, sid);
        result = pstmt->executeQuery();

        cout << "PW ������ �Ϸ�Ǿ����ϴ�." << endl;
    }
    else cout << "�������� �ʴ� ȸ���Դϴ�." << endl;
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
    cout << "�� ������ ";
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

    login();
    id_search();
    pw_search();
    sign_up();

    // MySQL Connector/C++ ����
    delete pstmt;
    delete con;

    return 0;
}