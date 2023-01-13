#define _WINSOCK_DEPRECATED_NO_WARNINGS									// winsock ��� ����� ��� ����
#define _CRT_SECURE_NO_WARNINGS											// 'scanf', 'strcpy' ����� ��� ����

#include "pch.h"													// ���������� ���
#include <iostream>													// c++ ǥ�� ����� ���
#include <WinSock2.h>												// ���� ����� ���� ���
#include <thread>													// ��Ƽ������ ������ ���� ���

// proc1�� Client ���Ͽ��� ����. ������ ���� send�� recv�ϵ��� ����
// proc2�� Server ���Ͽ��� ����. Ŭ���̾�Ʈ�� ���� send�� recv�ϵ��� ����

#pragma comment(lib, "ws2_32")											// winsock2 ��� �� ws2_32.DLL ���̺귯�� ��ũ ����
#pragma warning(disable : 4996)											//inet_addr API ����� ��� ���� 



using namespace std;

SOCKET skt;											// Ŭ���̾�Ʈ ���� ����

#define PACKET_SIZE	1024							// ��Ŷ ������ ���� ���� (TCP �ִ밪�� 65,535 byte)

void proc_recv() {									// Ŭ���̾�Ʈ���� recv�޴� �޼����� ó���ϴ� ���ν����� �����忡 �������ֱ� ���� �Լ�
	char buffer[PACKET_SIZE] = {};					// �����ʿ��� ������ buffer�� buffer�� ����� ����
	char QuitMsg[] = "quit";						// "quit"��� ��ȭ���� ���� �� ����

	while (!WSAGetLastError()) {									// Winsock2.h ���� ������ �߻��ñ��� �����Ű�� ���� �ݺ���
		ZeroMemory(&buffer, PACKET_SIZE);							// buffer �޸� �ʱ�ȭ
		recv(skt, buffer, PACKET_SIZE, 0);							// Connect �� �������� ������ buffer�� skt ������ ���� �Ѱܹޱ� ���� recv �Լ�
		cout << "Server���� ���� �޼���: " << buffer << endl;
		if (strcmp(QuitMsg, buffer) == 0)							// �������� ���� buffer�� QuitMsg(="quit")�� ��ġ�ϸ� ���� �� 
		{
			cout << "Quit Message Detected" << endl;
			cout << "Server Down" << endl;
			cout << "Close Socket" << endl;
			closesocket(skt);						// ���� ����
			exit;									// ������ ����
			WSACleanup();							// ws2_32.DLL ��� ���� �Լ�
			break;

		}
	}
}



int main(void)
{

	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);											// winsock2 ��, 2���� winsock ��� �ʱ�ȭ�� ����

	skt = socket(AF_INET, SOCK_STREAM, 0);
	// AF_INET - IPv4 �ּ� �йи��� ���. �������� ����� ���� PF_INET�� ������ �� �� ��� ������ 2������ ��ȯ�Ǹ� ���� �뵵�� ���.
	// SOCK_STREAM - TCP ��Ŷ�� �ްڴٴ� �ǹ�

	SOCKADDR_IN addr = {};									// ���� ���� address�� connect�� ������ ���� ����
	addr.sin_family = AF_INET;								// IPv4
	addr.sin_port = htons(8080);							// 8080�� ��Ʈ ���.
	/*
	�� �˷��� ��Ʈ(well-known port) : 0�� ~ 1023��   => IANA(Internet Assigned Numbers Authority)���� �Ҵ�� TCP �� UDP Ư�� ���ӻ��� ���� ��Ʈ. (����� �ǰ��ִ�)
	��ϵ� ��Ʈ(registered port) : 1024�� ~ 49151��  => well-known port�� �ƴ� �ٸ� ���α׷��� ����ϴ� ��Ʈ
	���� ��Ʈ(dynamic port) : 49152�� ~ 65535��      => �� ������ Ŭ���̾�Ʈ���� Response�� ������ �Ϳ� ���Ǵ� ��Ʈ. ����� ��Ʈ�� �� ���� ��Ʈ 80(HTTP)�� ���
																													   ������ ��Ʈ�� dynamic port(ex. 50000)
	*/

	addr.sin_addr.s_addr = inet_addr("192.168.0.162");									// ���� Host IP Address

	while (1) {
		if (!connect(skt, (SOCKADDR*)&addr, sizeof(addr))) break;						// ������ ���� connect�� accept �ɶ����� connect ��û
	}
	thread proc1(proc_recv);															// proc1�� ó���ϴ� �����忡�� proc_recv�� �۵���Ű�� ���� ����
	char msg[PACKET_SIZE] = { 0 };

	cout << "Server Connected" << endl;													// connect�� accept �����Ƿ� ������ Ŀ��Ʈ �Ǿ����� ���

	char ClientID[100] = {};															// ������ ���� ID�� �۽�
	cout << "Ŭ���̾�Ʈ ID : ";
	cin >> ClientID;
	send(skt, ClientID, strlen(ClientID), 0);

	while (!WSAGetLastError()) {

		cin >> msg;

		if (strcmp(msg, "quit") == 0)													// Ŭ���̾�Ʈ�� ������ �޽����� "quit"�϶��� ���α׷��� �����ϱ� ���� ���
		{
			send(skt, msg, strlen(msg), 0);
			cout << "quit Access" << endl;

			return 0;
		}

		send(skt, msg, strlen(msg), 0);													// �޽����� "quit"�� �ƴ� ��� ������� send
	}

	proc1.join();												// proc1 �����尡 ����ɶ����� ��� �� ����Ǿ����� ���� �ڵ带 ����
	closesocket(skt);											// ���� ����
	WSACleanup();												// ws2_32.DLL ��� ���� 

}