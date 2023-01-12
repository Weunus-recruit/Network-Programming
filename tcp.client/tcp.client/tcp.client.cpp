#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

#define PORT		4578
#define PACKET_SIZE	1024
#define SERVER_IP	"192.168.0.162"

using namespace std;

int main(void)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hSocket;
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));

	cout << "Server Connected" << endl;

	cout << "Name : ";
	char	cName[100];
	cin >> cName;
	strcat(cName, " Send : ");
	char	cNameCopy[100];
	strcpy(cNameCopy, cName);
	send(hSocket, cName, strlen(cName), 0);
	
	char	cBuffer[PACKET_SIZE] = {};
	char	cMsg[100];
	
	

	while (cBuffer != "quit")
	{
		cout << "Input Message: ";
		cin >> cMsg;
		strcat(cName, cMsg);
		send(hSocket, cName, strlen(cName), 0);
		recv(hSocket, cBuffer, PACKET_SIZE, 0);
		cout << "Server Send : " << cBuffer << endl;
		strcpy(cName, cNameCopy);
	}
	cout << "Close Socket" << endl;
	closesocket(hSocket);

	WSACleanup();


	return 0;
}