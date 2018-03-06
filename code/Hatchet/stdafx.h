// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT


#include <afxsock.h>		// MFC socket extensions


#import "c:\Program Files\Common Files\system\ado\msadox.dll"
#import "C:\\Program Files\\Common Files\\System\\ado\\msado15.dll" no_namespace rename("EOF","adoEOF")

#include "CPData.h"
#include "Function.h"

extern	int		nShowTab;					//当前显示的TAB，对比，不用每次都变，放到全局，因为点击左边，没有改变

extern CPData* pMydb;
extern CStringArray MydbArray;

extern CPData* pMydbTmp;
extern CStringArray MydbTmpArray;

extern	bool bCloseTab;//创建TAB，加载管理失败。关闭tab

extern CString szError;//http 失败返回信息
extern bool	bHttpIsOK;//http 是否OK

extern	CString			szGlobal_SiteUrl;							//Shell Url
extern	CString			szGlobal_SiteJoinUrl;							//Shell Join Url
extern	CString			szGlobal_SitePass;							//Shell Pass
extern	int				nGlobal_SiteCode;							//Shell Code 编码
extern	CString			szGlobal_SiteConfig;						//Shell Config
extern	int				nGlobal_SiteScript;							//Shell Script
extern	CString			szGlobal_SiteFileSave;						//保存文件传递文件路径

//////////////////////////////////////////////////////////////////////////
extern	CString			szCmdGetInfo[4];
extern	CString			szCmdGetExec[4];

extern	CString			szFileGetInfo[4];
extern	CString			szFileGetFiles[4];

extern	CString			szFileGetFileContent[4];
extern	CString			szFileSaveFileContent[4];

extern	CString			szFileUpload[4];							//文件上传
extern	CString			szFileDelete[4];							//文件删除
extern	CString			szFileReName[4];							//文件重命名
extern	CString			szFileChangeTime[4];						//修改文件时间
extern	CString			szFileNewDir[4];							//新建文件夹
extern	CString			szFileDownFile[4];							//下载文件
extern	CString			szWget[4];									//WGET

extern	CString			szFileCopy[4];								//文件复制
extern	CString			szFileCut[4];								//文件剪切

extern	CString			szRegistryRead[4];							//注册表读取
extern	CString			szRegistryWrite[4];							//注册表写入
extern	CString			szRegistryDelete[4];						//注册表删除
//////////////////////////////////////////////////////////////////////////		DATA PHP
extern	CString			szDataPhpMysql;
extern	CString			szDataPhpMssql;
extern	CString			szDataPhpOracle;
extern	CString			szDataPhpInformix;
extern	CString			szDataPhpPostgreSQL;
extern	CString			szDataPhpODBC_MSSQL;
extern	CString			szDataPhpPDO_MYSQL;
extern	CString			szDataPhpPDO_MSSQL;
//////////////////////////////////////////////////////////////////////////		DATA ASP
extern	CString			szDataAspSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
extern	CString			szDataAsp1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
extern	CString			szDataAsp2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//////////////////////////////////////////////////////////////////////////		DATA ASPX
extern	CString			szDataAspxSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
extern	CString			szDataAspx1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
extern	CString			szDataAspx2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//////////////////////////////////////////////////////////////////////////

extern	CString			szDataTablePhpMysql;
extern	CString			szDataTablePhpMssql;
extern	CString			szDataTablePhpOracle;
extern	CString			szDataTablePhpInformix;
extern	CString			szDataTablePhpPostgreSQL;
extern	CString			szDataTablePhpODBC_MSSQL;
extern	CString			szDataTablePhpPDO_MYSQL;
extern	CString			szDataTablePhpPDO_MSSQL;
//////////////////////////////////////////////////////////////////////////		Table ASP
extern	CString			szDataTableAspSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
extern	CString			szDataTableAsp1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
extern	CString			szDataTableAsp2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//////////////////////////////////////////////////////////////////////////		Table ASPX
extern	CString			szDataTableAspxSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
extern	CString			szDataTableAspx1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
extern	CString			szDataTableAspx2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//////////////////////////////////////////////////////////////////////////

extern	CString			szDataColumnsPhpMysql;
extern	CString			szDataColumnsPhpMssql;
extern	CString			szDataColumnsPhpOracle;
extern	CString			szDataColumnsPhpInformix;
extern	CString			szDataColumnsPhpPostgreSQL;
extern	CString			szDataColumnsPhpODBC_MSSQL;
extern	CString			szDataColumnsPhpPDO_MYSQL;
extern	CString			szDataColumnsPhpPDO_MSSQL;
//////////////////////////////////////////////////////////////////////////		Columns ASP
extern	CString			szDataColumnsAspSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
extern	CString			szDataColumnsAsp1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
extern	CString			szDataColumnsAsp2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//////////////////////////////////////////////////////////////////////////		Columns ASPX
extern	CString			szDataColumnsAspxSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
extern	CString			szDataColumnsAspx1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
extern	CString			szDataColumnsAspx2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//////////////////////////////////////////////////////////////////////////

extern	CString			szDataExecPhpMysql;
extern	CString			szDataExecPhpMssql;
extern	CString			szDataExecPhpOracle;
extern	CString			szDataExecPhpInformix;
extern	CString			szDataExecPhpPostgreSQL;
extern	CString			szDataExecPhpODBC_MSSQL;
extern	CString			szDataExecPhpPDO_MYSQL;
extern	CString			szDataExecPhpPDO_MSSQL;

//////////////////////////////////////////////////////////////////////////		Exec ASP
extern	CString			szDataExecAspSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
extern	CString			szDataExecAsp1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
extern	CString			szDataExecAsp2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//////////////////////////////////////////////////////////////////////////		Exec ASPX
extern	CString			szDataExecAspxSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
extern	CString			szDataExecAspx1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
extern	CString			szDataExecAspx2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
extern	CString			szProxyGetInfo;
//////////////////////////////////////////////////////////////////////////
extern	CFont			m_FontSample;									//这个要定义成类变量 等宽字体
//////////////////////////////////////////////////////////////////////////

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


