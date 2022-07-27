#include "../tcpSocket/tcpSocke.h"

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

	char buf[BUFSIZ * 2] = { 0 };

	while (true)
	{
		recv(clifd, buf, BUFSIZ * 2, 0);
		decode(buf);
		cout << buf << endl;
		memset(buf, 0, sizeof(buf));
		Sleep(200);
	}
	Sleep(200);


	close_Socket();
	return 0;
}