#include "stdafx.h"
#include <Wininet.h>
#include <wincrypt.h>
#pragma comment(lib,"wininet.lib")
#include "Base64.h"

extern	CEdit	m_edit;						//状态栏
extern	CEdit	m_version_edit;				//版本
//////////////////////////////////////////////////////////////////////////		INI配置
extern	CString			Ini_szUser_Agent;
extern	CString			Ini_szHeaders;
extern	bool			Ini_bReferer;
extern	bool			Ini_bX_Forwarded_For;

extern	CString			Ini_szASP_POST_DATA;
extern	CString			Ini_szASPX_POST_DATA;
extern	CString			Ini_szPHP_POST_DATA;
extern	CString			Ini_szPHP_POST_Z0_DATA;
//////////////////////////////////////////////////////////////////////////
extern	CStringArray	szGlobal_RandType;										//变换的URL类型
extern	CStringArray	szGlobal_RandTypeExclude;								//变换的URL类型，排除


#define TOHEX( x ) ( ( x ) > 9 ? ( x ) + 55 : ( x ) + 48 )


static CString TimeToTime(CString szDataTimes)
{
	CString szReturn;
	CString szTemp;
	int posX=0,posY=0,posZ=0;
	posX = szDataTimes.Find("-");
	posY = szDataTimes.Find("-",posX+1);
	posZ = szDataTimes.Find(" ",posY+1);
	
	szTemp = szDataTimes.Mid(posX+1, posY-posX-1);
	if (szTemp.GetLength() == 2)
	{
		szReturn = szDataTimes.Left(posY+1);
	}else{
		szReturn = szDataTimes.Left(posX+1) + "0" +szTemp +"-";
	}

	szTemp = szDataTimes.Mid(posY+1, posZ-posY-1);
	if (szTemp.GetLength() == 2)
	{
		szReturn += szDataTimes.Mid(posY+1, posZ-posY-1) + szDataTimes.Mid(posZ, szDataTimes.GetLength()-posZ);
	}else{
		szReturn += "0" + szDataTimes.Mid(posY+1, posZ-posY-1) + szDataTimes.Mid(posZ, szDataTimes.GetLength()-posZ);
	}

	//2014-1-6 11:46:25
	return szReturn;
}
//字符串编码格式转换

//GB2312 转换成 Unicode：
static wchar_t* GB2312ToUnicode(const char* szGBString)
{
	UINT nCodePage = 936; //GB2312
	int nLength=MultiByteToWideChar(nCodePage,0,szGBString,-1,NULL,0);
	wchar_t* pBuffer = new wchar_t[nLength+1];
	MultiByteToWideChar(nCodePage,0,szGBString,-1,pBuffer,nLength);
	pBuffer[nLength]=0;
	return pBuffer;
}

//BIG5 转换成 Unicode：
static wchar_t* BIG5ToUnicode(const char* szBIG5String)
{
	UINT nCodePage = 950; //BIG5
	int nLength=MultiByteToWideChar(nCodePage,0,szBIG5String,-1,NULL,0);
	wchar_t* pBuffer = new wchar_t[nLength+1];
	MultiByteToWideChar(nCodePage,0,szBIG5String,-1,pBuffer,nLength);
	pBuffer[nLength]=0;
	return pBuffer;
}

//Unicode 转换成 GB2312：
static char* UnicodeToGB2312(const wchar_t* szUnicodeString)
{
	UINT nCodePage = 936; //GB2312
	int nLength=WideCharToMultiByte(nCodePage,0,szUnicodeString,-1,NULL,0,NULL,NULL);
	char* pBuffer=new char[nLength+1];
	WideCharToMultiByte(nCodePage,0,szUnicodeString,-1,pBuffer,nLength,NULL,NULL);
	pBuffer[nLength]=0;
	return pBuffer;
}

//Unicode 转换成 BIG5：
static char* UnicodeToBIG5(const wchar_t* szUnicodeString)
{
	UINT nCodePage = 950; //BIG5
	int nLength=WideCharToMultiByte(nCodePage,0,szUnicodeString,-1,NULL,0,NULL,NULL);
	char* pBuffer=new char[nLength+1];
	WideCharToMultiByte(nCodePage,0,szUnicodeString,-1,pBuffer,nLength,NULL,NULL);
	pBuffer[nLength]=0;
	return pBuffer;
}

//繁体中文BIG5 转换成 简体中文 GB2312
static char* BIG5ToGB2312(const char* szBIG5String)
{
	LCID lcid = MAKELCID(MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED),SORT_CHINESE_PRC);
	wchar_t* szUnicodeBuff = BIG5ToUnicode(szBIG5String);
	char* szGB2312Buff = UnicodeToGB2312(szUnicodeBuff);
	int nLength = LCMapString(lcid,LCMAP_SIMPLIFIED_CHINESE, szGB2312Buff,-1,NULL,0);
	char* pBuffer = new char[nLength + 1];
	LCMapString(0x0804,LCMAP_SIMPLIFIED_CHINESE,szGB2312Buff,-1,pBuffer,nLength);
	pBuffer[nLength] = 0;
	delete[] szUnicodeBuff;
	delete[] szGB2312Buff;
	return pBuffer;
}

//简体中文 GB2312 转换成 繁体中文BIG5
static char* GB2312ToBIG5(const char* szGBString)
{
	LCID lcid = MAKELCID(MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_SIMPLIFIED),SORT_CHINESE_PRC);
	int nLength = LCMapString(lcid,LCMAP_TRADITIONAL_CHINESE,szGBString,-1,NULL,0);
	char* pBuffer=new char[nLength+1];
	LCMapString(lcid,LCMAP_TRADITIONAL_CHINESE,szGBString,-1,pBuffer,nLength);
	pBuffer[nLength]=0;
	wchar_t* pUnicodeBuff = GB2312ToUnicode(pBuffer);
	char* pBIG5Buff = UnicodeToBIG5(pUnicodeBuff);
	delete[] pBuffer;
	delete[] pUnicodeBuff;
	return pBIG5Buff;
}


//UTF8 转换成 简体中文 GB2312
static CString UTF8ToGB2312( CString strIn )
{
	CString strOut = "";
	int nLength=MultiByteToWideChar(CP_UTF8, 0, strIn, -1, NULL, 0);
	wchar_t* pBuffer = new wchar_t[nLength+1];
	MultiByteToWideChar(CP_UTF8, 0, strIn, -1, pBuffer, nLength);
	pBuffer[nLength]=0;

	nLength=WideCharToMultiByte(CP_ACP, 0, pBuffer, -1, NULL, 0, NULL, NULL);
	char* pBuffer2=new char[nLength+1];
	WideCharToMultiByte(CP_ACP, 0, pBuffer, -1, pBuffer2, nLength, NULL, NULL);
	pBuffer2[nLength]=0;
	strOut = pBuffer2;
	delete[] pBuffer;
	delete[] pBuffer2;
	return strOut;
}

//简体中文 GB2312 转换成 UTF8
static CString GB2312ToUTF8( CString strIn )
{
	CString strOut = "";
	WCHAR* strSrc    = NULL;
	TCHAR* szRes    = NULL;
	int len = MultiByteToWideChar(CP_ACP, 0, strIn, -1, NULL,0);
	unsigned short* wszUtf8 = new unsigned short[len+1];
	memset(wszUtf8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, strIn, -1, (LPWSTR)wszUtf8, len);
	len = WideCharToMultiByte(CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, NULL, 0, NULL, NULL);
	char* szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	WideCharToMultiByte (CP_UTF8, 0, (LPCWSTR)wszUtf8, -1, szUtf8, len, NULL,NULL);
	strOut = szUtf8;
	delete[] szUtf8;
	delete[] wszUtf8;
	return strOut;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static CString MyBase64Encode(int nSiteCode, CString szInpt)
{
	if (nSiteCode == 950)
	{
		szInpt = GB2312ToBIG5(szInpt);
	}else if(nSiteCode == 65001)
	{
		szInpt = GB2312ToUTF8(szInpt);
	}

	CBase64 Base64;
	CString szMyBase64Encode = Base64.Encode(szInpt, szInpt.GetLength());
	szMyBase64Encode.Replace("+","%2B");
	return szMyBase64Encode;
}

static CString HosttoIP(CString szServerName)
{
	szServerName.MakeLower();
	szServerName.Replace("http://","");
	szServerName.Replace("https://","");
	int posX = szServerName.Find("/");
	int poxY = szServerName.Find(":");
	if (poxY != -1 && posX > poxY)
	{
		posX = poxY;
	}
	if (posX != -1)
	{
		szServerName = szServerName.Mid(0,posX);
	}

	CString szHostIP = "";
	WSADATA WSAData;
	int WSA_return = WSAStartup(0x0101,&WSAData);
	//结构指针
	HOSTENT *host_entry;
	if(WSA_return == 0)
	{
		//即要解析的域名或主机名
		host_entry = gethostbyname(szServerName);
		if(host_entry != 0)
		{
			szHostIP.Format("%d.%d.%d.%d",
				(host_entry->h_addr_list[0][0]&0x00ff),
				(host_entry->h_addr_list[0][1]&0x00ff),
				(host_entry->h_addr_list[0][2]&0x00ff),
				(host_entry->h_addr_list[0][3]&0x00ff));
		}
	}
	return szHostIP;
}

static CString ToShotUrl(CString Url)
{
	Url.Replace("http://","");
	Url.Replace("https://","");
	Url = Url.Mid(0,Url.Find("/"));
	return " "+Url+" ";
}

//参数说明:
//source： 需要分割的源字符串；
//dest： 存储分割后的各个字符串变量的目标变量；
//division：分割字符
static void decodeCString(CString source, CStringArray& dest, char division)
{
	dest.RemoveAll();
	for(int i=0;i < source.GetLength();i++)
	{
		if(source.GetAt(i)== division)
		{
			dest.Add(source.Left(i)); //去掉右边
			for(int j=0;j < (dest.GetSize()-1);j++)
			{
				//去掉左边
				dest[dest.GetSize()-1] = dest[dest.GetSize()-1].Right(dest[dest.GetSize()-1].GetLength()-dest[j].GetLength()-1); 
			}
		} 
	}
}
static CString DelCRLF(CString temp)
{
	temp.Replace("\r","");
	temp.Replace("\n","");
	temp.TrimLeft();
	temp.TrimRight();

	int templen = temp.GetLength();
	int i=1;
	while (temp[templen-i] == '\r' || temp[templen-i] == '\n')
	{
		temp.Delete( templen-i, 1 );
		i++;
	}
	return temp;
}

static CString UrlEnCode(int nSiteCode, CString szstr, bool bIsPercent=true, bool bIsall=false)
{
	if (nSiteCode == 950)
	{
		szstr = GB2312ToBIG5(szstr);
	}else if(nSiteCode == 65001)
	{
		szstr = GB2312ToUTF8(szstr);
	}

	CString encoded;
	char* buf = new char[16];

	unsigned char c;
	for(int i=0; i < szstr.GetLength(); i++)
	{
		c = szstr[i];
		if (!bIsall)
		{
			if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ||
				c == '.' || c == '-' || c == '_')
			{
				encoded += c;
				continue;
			}
			if(c == ' ')
			{
				encoded += '+';
				continue;
			}
		}
		sprintf_s(buf, 16, "%02X", c);
		if (bIsPercent)
		{
			encoded += "%";
		}
		encoded += buf;
	}
	delete[] buf;
	return encoded;
}

static bool UrlExplode(CString szUrl, CString &szHost, CString &szPort, CString &szPath)
{
	bool	IsHttps=false;
	CString szUrlExpTemp = szUrl;
	szUrlExpTemp.MakeLower();

	int posX = 0;
	if (szUrlExpTemp.Find("https://") != -1)
	{
		IsHttps=true;
		posX = szUrlExpTemp.Find("https://");
	}else{
		IsHttps=false;
		posX = szUrlExpTemp.Find("http://");
	}
	int posY = szUrl.Find("/", posX+10);
	if (posY == -1)
	{
		szPath = "/";
		posY = szUrl.GetLength();
	}else{
		szPath = szUrl.Mid(posY, szUrl.GetLength()-posY);
	}
	
	int posZ = szUrl.Find(":", posX+10);
	if (posZ != -1 && posZ < posY)
	{
		szPort = szUrl.Mid(posZ+1, posY-posZ-1);
		if (IsHttps)
		{
			szHost = szUrl.Mid(posX+8, posZ-posX-8);
		}else{
			szHost = szUrl.Mid(posX+7, posZ-posX-7);
		}
	}else{
		if (IsHttps)
		{
			szHost = szUrl.Mid(posX+8, posY-posX-8);
			szPort = "443";
		}else{
			szHost = szUrl.Mid(posX+7, posY-posX-7);
			szPort = "80";
		}
	}
	return IsHttps;
}


static int GetStringArrPos(CStringArray* pArray, CString str)
{
	int nReturn = -1;
	int i=0, nCount = (int)pArray->GetCount();

	if (nCount == 0)
		return nReturn;


	for (i = 0; i <nCount; i++)
	{
		if (pArray->GetAt(i) == str)
		{
			nReturn = i;
			break;
		}
	}
	return nReturn;
}

/*
	在 str 里从 pos 开始倒这查找 ch
	如果pos为0，则从str最后开始找
	返回最右边出现的位置
	str.Right();
*/
static int rfind(CString str, CString ch, int pos=0)
{
	if (ch == "" || str == "") return -1;

	int strlens = str.GetLength();
	int chlens = ch.GetLength();
	if (pos == 0)
	{
		pos = strlens;
	}else{
		pos = strlens - pos;
	}
	for (; pos >= 0 ; pos--)
	{
		if (ch == str.Mid(pos,chlens))
		{
			return strlens-pos;
		}
	}
	return -1;
}

static CString GetUpperDtoryPath(CString szPath, bool IsWin)		//获取返回上一层的路径地址
{
	if (szPath == "")
	{
		return "";
	}

	CString temp = szPath;
	int nLen = temp.GetLength();
	int nPos = -1;

	if (IsWin)
	{
		if (nLen == 3)//最上层了
		{
			return szPath;
		}
		if (temp.Right(1) == '\\')
		{
			temp = temp.Mid(0, nLen - 1);
		}
		nPos = rfind(temp, "\\");
	}else{
		if (nLen == 1)//最上层了
		{
			return szPath;
		}
		if (temp.Right(1) == '/')
		{
			temp = temp.Mid(0, nLen - 1);
		}
		nPos = rfind(temp, "/");
	}

	if (nPos != -1)
	{
		return temp.Mid(0, nLen - nPos);
	}else{
		return szPath;
	}
}

//参数说明:
//treeCtrl:	m_tree
//szPath:		c:\11\22\33\
//bIsWin:		true
static HTREEITEM FindItems(CTreeCtrl& treeCtrl, CString szPath, bool bIsWin)
{
	if (szPath == "") return NULL;

	CStringArray szPathArr;
	szPathArr.RemoveAll();

	if (bIsWin)
	{
		if (szPath.Right(1) != "\\")
		{
			szPath = szPath + "\\";
		}
		decodeCString(szPath, szPathArr, '\\');					//注意Win和Linux 不一样 \、/
	}else{
		if (szPath.Right(1) != "/")
		{
			szPath = szPath + "/";
		}
		decodeCString(szPath, szPathArr, '/');					//注意Win和Linux 不一样 \、/
		szPathArr.SetAt(0,"/");
	}

	int			nIndex = (int)szPathArr.GetCount();
	int			i = 0;
	CString		szPathArrAtI;
	HTREEITEM	h_find = NULL;
	bool		bIsFind = false;		//都没有找到，就要插入了
	CString		szFindItemsTemp;

	while(i < nIndex)
	{
		bIsFind = false;
		szPathArrAtI = szPathArr.GetAt(i);

		if (szPathArrAtI == "")
		{
			//break;
		}
		if (i == 0)				// 0 第一次，Root节点
		{
			//////////////////////////////////////////////////////////////////////////
			if ( nIndex > 2 )
			{
				if (szPathArr.GetAt(0) == "" && szPathArr.GetAt(1) == "")
				{
					i = i + 2;
					szPathArrAtI = "\\\\" + szPathArr.GetAt(2);
				}
			}
			//////////////////////////////////////////////////////////////////////////
			h_find = treeCtrl.GetRootItem();
			while(h_find)					//枚举所有Root节点
			{
				szFindItemsTemp = treeCtrl.GetItemText(h_find);
				szFindItemsTemp.MakeLower();	//C D E 转小写比较
				szPathArrAtI.MakeLower();
				if (szPathArrAtI == szFindItemsTemp)						//找到Root节点
				{
					bIsFind = true;
					break;
				}
				h_find = treeCtrl.GetNextItem(h_find, TVGN_NEXT);
			}
			if (!bIsFind)		//找了一圈都没有找到，就要添加了
			{
				h_find = treeCtrl.InsertItem(szPathArrAtI, TVI_ROOT, TVI_LAST);
			}
		}else{												// 不是 Root节点
			if ( treeCtrl.ItemHasChildren(h_find) )			//判断给定树项是否包含子项,2014.04.04BUG，有子项，不一定已经存在
			{
				HTREEITEM h_find_temp = h_find;	//遍历查找，如果没有，还要用到这个句柄，插入一条
				int RootItemNext = 0;						//又开始，重新开始一次

				while ( h_find_temp )		//遍历所有，查找
				{
					if (RootItemNext == 0)
					{
						h_find_temp = treeCtrl.GetChildItem(h_find_temp);							//获取下一级的，接下来就是对比，这级的所有
						RootItemNext++;
					}
					szFindItemsTemp = treeCtrl.GetItemText(h_find_temp);
					CString szFindItemsTemp2 = szPathArrAtI;
					if (bIsWin)
					{
						szFindItemsTemp.MakeLower();//转小写比较 BUG 2014.04.04
						szFindItemsTemp2.MakeLower();//转小写比较
					}
					if(szFindItemsTemp2 == szFindItemsTemp)		//如果找到，跳出循环，寻找下一级
					{
						bIsFind = true;
						h_find = h_find_temp;
						break;
					}else{
						h_find_temp = treeCtrl.GetNextItem( h_find_temp, TVGN_NEXT );	//没有找到，寻找同级的下一个 C 完了 D 完了 E
					}
				}
				if (!bIsFind)
				{
					HTREEITEM h_find2;
					h_find2 = treeCtrl.InsertItem(szPathArrAtI, 1, 1, h_find);
					treeCtrl.Expand(h_find, TVE_EXPAND);
					h_find = h_find2;
				}
			}else{										//判断给定树项是否包含子项,没有，就插入
				HTREEITEM h_find2;
				h_find2 = treeCtrl.InsertItem(szPathArrAtI, 1, 1, h_find);
				treeCtrl.Expand(h_find, TVE_EXPAND);
				h_find = h_find2;
			}
		}
		i++;
	}
	return h_find;
}

//返回有多少个子节点
//nCount		如果子节点大于这个数就返回假，效率问题，0 不使用
static int nChildCount(CTreeCtrl& treeCtrl, HTREEITEM hItem)
{
	int i = 0;			//计数，几个字节点
	hItem = treeCtrl.GetChildItem(hItem);
	while ( hItem )		//遍历所有，查找
	{
		i++;
		hItem = treeCtrl.GetNextItem( hItem, TVGN_NEXT );	//寻找同级的下一个
	}
	return i;
}


//treeCtrl		指向树形控件的指针
//hItem			要查找节点的父节点
//szItemName	对比的字符串
//bIsWin		区分大小写
static bool bIsExistItem(CTreeCtrl& treeCtrl, HTREEITEM hItem, CString szItemName, bool bIsWin)
{
	CString szFindItemsTemp = szItemName,
		szFindItemsTemp2;
	if (bIsWin)
	{
		szFindItemsTemp.MakeLower();//转小写比较
	}

	hItem = treeCtrl.GetChildItem(hItem);
	while ( hItem )		//遍历所有，查找
	{
		szFindItemsTemp2 = treeCtrl.GetItemText(hItem);
		if (bIsWin)
		{
			szFindItemsTemp2.MakeLower();//转小写比较
		}

		if(szFindItemsTemp == szFindItemsTemp2)		//如果找到，跳出循环，寻找下一级
		{
			return true;
		}else{
			hItem = treeCtrl.GetNextItem( hItem, TVGN_NEXT );	//没有找到，寻找同级的下一个
		}
	}
	return false;
}

//treeCtrl:指向树形控件的指针
//hItem: 要删除的所有节点的父节点
static bool bDeleteItems(CTreeCtrl& treeCtrl, HTREEITEM hItem)
{
	if(!treeCtrl.ItemHasChildren(hItem))
		return false;     
	HTREEITEM hChild = treeCtrl.GetChildItem(hItem); 
	do   
	{   
		if(treeCtrl.ItemHasChildren(hChild)) {   
			bDeleteItems(treeCtrl,hChild);   
		}
		else { 
			HTREEITEM hChild1 = treeCtrl.GetNextSiblingItem(hChild);
			CString str = treeCtrl.GetItemText(hChild);
			treeCtrl.DeleteItem(hChild);
			hChild = hChild1;
		}
	}while(hChild != NULL);
	return true;
}

static CString UrlEnFile(CString szUrlEnFileName)
{
	CString ReUrlEnFile = "";
	HANDLE hFile;
	hFile = CreateFile(szUrlEnFileName,			// 文件名称
		GENERIC_READ,						// 读
		FILE_SHARE_READ,					// 共享
		NULL,								// default security
		OPEN_EXISTING,						// 文件必须已经存在。由设备提出要求
		FILE_ATTRIBUTE_NORMAL,				// normal file
		NULL);				// no attr. template
	if (hFile == INVALID_HANDLE_VALUE)
	{
		AfxMessageBox("File read error!");
		return "";
	}
	DWORD dwFileSize = GetFileSize(hFile, NULL);	//检取文件字节大小
	DWORD dwBytesRead;

	char* lpBuffer = new char[dwFileSize+1];
	char* pOut = new char[(dwFileSize*2)+1];//%不要了,所以*2

	if(dwFileSize != INVALID_FILE_SIZE)				//文件大小有效
	{
		ReadFile(hFile, lpBuffer, dwFileSize, &dwBytesRead, NULL);
		lpBuffer[dwFileSize] = '\0';
		if (dwBytesRead != 0)
		{
			int nFlag = 0;
			BYTE byte = NULL;
			for ( int i = 0; i < (int)dwFileSize; i++ )
			{
				byte = lpBuffer[i];
				pOut[nFlag++] = TOHEX( byte >> 4 );
				pOut[nFlag++] = TOHEX( byte % 16 );
			}
			pOut[nFlag] = '\0';
		}
	}
	if (hFile)
	{
		CloseHandle(hFile);
	}
	ReUrlEnFile.Format("%s", pOut);
	delete[] lpBuffer;
	delete[] pOut;
	return ReUrlEnFile;
}

static CString RandIP()
{
	srand(GetTickCount());//随机IP，都一样

	CString szRandIP;
	int t1,t2,t3,t4;
	t1 = rand()%255+1;
	t2 = rand()%255+1;
	t3 = rand()%255+1;
	t4 = rand()%255+1;
	szRandIP.Format("%d.%d.%d.%d",t1,t2,t3,t4);
	return szRandIP;
}

//////////////////////////////////////////////////////////////////////////
static bool Http_GP(bool bIsHttps,
	CString szGetPost,//GET POST
	CString szHost , CString szPath,
	CString szData , CString szCookie,
	int nPort,
	bool bHeader,	//要头部吗
	CString &strContent, CString &strErrMsg
	)
{
	DWORD m_Flags = 0;
	CString sz_Referer;
	if (bIsHttps)
	{
		m_Flags = INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_COOKIES|
			INTERNET_FLAG_SECURE|
			INTERNET_FLAG_IGNORE_CERT_CN_INVALID|
			INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
		sz_Referer = "https://";
	}else{
		m_Flags = INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_COOKIES;
		sz_Referer = "http://";
	}
	CString m_Header =	Ini_szHeaders;
	if (Ini_bReferer)
	{
		sz_Referer = sz_Referer + szHost;
		m_Header += "Referer: "+ sz_Referer +"/\r\n";
	}
	if (Ini_bX_Forwarded_For)
	{
		m_Header += "X-Forwarded-For: "+ RandIP() +"\r\n";
	}
	//////////////////////////////////////////////////////////////////////////
	HINTERNET m_hInternet;
	HINTERNET m_hSession;
	HINTERNET m_hRequest;

	try {	
		m_hInternet = InternetOpen(Ini_szUser_Agent, INTERNET_OPEN_TYPE_PRECONFIG , NULL, NULL, 0);
		if (!m_hInternet) {
			strErrMsg = "Cannot open internet";//m_lastErrorCode = GetLastError();
			return false;
		}

		m_hSession = InternetConnect(m_hInternet, szHost, nPort, NULL, NULL, INTERNET_SERVICE_HTTP,	0,0);
		if (!m_hSession) {			
			strErrMsg = "Cannot connect to internet";//m_lastErrorCode = GetLastError();
			if (m_hInternet) {
				InternetCloseHandle(m_hInternet);
				m_hInternet = NULL;
			}
			return false;
		}
		m_hRequest = HttpOpenRequest(m_hSession, szGetPost, szPath, NULL, "", NULL, m_Flags, 0);
		if (!m_hRequest) {
			strErrMsg = "Cannot perform http request";//m_lastErrorCode = GetLastError();
			if (m_hInternet) {
				InternetCloseHandle(m_hInternet);
				m_hInternet = NULL;
			}
			if (m_hSession) {
				InternetCloseHandle(m_hSession);
				m_hSession = NULL;
			}
			return false;
		}
	}catch(...) {
		strErrMsg = "Memory Exception occured";//m_lastErrorCode = GetLastError();
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	try {
		if (bIsHttps)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (m_hRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (m_hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof (dwFlags) );
		}
		BOOL BSendRequest = FALSE;
		if (szGetPost == "GET")
		{
			BSendRequest = HttpSendRequest(m_hRequest, m_Header, m_Header.GetLength(), NULL, 0);
		}else{
			BSendRequest = HttpSendRequest(m_hRequest, m_Header, m_Header.GetLength(), szData.GetBuffer(0), szData.GetLength());
		}
		if ( BSendRequest )
		{
			//return true;
		}else{
			switch( GetLastError() )
			{
			case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
				{
					//if (!SetClientCert()) {
						strErrMsg = "Cannot perform http request, client authentication needed but couldnt detect required client certificate";
						//m_lastErrorCode = GetLastError();
						if (m_hInternet) {
							InternetCloseHandle(m_hInternet);
							m_hInternet = NULL;
						}
						if (m_hSession) {
							InternetCloseHandle(m_hSession);
							m_hSession = NULL;
						}
						if (m_hRequest) {
							InternetCloseHandle(m_hRequest);
							m_hRequest = NULL;
						}
						return false;
					//}
					break;
				}
			case ERROR_INTERNET_INVALID_CA:
				{
					strErrMsg = "Cannot perform http request, client authentication needed, invalid client certificate is used";
					//m_lastErrorCode = GetLastError();
					if (m_hInternet) {
						InternetCloseHandle(m_hInternet);
						m_hInternet = NULL;
					}
					if (m_hSession) {
						InternetCloseHandle(m_hSession);
						m_hSession = NULL;
					}
					if (m_hRequest) {
						InternetCloseHandle(m_hRequest);
						m_hRequest = NULL;
					}
					return false;
					break;
				}
			default:
				{
					strErrMsg = "Cannot perform http request";
					//m_lastErrorCode = GetLastError();
					if (m_hInternet) {
						InternetCloseHandle(m_hInternet);
						m_hInternet = NULL;
					}
					if (m_hSession) {
						InternetCloseHandle(m_hSession);
						m_hSession = NULL;
					}
					if (m_hRequest) {
						InternetCloseHandle(m_hRequest);
						m_hRequest = NULL;
					}
					return false;
					break;
				}
			}
		}
	}catch(...) {
		strErrMsg = "Memory Exception occured";	//m_lastErrorCode = GetLastError();
		if (m_hInternet) {
			InternetCloseHandle(m_hInternet);
			m_hInternet = NULL;
		}
		if (m_hSession) {
			InternetCloseHandle(m_hSession);
			m_hSession = NULL;
		}
		if (m_hRequest) {
			InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	strContent = "";
	char buffer[1024];
	memset(buffer, 0, 1024);
	if (bHeader)
	{
		DWORD nLength = 1024;//这一句很重要，，因为HttpQueryInfo返回的时候，会改变此值
		HttpQueryInfo(m_hRequest, HTTP_QUERY_RAW_HEADERS_CRLF, buffer,(unsigned long *)&nLength, NULL);
		
		strContent.Format("%s", buffer);
		memset(buffer, 0, 1024);
	}
	//////////////////////////////////////////////////////////////////////////
	DWORD dwNumberOfBytesRead;
	BOOL result; 
	while(true)
	{
		result = InternetReadFile(m_hRequest, buffer, 1023, &dwNumberOfBytesRead);
		if (dwNumberOfBytesRead == 0 || !result)
		{
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		for (int i_1=0; i_1<=1023; i_1++)
		{
			if (buffer[i_1] == '\0')
			{
				buffer[i_1] = ' ';
			}
		}
		//////////////////////////////////////////////////////////////////////////
		buffer[dwNumberOfBytesRead] = '\0';
		strContent += buffer;
		memset(buffer, 0, 1024);
	}
	//////////////////////////////////////////////////////////////////////////
	if (m_hInternet) {
		InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
	}
	if (m_hSession) {
		InternetCloseHandle(m_hSession);
		m_hSession = NULL;
	}
	if (m_hRequest) {
		InternetCloseHandle(m_hRequest);
		m_hRequest = NULL;
	}
	return true;
}

static bool HttpDown_GP(bool bIsHttps,
	CString szGetPost,//GET POST
	CString szHost , CString szPath,
	CString szData , CString szCookie,
	int nPort,
	bool bHeader,	//要头部吗
	CString szSaveFile, CString &strErrMsg
	)
{
	CString sz_Referer;
	DWORD m_Flags = 0;
	if (bIsHttps)
	{
		m_Flags = INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_COOKIES|
			INTERNET_FLAG_SECURE|
			INTERNET_FLAG_IGNORE_CERT_CN_INVALID|
			INTERNET_FLAG_IGNORE_CERT_DATE_INVALID;
		sz_Referer = "https://";
	}else{
		m_Flags = INTERNET_FLAG_RELOAD|INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_NO_COOKIES;
		sz_Referer = "http://";
	}
	/*
	sz_Referer = sz_Referer + szHost;
	CString m_Header =	"Referer: "+ sz_Referer +"/\r\n"
		"Accept-Language: en-us\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"X-Forwarded-For: "+ RandIP() +"\r\n";
	*/
	CString m_Header =	Ini_szHeaders;
	if (Ini_bReferer)
	{
		sz_Referer = sz_Referer + szHost;
		m_Header += "Referer: "+ sz_Referer +"/\r\n";
	}
	if (Ini_bX_Forwarded_For)
	{
		m_Header += "X-Forwarded-For: "+ RandIP() +"\r\n";
	}
	//////////////////////////////////////////////////////////////////////////
	HINTERNET m_hInternet;
	HINTERNET m_hSession;
	HINTERNET m_hRequest;

	try {	
		m_hInternet = InternetOpen(Ini_szUser_Agent, INTERNET_OPEN_TYPE_PRECONFIG , NULL, NULL, 0);
		if (!m_hInternet) {
			strErrMsg = "Cannot open internet";//m_lastErrorCode = GetLastError();
			return false;
		}

		m_hSession = InternetConnect(m_hInternet, szHost, nPort, NULL, NULL, INTERNET_SERVICE_HTTP,	0,0);
		if (!m_hSession) {			
			strErrMsg = "Cannot connect to internet";//m_lastErrorCode = GetLastError();
			if (m_hInternet) {
				InternetCloseHandle(m_hInternet);
				m_hInternet = NULL;
			}
			return false;
		}
		m_hRequest = HttpOpenRequest(m_hSession, szGetPost, szPath, NULL, "", NULL, m_Flags, 0);
		if (!m_hRequest) {
			strErrMsg = "Cannot perform http request";//m_lastErrorCode = GetLastError();
			if (m_hInternet) {
				InternetCloseHandle(m_hInternet);
				m_hInternet = NULL;
			}
			if (m_hSession) {
				InternetCloseHandle(m_hSession);
				m_hSession = NULL;
			}
			return false;
		}
	}catch(...) {
		strErrMsg = "Memory Exception occured";//m_lastErrorCode = GetLastError();
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	try {
		if (bIsHttps)
		{
			DWORD dwFlags;
			DWORD dwBuffLen = sizeof(dwFlags);
			InternetQueryOption (m_hRequest, INTERNET_OPTION_SECURITY_FLAGS, (LPVOID)&dwFlags, &dwBuffLen);
			dwFlags |= SECURITY_FLAG_IGNORE_UNKNOWN_CA;
			InternetSetOption (m_hRequest, INTERNET_OPTION_SECURITY_FLAGS, &dwFlags, sizeof (dwFlags) );
		}
		BOOL BSendRequest = FALSE;
		if (szGetPost == "GET")
		{
			BSendRequest = HttpSendRequest(m_hRequest, m_Header, m_Header.GetLength(), NULL, 0);
		}else{
			BSendRequest = HttpSendRequest(m_hRequest, m_Header, m_Header.GetLength(), szData.GetBuffer(0), szData.GetLength());
		}
		if ( BSendRequest )
		{
			//return true;
		}else{
			switch( GetLastError() )
			{
			case ERROR_INTERNET_CLIENT_AUTH_CERT_NEEDED:
				{
					//if (!SetClientCert()) {
					strErrMsg = "Cannot perform http request, client authentication needed but couldnt detect required client certificate";
					//m_lastErrorCode = GetLastError();
					if (m_hInternet) {
						InternetCloseHandle(m_hInternet);
						m_hInternet = NULL;
					}
					if (m_hSession) {
						InternetCloseHandle(m_hSession);
						m_hSession = NULL;
					}
					if (m_hRequest) {
						InternetCloseHandle(m_hRequest);
						m_hRequest = NULL;
					}
					return false;
					//}
					break;
				}
			case ERROR_INTERNET_INVALID_CA:
				{
					strErrMsg = "Cannot perform http request, client authentication needed, invalid client certificate is used";
					//m_lastErrorCode = GetLastError();
					if (m_hInternet) {
						InternetCloseHandle(m_hInternet);
						m_hInternet = NULL;
					}
					if (m_hSession) {
						InternetCloseHandle(m_hSession);
						m_hSession = NULL;
					}
					if (m_hRequest) {
						InternetCloseHandle(m_hRequest);
						m_hRequest = NULL;
					}
					return false;
					break;
				}
			default:
				{
					strErrMsg = "Cannot perform http request";
					//m_lastErrorCode = GetLastError();
					if (m_hInternet) {
						InternetCloseHandle(m_hInternet);
						m_hInternet = NULL;
					}
					if (m_hSession) {
						InternetCloseHandle(m_hSession);
						m_hSession = NULL;
					}
					if (m_hRequest) {
						InternetCloseHandle(m_hRequest);
						m_hRequest = NULL;
					}
					return false;
					break;
				}
			}
		}
	}catch(...) {
		strErrMsg = "Memory Exception occured";
		//m_lastErrorCode = GetLastError();
		if (m_hInternet) {
			InternetCloseHandle(m_hInternet);
			m_hInternet = NULL;
		}
		if (m_hSession) {
			InternetCloseHandle(m_hSession);
			m_hSession = NULL;
		}
		if (m_hRequest) {
			InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	HANDLE fp = CreateFile(szSaveFile, GENERIC_WRITE,0,0,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,0);  //保存文件
	if (fp == INVALID_HANDLE_VALUE)
	{
		strErrMsg = "Create File failed!";
		if (m_hInternet) {
			InternetCloseHandle(m_hInternet);
			m_hInternet = NULL;
		}
		if (m_hSession) {
			InternetCloseHandle(m_hSession);
			m_hSession = NULL;
		}
		if (m_hRequest) {
			InternetCloseHandle(m_hRequest);
			m_hRequest = NULL;
		}
		return false;
	}
	DWORD dwNumberOfBytesRead;
	char buffer[1024];
	memset(buffer, 0, 1024);
	DWORD dwWritten;
	BOOL result;
	BOOL BWrite;//写入成功吗？
	//////////////////////////////////////////////////////////////////////////
	int nReaded = 0;//已读取个数
	int nWrite = 0; //要写入个数
	int nStart = 0;
	int bStart = 0;	//0 没找到
					//1 找到一位
					//2 找到二位
					//大于2，找到了
	bool bIsFind = false;
	char buffer2[1024];
	memset(buffer2, 0, 1024);
	//////////////////////////////////////////////////////////////////////////
	while(true)
	{
		result = InternetReadFile(m_hRequest, buffer, 1023, &dwNumberOfBytesRead);
		if (dwNumberOfBytesRead == 0 || !result || dwNumberOfBytesRead <= 3)//后面三个不要了
		{
			break;
		}
		//////////////////////////////////////////////////////////////////////////
		nReaded = nReaded + dwNumberOfBytesRead;//已接收
		//printf("本页面: %d\n已接收: %d\n一共: %d\n", dwNumberOfBytesRead, nReaded, dwContentLen);
		CString sz_edit;
		sz_edit.Format("Received %d bytes", nReaded);
		m_edit.SetWindowText( sz_edit );					//设置状态栏

		if (dwNumberOfBytesRead <= 1023 && 
			buffer[dwNumberOfBytesRead-3] == '|' &&
			buffer[dwNumberOfBytesRead-2] == '<' &&
			buffer[dwNumberOfBytesRead-1] == '-')										//有BUG，被截断就出问题了。
		{
			nWrite = dwNumberOfBytesRead-3;
		}else{
			nWrite = dwNumberOfBytesRead;
		}


		if (bStart == 0)//0 没找到
		{
			for (int i =0; i < 1023; i++)
			{
				if (buffer[i] == '-' && buffer[i+1] == '>' && buffer[i+2] == '|')
				{
					nStart = i+3;
					bStart = 3;//大于2，找到了
					bIsFind = true;
					//printf("Find: %d\n", i);
				}
			}
		}else if (bStart == 1)//1 找到一位
		{
			if (buffer[0] == '>' && buffer[1] == '|')
			{
				//printf("找到一位\n");
				nStart = 2;
				bStart = 3;//大于2，找到了
				bIsFind = true;
			}
		}else if (bStart == 2)//2 找到二位
		{
			if (buffer[0] == '|')
			{
				//printf("找到二位\n");
				nStart = 1;
				bStart = 3;//大于2，找到了
				bIsFind = true;
			}
		}
		if (!bIsFind)
		{
			if (buffer[1021] == '-' && buffer[1022] == '>')
			{
				bStart = 2;//2 找到二位
				//printf("1021 1022 = ->\n");
			}
			if (buffer[1022] == '-')
			{
				bStart = 1;//1 找到一位
				//printf("1022 = >\n");
			}
		}
		if (bIsFind && bStart == 3)
		{
			//printf("nStart: %d\n", nStart);
			int j = 0;
			for(int i =nStart; i < 1023; i++)
			{
				buffer2[j++] = buffer[i];
			}
			//printf("%d\n", nWrite-nStart);
			BWrite = WriteFile(fp, buffer2, nWrite-nStart, &dwWritten, NULL);

			memset(buffer2, 0, 1024);
			bStart = 4;
		}else if (bIsFind)
		{
			BWrite = WriteFile(fp, buffer, nWrite, &dwWritten, NULL);
		}

		//////////////////////////////////////////////////////////////////////////
		if ( BWrite == 0)
		{
			CloseHandle(fp);
			strErrMsg = "WriteFile File failed!";
			if (m_hInternet) {
				InternetCloseHandle(m_hInternet);
				m_hInternet = NULL;
			}
			if (m_hSession) {
				InternetCloseHandle(m_hSession);
				m_hSession = NULL;
			}
			if (m_hRequest) {
				InternetCloseHandle(m_hRequest);
				m_hRequest = NULL;
			}
			return false;
		}
		memset(buffer, 0, 1024);
	}
	//////////////////////////////////////////////////////////////////////////
	CloseHandle(fp);
	if (m_hInternet) {
		InternetCloseHandle(m_hInternet);
		m_hInternet = NULL;
	}
	if (m_hSession) {
		InternetCloseHandle(m_hSession);
		m_hSession = NULL;
	}
	if (m_hRequest) {
		InternetCloseHandle(m_hRequest);
		m_hRequest = NULL;
	}
	if (!bIsFind)
	{
		strErrMsg = "File read failed!";
		return false;
	}
	return true;
}

static CString RandURL()
{
	if ( szGlobal_RandType.GetCount() <= 0)
	{
		return "";
	}

	srand(GetTickCount());//随机IP，都一样

	CString szRandURL = "";
	int tid = rand() % (int)szGlobal_RandType.GetCount();
	int id = rand() % 99999 + 1;
	szRandURL.Format("/%s/%d.html", szGlobal_RandType.GetAt( tid ), id);

	/*
	http://192.168.1.250/1.php/index-view-aid-615.html
	http://www.wuhanopen.org/index.php/index-show-tid-53.html
	http://www.wuhanopen.org/index.php/index-show-tid-11.html
	http://www.kuimao.net/index.php/Lightning/light_article/goods_id/133863.html
	http://www.bskcare.com/index.php/GroupBuy/index.html
	*/

	return szRandURL;
}

static bool MyHttp(CString szUrl, CString szPost, int nSiteCode, CString &szConten, CString &szError)
{
	szConten	= "";
	szError		= "";
	CString aaa="",bbb="",szHost="",szPort="",szPath="";
	bool bIsSSL = UrlExplode(szUrl, szHost, szPort, szPath);

	bool bIsInRandTypeExclude = false;
	for (int i=0; i < (int)szGlobal_RandTypeExclude.GetCount(); i++)
	{
		if ( szGlobal_RandTypeExclude.GetAt(i) == szHost)
		{
			bIsInRandTypeExclude = true;
			break;
		}
	}
	if ( !bIsInRandTypeExclude )
	{
		aaa = szPath;
		aaa.MakeLower();
		if (aaa.Find(".php") == aaa.GetLength()-4)
		{
			szPath = szPath + RandURL();
		}else{
			if(aaa.Find("?") != -1)
			{
				szPath = szPath +"&"+ RandURL();
			}else{
				szPath = szPath +"?"+ RandURL();
			}
		}
		aaa = "";
	}
	
	bool bHttp_GP = Http_GP(bIsSSL, "POST", szHost, szPath, szPost, "", atoi(szPort), true, bbb, aaa);
	if (!bHttp_GP)
	{
		szError = aaa;
		return false;
	}

	if (bbb == "")
	{
		szError = bbb + aaa;
		return false;
	}

	
	if (nSiteCode == 950)
	{
		bbb = BIG5ToGB2312( bbb );
	}else if(nSiteCode == 65001)
	{
		bbb = UTF8ToGB2312( bbb );
	}


	int posX = bbb.Find("->|");
	if (posX == -1)
	{
		szError = bbb + aaa;
		return false;
	}
	int posY = bbb.Find("|<-",posX);
	if (posY == -1)
	{
		szError = bbb + aaa;
		return false;
	}

	szConten = bbb.Mid(posX+3,posY-posX-3);
	
	return true;
}