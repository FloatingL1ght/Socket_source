#include "../tcpSocket/tcpSocke.h"

int main()
{
	init_Socket();

	SOCKET serfd = CreatServerScoket();

	cout << "wait client connect...\n";

	SOCKET clifd = accept(serfd, NULL, NULL);	//����пͻ�����������
	if (clifd == INVALID_SOCKET)
	{
		err("accept");
	}
	char buf[BUFSIZ] = { 0 };

	if (recv(clifd, buf, BUFSIZ, 0) > 0)//���ܲ����
	{
		decode(buf);
		cout << buf << endl;
	}
	memset(buf, 0, sizeof(buf));//��������ʼ������ֹ�ظ����
	if (recv(clifd, buf, BUFSIZ, 0) > 0)
	{
		decode(buf);
		cout << "PCName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));
	//send(clifd, "1", 1, 0);//����һ���ַ���ֹճ��
	//Sleep(200);
	if (recv(clifd, buf, BUFSIZ, 0) > 0)
	{
		decode(buf);
		cout << "UserName is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));
	//send(clifd, "1", 1, 0);
	//Sleep(200);
	if (recv(clifd, buf, BUFSIZ, 0) > 0)
	{
		decode(buf);
		cout << "IP is :" << buf << endl;
	}
	memset(buf, 0, sizeof(buf));
	//send(clifd, "1", 1, 0);
	close_Socket();
	return 0;
}