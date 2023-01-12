#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <WinSock2.h>
#include <time.h>

#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)

#define PORT		4578
#define PACKET_SIZE	1024

using namespace std;

int main(void)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET hListen;
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tListenAddr = {};
	tListenAddr.sin_family = AF_INET;
	tListenAddr.sin_port = htons(PORT);
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	SOCKADDR_IN tCIntAddr = {};
	int iCIntSize = sizeof(tCIntAddr);
	SOCKET hClient = accept(hListen, (SOCKADDR*)&tCIntAddr, &iCIntSize);

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
		
		cout << "Input Message: ";
		cin >> sMsg ;
		recv(hClient, cBuffer, sizeof(cBuffer) - 1, 0);
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
		send(hClient, sMsg, sizeof(sMsg) -1, 0);


	}

	cout << "Close Listen" << endl;
	
	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();

	return 0;
}