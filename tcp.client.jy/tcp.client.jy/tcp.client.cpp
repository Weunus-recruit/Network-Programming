#define _WINSOCK_DEPRECATED_NO_WARNINGS									// winsock 헤더 취약점 경고 방지
#define _CRT_SECURE_NO_WARNINGS											// 'scanf', 'strcpy' 취약점 경고 방지

#include "pch.h"													// 프리컴파일 헤더
#include <iostream>													// c++ 표준 입출력 헤더
#include <WinSock2.h>												// 소켓 통신을 위한 헤더
#include <thread>													// 멀티스레딩 구현을 위한 헤더

// proc1은 Client 파일에서 선언. 서버가 보낸 send를 recv하도록 설정
// proc2는 Server 파일에서 선언. 클라이언트가 보낸 send를 recv하도록 설정

#pragma comment(lib, "ws2_32")											// winsock2 헤더 속 ws2_32.DLL 라이브러리 링크 선언
#pragma warning(disable : 4996)											//inet_addr API 취약점 경고 방지 



using namespace std;

SOCKET skt;											// 클라이언트 소켓 선언

#define PACKET_SIZE	1024							// 패킷 사이즈 임의 선언 (TCP 최대값은 65,535 byte)

void proc_recv() {									// 클라이언트에서 recv받는 메세지를 처리하는 프로시저를 스레드에 지정해주기 위한 함수
	char buffer[PACKET_SIZE] = {};					// 서버쪽에서 보내는 buffer와 buffer의 사이즈를 지정
	char QuitMsg[] = "quit";						// "quit"라는 대화문을 받을 시 종료

	while (!WSAGetLastError()) {									// Winsock2.h 소켓 오류가 발생시까지 실행시키는 무한 반복문
		ZeroMemory(&buffer, PACKET_SIZE);							// buffer 메모리 초기화
		recv(skt, buffer, PACKET_SIZE, 0);							// Connect 된 서버에서 보내는 buffer를 skt 소켓을 통해 넘겨받기 위한 recv 함수
		cout << "Server에게 받은 메세지: " << buffer << endl;
		if (strcmp(QuitMsg, buffer) == 0)							// 서버에서 보낸 buffer와 QuitMsg(="quit")가 일치하면 소켓 및 
		{
			cout << "Quit Message Detected" << endl;
			cout << "Server Down" << endl;
			cout << "Close Socket" << endl;
			closesocket(skt);						// 소켓 종료
			exit;									// 스레드 종료
			WSACleanup();							// ws2_32.DLL 사용 종료 함수
			break;

		}
	}
}



int main(void)
{

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);											// winsock2 즉, 2버전 winsock 헤더 초기화를 선언

	skt = socket(AF_INET, SOCK_STREAM, 0);
	// AF_INET - IPv4 주소 패밀리를 허용. 프로토콜 허용을 위한 PF_INET도 있으나 두 값 모두 정수형 2값으로 변환되며 같은 용도로 사용.
	// SOCK_STREAM - TCP 패킷을 받겠다는 의미

	SOCKADDR_IN addr = {};									// 서버 소켓 address에 connect를 보내기 위해 선언
	addr.sin_family = AF_INET;								// IPv4
	addr.sin_port = htons(8080);							// 8080번 포트 사용.
	/*
	잘 알려진 포트(well-known port) : 0번 ~ 1023번   => IANA(Internet Assigned Numbers Authority)에서 할당된 TCP 및 UDP 특정 쓰임새를 위한 포트. (사용이 되고있는)
	등록된 포트(registered port) : 1024번 ~ 49151번  => well-known port가 아닌 다른 프로그램이 사용하는 포트
	동적 포트(dynamic port) : 49152번 ~ 65535번      => 웹 서버가 클라이언트에게 Response를 보내는 것에 사용되는 포트. 출발지 포트는 웹 서버 포트 80(HTTP)를 사용
																													   목적지 포트는 dynamic port(ex. 50000)
	*/

	addr.sin_addr.s_addr = inet_addr("192.168.0.162");									// 서버 Host IP Address

	while (1) {
		if (!connect(skt, (SOCKADDR*)&addr, sizeof(addr))) break;						// 서버에 보낸 connect가 accept 될때까지 connect 요청
	}
	thread proc1(proc_recv);															// proc1을 처리하는 스레드에서 proc_recv를 작동시키기 위해 선언
	char msg[PACKET_SIZE] = { 0 };

	cout << "Server Connected" << endl;													// connect가 accept 됬으므로 서버와 커넥트 되었음을 출력

	char ClientID[100] = {};															// 서버에 먼저 ID를 송신
	cout << "클라이언트 ID : ";
	cin >> ClientID;
	send(skt, ClientID, strlen(ClientID), 0);

	while (!WSAGetLastError()) {

		cin >> msg;

		if (strcmp(msg, "quit") == 0)													// 클라이언트가 보내는 메시지가 "quit"일때도 프로그램을 중지하기 위해 사용
		{
			send(skt, msg, strlen(msg), 0);
			cout << "quit Access" << endl;

			return 0;
		}

		send(skt, msg, strlen(msg), 0);													// 메시지가 "quit"가 아닐 경우 종료없이 send
	}

	proc1.join();												// proc1 스레드가 종료될때까지 대기 후 종료되었으면 다음 코드를 실행
	closesocket(skt);											// 소켓 종료
	WSACleanup();												// ws2_32.DLL 사용 종료 

}