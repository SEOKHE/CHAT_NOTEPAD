#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h>
#include <string>
#include <iostream>
#include <thread>
#include <vector>

#define MAX_SIZE 1024
#define MAX_CLIENT 3

using std::cout;
using std::cin;
using std::endl;
using std::string;

struct SOCKET_INFO {
    SOCKET sck;
    string user;
};

std::vector<SOCKET_INFO> sck_list;
SOCKET_INFO server_sock;
int client_count = 0;

void server_init();
void add_client();
void send_msg(const char* msg);
void recv_msg(int idx);
void del_client(int idx);

int main() {
    WSADATA wsa;

    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {
        server_init();

        std::thread th1[MAX_CLIENT];
        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i] = std::thread(add_client);
        }

        while (1) {
            string text, msg = "";

            std::getline(cin, text);
            const char* buf = text.c_str();
            msg = server_sock.user + " : " + buf;
            send_msg(msg.c_str());
        }

        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i].join();
        }

        closesocket(server_sock.sck);
    }
    else {
        cout << "프로그램 종료. (Error code : " << code << ")";
    }

    WSACleanup();

    return 0;
}

void server_init() {
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));
    listen(server_sock.sck, SOMAXCONN);

    server_sock.user = "server";

    cout << "Server On" << endl;
}

void add_client() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };

    ZeroMemory(&addr, addrsize);

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0);
    new_client.user = string(buf);

    string msg = "[공지] " + new_client.user + " 님이 입장했습니다.";
    cout << msg << endl;
    sck_list.push_back(new_client);

    std::thread th(recv_msg, client_count);

    client_count++;
    cout << "[공지] 현재 접속자 수 : " << client_count << "명" << endl;
    send_msg(msg.c_str());

    th.join();
}

void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) {
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}

void recv_msg(int idx) {
    char buf[MAX_SIZE] = { };
    string msg = "";

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
            msg = sck_list[idx].user + " : " + buf;
            cout << msg << endl;
            send_msg(msg.c_str());
        }
        else {
            msg = "[공지] " + sck_list[idx].user + " 님이 퇴장했습니다.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx);
            return;
        }
    }
}

void del_client(int idx) {
    closesocket(sck_list[idx].sck);
    client_count--;
}