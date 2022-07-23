#include "../tcpSocket/tcpSocke.h"

int main()
{
	init_Socket();

	SOCKET serfd = CreatServerScoket();

	cout << "wait client connect...\n";

	SOCKET clifd = accept(serfd, NULL, NULL);	//如果有客户端请求连接
	if (clifd == INVALID_SOCKET)
	{
		err("accept");
	}
	/*thread th = thread(ListenHeartBeat, clifd);

	th.detach();*/

	char buf[BUFSIZ * 2] = { 0 };

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//接受并输出
	{
		cout << buf << endl;
	}
	memset(buf, 0, sizeof(buf));//缓冲区初始化，防止重复输出
	if (recv(clifd, buf, BUFSIZ, 0) > 0)//接受并输出
	{
		cout << "PCName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));
	send(clifd, "1", 1, 0);//发送一个字符防止粘包

	if (recv(clifd, buf, BUFSIZ, 0) > 0)
	{
		cout << "UserName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));
	send(clifd, "1", 1, 0);
	if (recv(clifd, buf, BUFSIZ, 0) > 0)
	{
		cout << "IP is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));
	send(clifd, "1", 1, 0);
	close_Socket();
	return 0;
}