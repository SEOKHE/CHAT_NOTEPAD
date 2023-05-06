#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
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

// MySQL Connector/C++ 초기화
sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소 (3306: MySQL 데이터베이스 기본 포트 번호)
const string username = "root"; // 데이터베이스 사용자
const string password = "1234"; // 데이터베이스 접속 비밀번호
sql::ResultSet* result; //결과값 저장을 위해 필요한 부분

SOCKET client_sock;
string npw, spw, sid, sname, sbirth, sphone; // SELECT를 의미하는 s를 변수명 앞에 붙임

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            msg = buf;
            //닉네임 : 메시지
            std::stringstream ss(msg);  // 문자열을 스트림화
            string user;
            ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당
            if (user != sname) cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록.
        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}

//채팅 내역은 서버에서 해결할것//

void login() {
    cout << "로그인을 진행합니다." << endl << endl;
    int button = 0;
    int count = 5;
    while (1) {
        cout << "id를 입력해주세요." << endl;
        cin >> sid;
        cout << endl << endl;
        pstmt = con->prepareStatement("SELECT id FROM user WHERE id = ?;");
        pstmt->setString(1, sid);
        result = pstmt->executeQuery();
        count = 5;

        if (result->next()) { //if문을 통한 조건 추가
            while (count > 0) {
                cout << "비밀 번호 5회 입력 실패시 메인 화면으로 돌아갑니다. (현재 " << count << "회 남음)" << endl << endl;
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
                cout << endl << endl;
                pstmt = con->prepareStatement("SELECT name, pw FROM user WHERE id = ? AND pw = ?;");
                pstmt->setString(1, sid);
                pstmt->setString(2, spw);
                result = pstmt->executeQuery();

                if (result->next()) { //if문을 통한 조건 추가
                    cout << "로그인이 완료되었습니다." << endl << endl;
                    sname = result->getString("name");
                    count = 5;
                    button = 1;
                    break;
                }
                else {
                    cout << "잘못된 PW입니다. 다시 입력해주세요." << endl << endl;
                    count--;
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
    cout << endl << endl;
    cout << "'-' 없이 전화번호를 입력해주세요" << endl;
    cout << ": ";
    cin >> sphone;
    cout << endl << endl;
    cout << "생일 8자리를 입력해주세요" << endl;
    cout << ": ";
    cin >> sbirth;
    cout << endl << endl;

    pstmt = con->prepareStatement("SELECT id FROM user WHERE name = ? AND phone = ? AND birth = ?;");
    pstmt->setString(1, sname);
    pstmt->setString(2, sphone);
    pstmt->setString(3, sbirth);
    result = pstmt->executeQuery();//SELECT된 결과 저장

    if (result->next()) { //if문을 통해 result안에 있는 결과들을 모두 출력시키는 출력부
        cout << endl << sname << "님의 ID는 " << result->getString(1) << " 입니다." << endl << endl;
    }
    else cout << "존재하지 않는 회원입니다." << endl << endl;
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
    cout << endl << endl;

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
            cout << endl << endl;

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

            cout << endl << endl;
            if (spw != npw) { //if문을 통한 일치여부 검사
                cout << "비밀번호가 일치하지 않습니다. 다시 입력해주세요." << endl << endl;
            }
            else break;
        }

        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?;");
        pstmt->setString(1, npw);
        pstmt->setString(2, sid);
        result = pstmt->executeQuery();

        cout << "PW 변경이 완료되었습니다." << endl << endl;
    }
    else cout << "존재하지 않는 회원입니다." << endl << endl;
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
    int c_num, button = 0;
    string instatus, insongs;
    
    while (1) {
        cout << ">> 내 정보 <<" << endl << endl;
        cout << "1. 내 정보 보기, 2. 상메 설정/수정, 3. 노래 설정/수정 0. 뒤로 가기" << endl;
        cin >> c_num;
        cout << endl << endl;

        switch (c_num) {
            case 1:
                cout << ">> 내 정보 <<" << endl << endl;
                pstmt = con->prepareStatement("SELECT status, songs, RIGHT(birth, 4) AS 'birth' FROM user WHERE id = ? AND pw = ?;");
                pstmt->setString(1, sid);
                pstmt->setString(2, spw);
                result = pstmt->executeQuery();

                while (result->next()) {
                    cout << "상태 메시지 : " << result->getString("status") << endl;
                    cout << "현재 설정 중인 노래 : " << result->getString("songs") << endl;
                    cout << "생일 : " << result->getString("birth") << endl;
                }
                cout << endl << endl;
                break;

            case 2:
                cout << ">> 상태 메시지 설정/수정 <<" << endl << endl;
                cout << "변경할 상태 메시지를 입력해주세요" << endl;
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
                cout << ">> 노래 설정/수정 <<" << endl;
                cout << "변경할 노래 제목을 입력해주세요" << endl;
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
                cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
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
        cout << ">> 친구 <<" << endl << endl;
        cout << "1. 친구 목록 보기, 2. 생일 검색 0. 뒤로 가기" << endl;
        cin >> c_num;
        cout << endl << endl;

        switch (c_num) {
        case 1:
            cout << ">> 친구 목록 <<" << endl << endl;
            pstmt = con->prepareStatement("SELECT name, status, songs, RIGHT(birth, 4) AS 'birth' FROM user WHERE id != ? AND pw != ?;");
            pstmt->setString(1, sid);
            pstmt->setString(2, spw);
            result = pstmt->executeQuery();

            while (result->next()) {
                cout << endl << "------------------------------" << endl;
                cout << "이름 : " << result->getString("name") << endl;
                cout << "상태 메시지 : " << result->getString("status") << endl;
                cout << "현재 설정 중인 노래 : " << result->getString("songs") << endl;
                cout << "생일 : " << result->getString("birth") << endl;
                cout << endl << "------------------------------" << endl;
            }
            cout << endl << endl;
            break;

        case 2:
            cout << ">> 생일 검색하기 <<" << endl << endl;
            cout << "조회를 시작할 생일 4자리를 입력해주세요. (ex. 0101)" << endl;
            cin >> inbirth1;
            cout << endl << endl;

            cout << "조회를 끝낼 생일 4자리를 입력해주세요. (ex. 0101)" << endl;
            cin >> inbirth2;

            pstmt = con->prepareStatement("SELECT name, RIGHT(birth, 4) AS 'birth' FROM user WHERE RIGHT(birth, 4) > ? AND RIGHT(birth, 4) < ?;");
            pstmt->setString(1, inbirth1);
            pstmt->setString(2, inbirth2);
            result = pstmt->executeQuery();
            cout << endl;

            if (result->next()) { //if문을 통해 result안에 있는 결과들을 모두 출력시키는 출력부
                while (1) {
                    cout << endl << "------------------------------" << endl;
                    cout << "이름 : " << result->getString("name") << endl;
                    cout << "생일 : " << result->getString("birth") << endl;
                    cout << endl << "------------------------------" << endl;
                    if (!result->next()) {
                        break;
                    }
                }
            }
            else cout << "조회하신 기간 내에 생일인 친구가 없습니다." << endl << endl;

            break;

        case 0:
            button = 1;
            break;

        default:
            cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
        }
        if (button == 1) {
            break;
        }
    }
}

void chatting() {
    cout << "채팅방에 입장합니다." << endl;
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
            const char* buffer = text.c_str(); // string형을 char* 타입으로 변환
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
        cout << endl << ">> 설정 <<" << endl << endl;
        cout << "1. 비밀번호 변경 2. 회원 탈퇴 0. 뒤로 가기" << endl;
        cin >> num;
        cout << endl << endl;

        switch (num) {
            case 1:
                cout << "현재 pw를 입력해주세요" << endl;
                cout << ": ";

                hpw = "";

                while (ch != 13) { // Enter 키를 누르면 입력 종료
                    ch = _getch();
                    if (ch == 13) break; // Enter 키를 누르면 입력 종료
                    if (ch == 8) { // Backspace 키인 경우
                        if (!hpw.empty()) { // 입력된 문자가 있으면
                            hpw.pop_back(); // 마지막 문자를 삭제
                            cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                        }
                    }
                    else {
                        hpw.push_back(ch);
                        cout << '*'; // 별표로 대체하여 출력
                    }
                }
                cout << endl << endl;

                pstmt = con->prepareStatement("SELECT pw FROM user WHERE pw = ?;");
                pstmt->setString(1, hpw);
                result = pstmt->executeQuery();

                if (result->next()) { //if문을 통한 조건 추가

                    while (1) {
                        cout << "변경할 pw를 입력해주세요" << endl;
                        cout << ": ";
                        spw = "";
                        ch = ' ';
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

                        cout << "한번 더 입력해주세요" << endl;
                        cout << ": ";


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

                        cout << endl << endl;
                        if (spw != npw) { //if문을 통한 일치여부 검사
                            cout << "비밀번호가 일치하지 않습니다. 다시 입력해주세요." << endl << endl;
                        }
                        else break;
                    }

                    pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?;");
                    pstmt->setString(1, npw);
                    pstmt->setString(2, sid);
                    result = pstmt->executeQuery();

                    cout << "PW 변경이 완료되었습니다." << endl << endl;
                }
                break;

            case 2:
                cout << "진심으로 회원탈퇴를 원하십니까?" << endl;
                cout << "1. 그렇다 2. 아니오 0. 뒤로 가기" << endl;
                cin >> c_num;
                cout << endl << endl;
                switch (c_num) {
                    case 1:
                        pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?;");
                        pstmt->setString(1, sid);
                        result = pstmt->executeQuery();

                        cout << "회원 탈퇴가 정상적으로 완료되었습니다." << endl;
                        cout << "이용해주셔서 감사했습니다." << endl;
                        button = 1;
                        break;

                    case 2:
                        cout << "회원 탈퇴가 취소되었습니다." << endl;
                        break;

                    case 0:
                        break;

                    default:
                        cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
                        break;
                    }
            case 0:
                button = 1;
                break;
            default:
                cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
                break;
            }

        if (button == 1) {
            break;
        }
    }
    
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

    int num, button = 0;

    while (1) {
        cout << "번호를 입력해주세요 1.로그인, 2. id 찾기, 3. pw 찾기, 4. 회원가입 0. 종료" << endl;
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
                cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
                break;
            }
        if (button == 1) {
            break;
        }
    }
    button = 0;

    while (1) {
        cout << "번호를 입력해주세요 1.내 정보 보기, 2. 친구, 3. 채팅 4. 설정 0. 종료" << endl;
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
                cout << "잘못된 입력입니다. 다시 입력해주세요." << endl;
                break;
            }

        if (button == 1) {
            break;
        }
   }

    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;

    WSACleanup();
    return 0;
}