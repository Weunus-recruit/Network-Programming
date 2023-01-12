#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS


#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <time.h>
#include <thread>

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

#define PACKET_SIZE	1024

using namespace std;

SOCKET skt, client_sock;

void proc_recvs() {
	char buffer[PACKET_SIZE] = { 0 };
	char ClientID[PACKET_SIZE] = {};
	ZeroMemory(&ClientID, PACKET_SIZE);
	recv(client_sock, ClientID, PACKET_SIZE, 0);
	char QuitMsg[] = "quit";
	while (!WSAGetLastError()) {
		ZeroMemory(&buffer, PACKET_SIZE);
		recv(client_sock, buffer, PACKET_SIZE, 0);
		cout << ClientID << "에게 받은 메시지: " << buffer << endl;
		if (strcmp(QuitMsg, buffer) == 0)
		{
			cout << "Client Left" << endl;
			cout << "Close Socket" << endl;

			closesocket(client_sock);
			closesocket(skt);
			WSACleanup();
			cout << "Insert quit" << endl;
			break;

		}
	}
}

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);

	skt = socket(AF_INET, SOCK_STREAM, 0);
	
	SOCKADDR_IN addr = {};
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	bind(skt, (SOCKADDR*)&addr, sizeof(addr));
	listen(skt, 5);

	SOCKADDR_IN client = {};
	int client_size = sizeof(client);
	ZeroMemory(&client, client_size);
	client_sock = accept(skt, (SOCKADDR*)&client, &client_size);

	if (client_sock == INVALID_SOCKET)
		cout << "accept error" << endl;

	char buffer[PACKET_SIZE] = { 0 };
	thread proc2(proc_recvs);

	cout << "Client Connected" << endl;

	while (!WSAGetLastError()) {
		cin >> buffer;
		
		if (strcmp(buffer, "quit") == 0)
		{
			send(client_sock, buffer, strlen(buffer), 0);
			cout << "quit Access" << endl;
			
			return 0;
		}
		
		send(client_sock, buffer, strlen(buffer), 0);
	}
	

	proc2.join();

	cout << "Close Listen" << endl;
	
	closesocket(client_sock);
	closesocket(skt);

	WSACleanup();
}