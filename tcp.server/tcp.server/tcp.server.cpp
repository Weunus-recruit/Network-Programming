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

#define PORT		4578
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

	skt = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(skt, (SOCKADDR*)&addr, sizeof(addr));
	listen(skt, SOMAXCONN);

	SOCKADDR_IN client = {};
	int client_size = sizeof(client);
	ZeroMemory(&client, client_size);
	client_sock = accept(skt, (SOCKADDR*)&client, &client_size);

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

	/*
	char	sID[] = "Server";
	char	cID[100] = {};
	char	sMsg[100];
	char	cBuffer[PACKET_SIZE] = {};
	char	quitMsg[100] = {};
	int		temp = 0;

	cout << "Client Connected" << endl;
	recv(hClient, cBuffer, sizeof(cBuffer) - 1, 0);
	
	strcpy(cID, cBuffer);
	
	
	while ((strcmp(quitMsg, cBuffer) != 0) && (strcmp(sMsg,"quit") != 0))
	{
		
		recv(hClient, cBuffer, sizeof(cBuffer) - 1, 0);
		cout << "Input Message: ";
		cin >> sMsg ;
		send(hClient, sMsg, sizeof(sMsg) - 1, 0);
		if (temp == 0)
		{
			cout << "Client Name : " << cID << endl;
			strcat(quitMsg, cID);
			strcat(quitMsg, " Send : quit");
			temp =+ 1;
		}
		else
		{
			cout << cBuffer << endl;
		}
		


	}
	*/
	cout << "Close Listen" << endl;
	
	closesocket(client_sock);
	closesocket(skt);

	WSACleanup();
}