#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <Windows.h>
#include <thread>


#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)



using namespace std;

SOCKET skt;

/*

IN_ADDR GetDefaultMyIP();

IN_ADDR GetDefaultMyIP()
{
	char localhostname[MAX_PATH];
	IN_ADDR clientIP = { 0, };

	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR)//호스트 이름 얻어오기
	{
		return clientIP;
	}
	HOSTENT* ptr = gethostbyname(localhostname);//호스트 엔트리 얻어오기
	while (ptr && ptr->h_name)
	{
		if (ptr->h_addrtype == PF_INET)//IPv4 주소 타입일 때
		{
			memcpy(&clientIP, ptr->h_addr_list[0], ptr->h_length);//메모리 복사
			break;//반복문 탈출
		}
		ptr++;
	}
	
	return clientIP;
}
*/

#define PACKET_SIZE	1024

void proc_recv() {
	char buffer[PACKET_SIZE] = {};
	char QuitMsg[] = "quit";
	
	string cmd;
	while (!WSAGetLastError()) {
		ZeroMemory(&buffer, PACKET_SIZE);
		recv(skt, buffer, PACKET_SIZE, 0);
		cmd = buffer;
		cout << "Server에게 받은 메세지: " << buffer << endl;
		if (strcmp(QuitMsg, buffer) == 0)
		{
			
			cout << "Server Down" << endl;
			cout << "Close Socket" << endl;
			closesocket(skt);
			WSACleanup();
			cout << "Inser quit" << endl;
			break;
			
		}
	}
}



int main(void)
{

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	skt = socket(AF_INET, SOCK_STREAM, 0);

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = inet_addr("192.168.0.162");

	while (1) {
		if (!connect(skt, (SOCKADDR*)&addr, sizeof(addr))) break;
	}
	thread proc1(proc_recv);
	char msg[PACKET_SIZE] = { 0 };

	cout << "Server Connected" << endl;

	char ClientID[100] = {};
	cout << "클라이언트 ID : ";
	cin >> ClientID;
	send(skt, ClientID, strlen(ClientID), 0);
	
	while (!WSAGetLastError()) {
		
		cin >> msg;
		
		if (strcmp(msg, "quit") == 0)
		{
			send(skt, msg, strlen(msg), 0);
			cout << "quit Access" << endl;
			
			return 0;
		}
		
		send(skt, msg, strlen(msg), 0);
	}
	
	proc1.join();

	closesocket(skt);
	WSACleanup();
	
}