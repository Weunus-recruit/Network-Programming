#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include "pch.h"
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <WinSock2.h>
#include <Windows.h>



#pragma comment(lib, "ws2_32")
#pragma warning(disable : 4996)
IN_ADDR GetDefaultMyIP();

IN_ADDR GetDefaultMyIP()
{
	char localhostname[MAX_PATH];
	IN_ADDR addr = { 0, };

	if (gethostname(localhostname, MAX_PATH) == SOCKET_ERROR)//호스트 이름 얻어오기
	{
		return addr;
	}
	HOSTENT* ptr = gethostbyname(localhostname);//호스트 엔트리 얻어오기
	while (ptr && ptr->h_name)
	{
		if (ptr->h_addrtype == PF_INET)//IPv4 주소 타입일 때
		{
			memcpy(&addr, ptr->h_addr_list[0], ptr->h_length);//메모리 복사
			break;//반복문 탈출
		}
		ptr++;
	}
	return addr;
}

#define PORT		4578
#define PACKET_SIZE	1024

using namespace std;

int main(void)
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	IN_ADDR addr;

	addr = GetDefaultMyIP();
	

	SOCKET hSocket;
	hSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tAddr = {};
	tAddr.sin_family = AF_INET;
	tAddr.sin_port = htons(PORT);
	tAddr.sin_addr.s_addr = inet_addr(inet_ntoa(addr));

	connect(hSocket, (SOCKADDR*)&tAddr, sizeof(tAddr));


	char	cBuffer[PACKET_SIZE] = {};
	char	cMsg[100];
	char	quitMsg[] = "quit";
	char	cName[100];
	char	cNameCopy[100];
	int		temp = 0;

	cout << "Server Connected" << endl;

	cout << "Name : ";
	cin >> cName;
	send(hSocket, cName, strlen(cName), 0);
	strcat(cName, " Send : ");
	strcpy(cNameCopy, cName);
	

	while ((strcmp(quitMsg,cBuffer) != 0) && (strcmp(cMsg,"quit") != 0))
	{
		cout << "Input Message: ";
		cin >> cMsg;
		strcat(cName, cMsg);
		send(hSocket, cName, sizeof(cName) -1, 0);
		recv(hSocket, cBuffer, sizeof(cBuffer)-1, 0);
		cout << "Server Send : " << cBuffer << endl;
		strcpy(cName, cNameCopy);
	}
	cout << "Close Socket" << endl;
	closesocket(hSocket);

	WSACleanup();


	return 0;
}