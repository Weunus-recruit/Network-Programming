#define _WINSOCK_DEPRECATED_NO_WARNINGS								// winsock 헤더 취약점 경고 방지
#define _CRT_SECURE_NO_WARNINGS										// 'scanf', 'strcpy' 취약점 경고 방지

#include "pch.h"													// 프리컴파일 헤더
#include <iostream>													// c++ 표준 입출력 헤더
#include <WinSock2.h>												// 소켓 통신을 위한 헤더
#include <thread>													// 멀티스레딩 구현을 위한 헤더

#pragma comment(lib, "ws2_32")										// winsock2.h에 ws2_32.DLL 라이브러리 링크 선언

#define PACKET_SIZE	1024											// 패킷 사이즈 임의 선언

using namespace std;

SOCKET skt, client_sock;											// 서버 프로그램에서 사용되는 소켓과 클라이언트 요청을 기다리다가 connect 요청이 오면 연결을 맺기
																	// 위한 소켓 두개를 선언

void proc_recvs() {													// 서버에서 recv되는 메시지를 처리하는 프로시저를 스레드에 지정해주기 위한 함수
	char buffer[PACKET_SIZE] = { 0 };								// 클라이언트가 보내는 buffer와 사이즈
	char ClientID[PACKET_SIZE] = {};								// 클라이언트가 가장 먼저 ID를 보내기 때문에 ID를 지정하기 위해 선언
	ZeroMemory(&ClientID, PACKET_SIZE);								// ID 메모리 초기화
	recv(client_sock, ClientID, PACKET_SIZE, 0);					// 클라이언트 소켓을 통해 넘어오는 ClientID를 먼저 받기 위해 recv 선언(대기)
	char QuitMsg[] = "quit";										// 클라이언트 메시지가 quit일 경우 종료하기 위해 선언
	while (!WSAGetLastError()) {									// Winsock2.h 소켓 오류가 발생시까지 실행시키는 무한 반복문
		ZeroMemory(&buffer, PACKET_SIZE);							// 버퍼 메모리 초기화
		recv(client_sock, buffer, PACKET_SIZE, 0);					// Accept 한 클라이언트가 send한 메시지를 recv
		cout << ClientID << "에게 받은 메시지: " << buffer << endl;
		if (strcmp(QuitMsg, buffer) == 0)							// 클라이언트가 보낸 buffer가 QuitMsg(="quit")가 일치하면 프로그램 종료
		{
			cout << "Client Left" << endl;
			cout << "Close Socket" << endl;

			closesocket(client_sock);		// 클라이언트 소켓 종료
			closesocket(skt);				// 서버 소켓 종료
			exit;							// 스레드 종료
			WSACleanup();					// ws2_32.DLL 사용 종료 함수
			break;

		}
	}
}

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);											// winsock2 즉, 2버전 winsock 헤더 초기화를 선언

	/*
	typedef struct WSAData {
		WORD                wVersion;											// 버전
		WORD                wHighVersion;										//사용할 수 있는 상위 버전으로 wVersion과 일치한다.
		char                   szDescription[WSADESCRIPTION_LEN + 1];	//윈속 설명
		char                   szSystemStatus[WSASYS_STATUS_LEN + 1];	//상태 문자열
		unsigned short     iMaxSockets;										//최대 소켓
		unsigned short     iMaxUdpDg;										//데이터 그램의 최대 크기
		char FAR* lpVendorInfo;											//벤더 정보(공급자 정보)
	*/

	skt = socket(AF_INET, SOCK_STREAM, 0);										// 서버 소켓 선언

	SOCKADDR_IN addr = {};
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;													// 서버 소켓에서 IPv4 주소 패밀리를 허용.
	addr.sin_port = htons(8080);												// 8080번 포트 사용.
	addr.sin_addr.s_addr = INADDR_ANY;											// 서버 프로그램에서 컴퓨터에 존재하는 사용가능한 랜카드의 IP주소를 사용하라는 의미.

	bind(skt, (SOCKADDR*)&addr, sizeof(addr));									// 위에서 설정한 서버가 사용할 IP와 포트 번호를 소켓에 bind(결합) 시킴
	listen(skt, SOMAXCONN);														// 클라이언트의 connect 요청이 올때까지 listen 상태로 대기.
	// SOMAXCONN => Socket Max Connection의 약자로 정수값 5로 작용.

	SOCKADDR_IN client = {};													// listen 상태에서 connect 요청이 인식되면 client의 소켓을 받기 위해 설정
	int client_size = sizeof(client);
	ZeroMemory(&client, client_size);
	client_sock = accept(skt, (SOCKADDR*)&client, &client_size);				// 클라이언트 소켓이 서버 소켓과 통신하는 것을 accept하면서 클라이언트 소켓 정보를 저장 

	if (client_sock == INVALID_SOCKET)											// 클라이언트 소켓이 작동이 불가능하면 에러 메시지 출력
		cout << "accept error" << endl;

	thread proc2(proc_recvs);													// proc2를 처리하는 스레드에서 proc_recvs가 작동하기 위해 선언

	cout << "Client Connected" << endl;											// connect가 accept 되었으므로 클라이언트와 커넥트 되었음을 출력.

	char buffer[PACKET_SIZE] = { 0 };											// 서버가 보내는 buffer를 설정하기 위해 선언

	while (!WSAGetLastError()) {
		cin >> buffer;															// 서버 버퍼(=메시지) 입력

		if (strcmp(buffer, "quit") == 0)										// 서버 메시지가 "quit"일 경우 클라이언트에 메시지를 전송한 후 프로그램 종료
		{
			send(client_sock, buffer, strlen(buffer), 0);
			cout << "quit Access" << endl;

			return 0;
		}

		send(client_sock, buffer, strlen(buffer), 0);							// 메시지가 "quit"가 아닌 경우엔 종료없이 메시지를 전송
	}


	proc2.join();																// proc2 스레드가 종료될 때 까지 대기 후 종료되었으면 다음 코드를 실행

	closesocket(client_sock);													// 클라이언트 소켓 종료
	closesocket(skt);															// 서버 소켓 종료
	WSACleanup();																// ws2_32.DLL 사용 종료
}