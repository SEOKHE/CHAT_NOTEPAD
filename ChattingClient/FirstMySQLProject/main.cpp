#include <iostream>
#include <string>
#include <mysql/jdbc.h>
#include <conio.h>

using std::cout;
using std::cin;
using std::endl;
using std::string;

// MySQL Connector/C++ 초기화
sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소 (3306: MySQL 데이터베이스 기본 포트 번호)
const string username = "root"; // 데이터베이스 사용자
const string password = "1234"; // 데이터베이스 접속 비밀번호
sql::ResultSet* result; //결과값 저장을 위해 필요한 부분

string npw, spw, sid, sname, sbirth, sphone; // SELECT를 의미하는 s를 변수명 앞에 붙임

void login() {
    cout << "로그인을 진행합니다." << endl;
    int button = 0;
    while (1) {
        cout << "id를 입력해주세요." << endl;
        cin >> sid;
        cout << endl;
        pstmt = con->prepareStatement("SELECT id FROM user WHERE id = ?;");
        pstmt->setString(1, sid);
        result = pstmt->executeQuery();

        if (result->next()) { //if문을 통한 조건 추가
            while (1) {
                cout << "pw를 입력해주세요." << endl;
                spw = "";
                char ch = ' ';
                while (ch != 13) { // Enter 키를 누르면 입력 종료
                    ch = _getch();
                    if (ch == 13) break; // Enter 키를 누르면 입력 종료
                    if (ch == 8) { // Backspace 키인 경우
                        if (!spw.empty()) { // 입력된 문자가 있으면
                            spw.pop_back(); // 마지막 문자를 삭제
                            cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                        }
                    }
                    else {
                        spw.push_back(ch);
                        cout << '*'; // 별표로 대체하여 출력
                    }
                }
                cout << endl;
                pstmt = con->prepareStatement("SELECT pw FROM user WHERE pw = ?;");
                pstmt->setString(1, spw);
                result = pstmt->executeQuery();

                if (result->next()) { //if문을 통한 조건 추가
                    cout << "로그인이 완료되었습니다." << endl;
                    button = 1;
                    break;
                }
                else {
                    cout << "존재하지 않는 PW입니다. 다시 입력해주세요." << endl << endl;
                }
                if (button == 1) {
                    break;
                }
            }
        }
        else {
            cout << "존재하지 않는 ID입니다. 다시 입력해주세요." << endl << endl;
        }
        if (button == 1) {
            break;
        }
    }
}

void id_search() {

    cout << "이름을 입력해주세요" << endl;
    cout << ": ";
    cin >> sname;
    cout << endl;
    cout << "'-' 없이 전화번호를 입력해주세요" << endl;
    cout << ": ";
    cin >> sphone;
    cout << endl;
    cout << "생일 8자리를 입력해주세요" << endl;
    cout << ": ";
    cin >> sbirth;
    cout << endl;


    pstmt = con->prepareStatement("SELECT id FROM user WHERE name = ? AND phone = ? AND birth = ?;");
    pstmt->setString(1, sname);
    pstmt->setString(2, sphone);
    pstmt->setString(3, sbirth);
    result = pstmt->executeQuery();//SELECT된 결과 저장

    if (result->next()) { //if문을 통해 result안에 있는 결과들을 모두 출력시키는 출력부
        cout << endl << sname << "님의 ID는 " << result->getString(1) << " 입니다." << endl << endl;
    }
    else cout << "존재하지 않는 회원입니다." << endl;

}

void pw_search() {
    cout << "ID를 입력해주세요" << endl;
    cout << ": ";
    cin >> sid;
    cout << endl << endl;
    cout << "이름을 입력해주세요" << endl;
    cout << ": ";
    cin >> sname;
    cout << endl << endl;
    cout << "'-' 없이 전화번호를 입력해주세요" << endl;
    cout << ": ";
    cin >> sphone;
    cout << endl << endl;
    cout << "생일 8자리를 입력해주세요" << endl;
    cout << ": ";
    cin >> sbirth;
    cout << endl;

    pstmt = con->prepareStatement("SELECT pw FROM user WHERE id = ? AND name = ? AND birth = ? AND phone = ?;");
    pstmt->setString(1, sid);
    pstmt->setString(2, sname);
    pstmt->setString(3, sbirth);
    pstmt->setString(4, sphone);
    result = pstmt->executeQuery();

    if (result->next()) { //if문을 통한 조건 추가

        while (1) {
            cout << "변경할 pw를 입력해주세요" << endl;
            cout << ": ";
            spw = "";
            char ch = ' ';
            while (ch != 13) { // Enter 키를 누르면 입력 종료
                ch = _getch();
                if (ch == 13) break; // Enter 키를 누르면 입력 종료
                if (ch == 8) { // Backspace 키인 경우
                    if (!spw.empty()) { // 입력된 문자가 있으면
                        spw.pop_back(); // 마지막 문자를 삭제
                        cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                    }
                }
                else {
                    spw.push_back(ch);
                    cout << '*'; // 별표로 대체하여 출력
                }
            }
            cout << endl;

            cout << "한번 더 입력해주세요" << endl;
            cout << ": ";

            npw = "";
            char ch2 = ' ';
            while (ch2 != 13) { // Enter 키를 누르면 입력 종료
                ch2 = _getch();
                if (ch2 == 13) break; // Enter 키를 누르면 입력 종료
                if (ch2 == 8) { // Backspace 키인 경우
                    if (!npw.empty()) { // 입력된 문자가 있으면
                        npw.pop_back(); // 마지막 문자를 삭제, 혹시 추후에 배열에 값이 존재해서 문제가 발생할 겨우 clear()를 사용해서 해결할 수 있음
                        cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                    }
                }
                else {
                    npw.push_back(ch2);
                    cout << '*'; // 별표로 대체하여 출력
                }
            }

            cout << endl;
            if (spw != npw) { //if문을 통한 일치여부 검사
                cout << "비밀번호가 일치하지 않습니다. 다시 입력해주세요." << endl << endl;
            }
            else break;
        }

        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?;");
        pstmt->setString(1, npw);
        pstmt->setString(2, sid);
        result = pstmt->executeQuery();

        cout << "PW 변경이 완료되었습니다." << endl;
    }
    else cout << "존재하지 않는 회원입니다." << endl;
}

void sign_up() {
    cout << "회원 가입을 진행하겠습니다." << endl;
    //이름, 생일, 전화번호, 아이디, 비번 순으로 입력받기
    //INSERT INTO 사용할것.

    cout << endl << "이름을 입력해주세요" << endl;
    cout << ": ";
    cin >> sname;
    cout << endl << endl;
    cout << "생일 8자리를 입력해주세요" << endl;
    cout << ": ";
    cin >> sbirth;
    cout << endl << endl;
    cout << "'-' 없이 전화번호를 입력해주세요" << endl;
    cout << ": ";
    cin >> sphone;
    cout << endl << endl;
    cout << "ID를 입력해주세요" << endl;
    cout << ": ";
    cin >> sid;
    cout << endl << endl;
    cout << "PW를 입력해주세요" << endl;
    cout << ": ";
    spw = "";
    char ch = ' ';
    while (ch != 13) { // Enter 키를 누르면 입력 종료
        ch = _getch();
        if (ch == 13) break; // Enter 키를 누르면 입력 종료
        if (ch == 8) { // Backspace 키인 경우
            if (!spw.empty()) { // 입력된 문자가 있으면
                spw.pop_back(); // 마지막 문자를 삭제
                cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
            }
        }
        else {
            spw.push_back(ch);
            cout << '*'; // 별표로 대체하여 출력
        }
    }
    cout << endl << endl;
    
    pstmt = con->prepareStatement("SELECT name, birth, phone, id, pw FROM user WHERE name = ? OR phone = ? OR id = ?");
    pstmt->setString(1, sname);
    pstmt->setString(2, sphone);
    pstmt->setString(3, sid);
    result = pstmt->executeQuery();

    if (result->next()) { //if문을 통한 조건 추가
        cout << "이미 존재하는 회원입니다." << endl;
    }
    else {
        pstmt = con->prepareStatement("INSERT INTO user(name, birth, phone, id, pw) VALUES(?, ?, ?, ?, ?);");
        pstmt->setString(1, sname);
        pstmt->setString(2, sbirth);
        pstmt->setString(3, sphone);
        pstmt->setString(4, sid);
        pstmt->setString(5, spw);
        result = pstmt->executeQuery();

        cout << "회원 가입이 완료되었습니다." << endl;
        cout << sid << "님 환영합니다!" << endl;
    }
}

void main_My_profile() {
    cout << "내 프로필 ";
}

int main()
{
    // MySQL 연결
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(server, username, password); // MySQL 데이터베이스 연결 객체
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        exit(1);
    }

    // 데이터베이스 선택
    con->setSchema("chatdb");

    // db 한글 저장을 위한 셋팅 
    stmt = con->createStatement();
    stmt->execute("set names euckr");
    if (stmt) { delete stmt; stmt = nullptr; }

    login();
    id_search();
    pw_search();
    sign_up();

    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;

    return 0;
}