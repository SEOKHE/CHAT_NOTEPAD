﻿#pragma comment(lib, "ws2_32.lib")  //명시적인 라이브러리의 링크. 윈속 라이브러리 참조

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
#define MAX_CLIENT 10

using std::cout;
using std::cin;
using std::endl;
using std::string;

struct SOCKET_INFO { // 연결된 소켓 정보에 대한 틀 생성
    SOCKET sck;
    string user;
};

std::vector<SOCKET_INFO> sck_list; // 연결된 클라이언트 소켓들을 저장할 배열 선언.
SOCKET_INFO server_sock; // 서버 소켓에 대한 정보를 저장할 변수 선언.
int client_count = 0; // 현재 접속해 있는 클라이언트를 count 할 변수 선언.

// MySQL Connector/C++ 초기화
sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌
sql::Connection* con;
sql::Statement* stmt;
sql::PreparedStatement* pstmt;

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소 (3306: MySQL 데이터베이스 기본 포트 번호)
const string username = "root"; // 데이터베이스 사용자
const string password = "1234"; // 데이터베이스 접속 비밀번호
sql::ResultSet* result; //결과값 저장을 위해 필요한 부분

void server_init(); // socket 초기화 함수. socket(), bind(), listen() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void add_client(); // 소켓에 연결을 시도하는 client를 추가(accept)하는 함수. client accept() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void send_msg(const char* msg); // send() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void recv_msg(int idx); // recv() 함수 실행됨. 자세한 내용은 함수 구현부에서 확인.
void del_client(int idx); // 소켓에 연결되어 있는 client를 제거하는 함수. closesocket() 실행됨. 자세한 내용은 함수 구현부에서 확인.

int main() {

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

    WSADATA wsa;

    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    SOCKET_INFO new_client = {};

    if (!code) {
        server_init();

        std::thread th1[MAX_CLIENT];
        for (int i = 0; i < MAX_CLIENT; i++) { // 인원 수 만큼 thread 생성해서 각각의 클라이언트가 동시에 소통할 수 있도록 함.
            th1[i] = std::thread(add_client);
        }

        
        while (1) { // 무한 반복문을 사용하여 서버가 계속해서 채팅 보낼 수 있는 상태를 만들어 줌. 반복문을 사용하지 않으면 한 번만 보낼 수 있음.
            string text, msg = "";
            int ind = 1;
            std::getline(cin, text);
            const char* buf = text.c_str();
            msg = server_sock.user + " : " + buf;
            send_msg(msg.c_str());
            pstmt = con->prepareStatement("INSERT INTO chat(name, time, chat) VALUES( ?, NOW(), ?);");
            pstmt->setString(1, server_sock.user);
            pstmt->setString(2, buf);
            result = pstmt->executeQuery();
        }

        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i].join();
            //thread 작업이 끝날 때까지 main 함수가 끝나지 않도록 해줌.
        }

        closesocket(server_sock.sck);
    }
    else {
        cout << "프로그램 종료. (Error code : " << code << ")";
    }

    // MySQL Connector/C++ 정리
    delete pstmt;
    delete con;

    WSACleanup();

    return 0;
}

void server_init() {
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN server_addr = {}; // 소켓 주소 설정 변수
    server_addr.sin_family = AF_INET; // 소켓은 Internet 타입 
    server_addr.sin_port = htons(7777); // 서버 포트 설정
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Any인 경우는 호스트를 127.0.0.1로 잡아도 되고 localhost로 잡아도 되고 양쪽 다 허용하게 할 수 있다. 그것이 INADDR_ANY이다.

    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr)); // 설정된 소켓 정보를 소켓에 바인딩한다.
    listen(server_sock.sck, SOMAXCONN); // 소켓을 대기 상태로 기다린다.

    server_sock.user = "server";

    cout << "Server On" << endl;
}

void add_client() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };

    ZeroMemory(&addr, addrsize); // addr의 메모리 영역을 0으로 초기화

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0); // Winsock2의 recv 함수. client가 보낸 닉네임을 받음.
    new_client.user = string(buf);
    
    bool reentry = false;
    int index = 0;
    for (int i = 0; i < sck_list.size(); i++) {
        if (sck_list[i].user == new_client.user) {
            reentry = true;
            index = i;
            break;
        }
    }



    string msg = "[공지] " + new_client.user + " 님이 입장했습니다.";
    cout << msg << endl;

    std::thread th;

    if (reentry) { 
        cout << "client socket " << new_client.sck;
        sck_list[index].sck = new_client.sck; 
        th = std::thread(recv_msg, index);
    }
    else {
        sck_list.push_back(new_client); // client 정보를 담는 sck_list 배열에 새로운 client 추가
        th = std::thread(recv_msg, client_count);  // 다른 사람들로부터 오는 메시지를 계속해서 받을 수 있는 상태로 만들어 두기
    }

    client_count++; // client 수 증가.
    cout << "[공지] 현재 접속자 수 : " << client_count << "명" << endl;
    send_msg(msg.c_str()); // c_str : string 타입을 const chqr* 타입으로 바꿔줌.

    th.join();
}

void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) { // 접속해 있는 모든 client에게 메시지 전송
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}

void recv_msg(int idx) {
    char buf[MAX_SIZE] = { };
    string msg = "";

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) { // 오류가 발생하지 않으면 recv는 수신된 바이트 수를 반환. 0보다 크다는 것은 메시지가 왔다는 것.

            if (string(buf) != "/quit") {
                msg = sck_list[idx].user + " : " + buf;
                cout << "  " << msg << endl;
                send_msg(msg.c_str());

                pstmt = con->prepareStatement("INSERT INTO chat(name, time, chat) VALUES( ?, NOW(), ?);");
                pstmt->setString(1, sck_list[idx].user);
                pstmt->setString(2, buf);
                result = pstmt->executeQuery();
            }
            
        }
        else { //그렇지 않을 경우 퇴장에 대한 신호로 생각하여 퇴장 메시지 전송
            msg = "[공지] " + sck_list[idx].user + " 님이 퇴장했습니다.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx); // 클라이언트 삭제
            return;
        }
    }
}

void del_client(int idx) {
    std::thread th(add_client);
    closesocket(sck_list[idx].sck);
    client_count--;
    th.join();
}