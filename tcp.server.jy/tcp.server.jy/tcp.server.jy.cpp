#define _WINSOCK_DEPRECATED_NO_WARNINGS								// winsock ��� ����� ��� ����
#define _CRT_SECURE_NO_WARNINGS										// 'scanf', 'strcpy' ����� ��� ����

#include "pch.h"													// ���������� ���
#include <iostream>													// c++ ǥ�� ����� ���
#include <WinSock2.h>												// ���� ����� ���� ���
#include <thread>													// ��Ƽ������ ������ ���� ���

#pragma comment(lib, "ws2_32")										// winsock2.h�� ws2_32.DLL ���̺귯�� ��ũ ����

#define PACKET_SIZE	1024											// ��Ŷ ������ ���� ����

using namespace std;

SOCKET skt, client_sock;											// ���� ���α׷����� ���Ǵ� ���ϰ� Ŭ���̾�Ʈ ��û�� ��ٸ��ٰ� connect ��û�� ���� ������ �α�
																	// ���� ���� �ΰ��� ����

void proc_recvs() {													// �������� recv�Ǵ� �޽����� ó���ϴ� ���ν����� �����忡 �������ֱ� ���� �Լ�
	char buffer[PACKET_SIZE] = { 0 };								// Ŭ���̾�Ʈ�� ������ buffer�� ������
	char ClientID[PACKET_SIZE] = {};								// Ŭ���̾�Ʈ�� ���� ���� ID�� ������ ������ ID�� �����ϱ� ���� ����
	ZeroMemory(&ClientID, PACKET_SIZE);								// ID �޸� �ʱ�ȭ
	recv(client_sock, ClientID, PACKET_SIZE, 0);					// Ŭ���̾�Ʈ ������ ���� �Ѿ���� ClientID�� ���� �ޱ� ���� recv ����(���)
	char QuitMsg[] = "quit";										// Ŭ���̾�Ʈ �޽����� quit�� ��� �����ϱ� ���� ����
	while (!WSAGetLastError()) {									// Winsock2.h ���� ������ �߻��ñ��� �����Ű�� ���� �ݺ���
		ZeroMemory(&buffer, PACKET_SIZE);							// ���� �޸� �ʱ�ȭ
		recv(client_sock, buffer, PACKET_SIZE, 0);					// Accept �� Ŭ���̾�Ʈ�� send�� �޽����� recv
		cout << ClientID << "���� ���� �޽���: " << buffer << endl;
		if (strcmp(QuitMsg, buffer) == 0)							// Ŭ���̾�Ʈ�� ���� buffer�� QuitMsg(="quit")�� ��ġ�ϸ� ���α׷� ����
		{
			cout << "Client Left" << endl;
			cout << "Close Socket" << endl;

			closesocket(client_sock);		// Ŭ���̾�Ʈ ���� ����
			closesocket(skt);				// ���� ���� ����
			exit;							// ������ ����
			WSACleanup();					// ws2_32.DLL ��� ���� �Լ�
			break;

		}
	}
}

int main()
{
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);											// winsock2 ��, 2���� winsock ��� �ʱ�ȭ�� ����

	/*
	typedef struct WSAData {
		WORD                wVersion;											// ����
		WORD                wHighVersion;										//����� �� �ִ� ���� �������� wVersion�� ��ġ�Ѵ�.
		char                   szDescription[WSADESCRIPTION_LEN + 1];	//���� ����
		char                   szSystemStatus[WSASYS_STATUS_LEN + 1];	//���� ���ڿ�
		unsigned short     iMaxSockets;										//�ִ� ����
		unsigned short     iMaxUdpDg;										//������ �׷��� �ִ� ũ��
		char FAR* lpVendorInfo;											//���� ����(������ ����)
	*/

	skt = socket(AF_INET, SOCK_STREAM, 0);										// ���� ���� ����

	SOCKADDR_IN addr = {};
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;													// ���� ���Ͽ��� IPv4 �ּ� �йи��� ���.
	addr.sin_port = htons(8080);												// 8080�� ��Ʈ ���.
	addr.sin_addr.s_addr = INADDR_ANY;											// ���� ���α׷����� ��ǻ�Ϳ� �����ϴ� ��밡���� ��ī���� IP�ּҸ� ����϶�� �ǹ�.

	bind(skt, (SOCKADDR*)&addr, sizeof(addr));									// ������ ������ ������ ����� IP�� ��Ʈ ��ȣ�� ���Ͽ� bind(����) ��Ŵ
	listen(skt, SOMAXCONN);														// Ŭ���̾�Ʈ�� connect ��û�� �ö����� listen ���·� ���.
	// SOMAXCONN => Socket Max Connection�� ���ڷ� ������ 5�� �ۿ�.

	SOCKADDR_IN client = {};													// listen ���¿��� connect ��û�� �νĵǸ� client�� ������ �ޱ� ���� ����
	int client_size = sizeof(client);
	ZeroMemory(&client, client_size);
	client_sock = accept(skt, (SOCKADDR*)&client, &client_size);				// Ŭ���̾�Ʈ ������ ���� ���ϰ� ����ϴ� ���� accept�ϸ鼭 Ŭ���̾�Ʈ ���� ������ ���� 

	if (client_sock == INVALID_SOCKET)											// Ŭ���̾�Ʈ ������ �۵��� �Ұ����ϸ� ���� �޽��� ���
		cout << "accept error" << endl;

	thread proc2(proc_recvs);													// proc2�� ó���ϴ� �����忡�� proc_recvs�� �۵��ϱ� ���� ����

	cout << "Client Connected" << endl;											// connect�� accept �Ǿ����Ƿ� Ŭ���̾�Ʈ�� Ŀ��Ʈ �Ǿ����� ���.

	char buffer[PACKET_SIZE] = { 0 };											// ������ ������ buffer�� �����ϱ� ���� ����

	while (!WSAGetLastError()) {
		cin >> buffer;															// ���� ����(=�޽���) �Է�

		if (strcmp(buffer, "quit") == 0)										// ���� �޽����� "quit"�� ��� Ŭ���̾�Ʈ�� �޽����� ������ �� ���α׷� ����
		{
			send(client_sock, buffer, strlen(buffer), 0);
			cout << "quit Access" << endl;

			return 0;
		}

		send(client_sock, buffer, strlen(buffer), 0);							// �޽����� "quit"�� �ƴ� ��쿣 ������� �޽����� ����
	}


	proc2.join();																// proc2 �����尡 ����� �� ���� ��� �� ����Ǿ����� ���� �ڵ带 ����

	closesocket(client_sock);													// Ŭ���̾�Ʈ ���� ����
	closesocket(skt);															// ���� ���� ����
	WSACleanup();																// ws2_32.DLL ��� ����
}