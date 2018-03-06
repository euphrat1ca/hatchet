/******************************************************************
 模块名称：数据库操作类；
 实现功能：提供接口，实现数据的索引，和操作。
 研究人员：长寿梦；
 最后更新：2010-05-12

 预先操作：
 【1】在stdafx.h中添加
    #import "C:\Program Files\Common Files\System\ado\msado15.dll" no_namespace rename("EOF","adoEOF")
 【2】在主进程的InitInstance()中添加
         if(!AfxOleInit())
        {
         AfxMessageBox("OLE初始化错误");
         return FALSE;
         }

 【3】在.cpp文件中要包含"CPFile.h"
*********************************************************************/
/**************************************************************
CPFile.cpp
**************************************************************/
#include "stdafx.h"
//////////////////////////////////////////////////////////////////////
// 构造函数
//////////////////////////////////////////////////////////////////////
//默认的构造函数
CPData::CPData()
{
    //初始化
    m_nResultRow = 0;
    m_nResultCol=0;
    m_pConnection = NULL;
    //创建对象
    m_pRecordset.CreateInstance(_uuidof(Recordset));
    m_pCommand.CreateInstance(_uuidof(Command));
}

//传入参数的构造函数
CPData::CPData(_ConnectionPtr pConnection)
{
    m_pConnection = pConnection;
    m_nResultRow = 0;
    m_nResultCol=0;
    //创建对象
    m_pRecordset.CreateInstance(_uuidof(Recordset));
    m_pCommand.CreateInstance(_uuidof(Command));

}
//////////////////////////////////////////////////////////////////////
// 析构函数
//////////////////////////////////////////////////////////////////////
CPData::~CPData()
{
    if(m_pRecordset->State!=adStateClosed)
        m_pRecordset->Close();
    m_pRecordset = NULL;

    if(m_pCommand->State!=adStateClosed)
        m_pCommand->Release();
    m_pCommand = NULL;

    if(m_pConnection->State!=adStateClosed)
        m_pConnection->Close();
    m_pConnection = NULL;
}

/////////////////////////////////////////////////////////////////////
///简单操作函数
////////////////////////////////////////////////////////////////////

//得到操作结果的行数
long CPData::GetResultRow()
{
    return this->m_nResultRow;
}

//得到操作结果的列数
long CPData::GetResultCol()
{
    return this->m_nResultCol;
}

//得到操作结果
_RecordsetPtr CPData::GetResult()
{
    return this->m_pRecordset;
}

BOOL CPData::Create(CString strFileName)
{
	CoInitialize(NULL);
	HRESULT hr = S_OK;
	
	CString strcnn ( _T("Provider=Microsoft.JET.OLEDB.4.0;Data source ="+ strFileName));
	try
	{
		ADOX::_CatalogPtr m_pCatalog = NULL;
		hr = m_pCatalog.CreateInstance(__uuidof (ADOX::Catalog));
		if(FAILED(hr))
		{
			_com_issue_error(hr);
		}
		else
		{
			m_pCatalog->Create(_bstr_t(strcnn));
		}
	}
	catch(_com_error &e)
	{
		// Notify the user of errors if any.
		AfxMessageBox(e.ErrorMessage());
		return FALSE;
    }
	return TRUE;
}
///////////////////////////////////////////////////////////////////////
///连接操作
///////////////////////////////////////////////////////////////////////

//连接到数据库
//1.连接字符串可以自己构造,也可以从文件中读出
BOOL CPData::Connect(CString strFileName)
{
    try{
        m_pConnection.CreateInstance(__uuidof(Connection));
		_bstr_t strConnect = "Provider=Microsoft.Jet.OLEDB.4.0;Data Source="+ strFileName +";Persist Security Info=False";

        HRESULT hr;

		hr = m_pConnection->Open(strConnect,"","",adModeUnknown);

        //进行连接
        //连接失败
        if(FAILED(hr))
        {
            AfxMessageBox("Connect Error!");
            return FALSE;
        }
    }
    catch(_com_error&e)
    {
        AfxMessageBox(e.Description()+"B");
        return FALSE;
    }
    return TRUE;
}

//断开连接
void CPData::DisConnect()
{
    if(m_pConnection->State!=adStateClosed)
        m_pConnection->Close();
}

///////////////////////////////////////////////////////////////////////
///更新操作
///////////////////////////////////////////////////////////////////////
BOOL CPData::Execute(CString strSql)
{
    try
    {
        _variant_t vNULL;
        vNULL.vt = VT_ERROR;

        ///定义为无参数
        vNULL.scode = DISP_E_PARAMNOTFOUND;

        ///非常关键的一句，将建立的连接赋值给它
        m_pCommand->ActiveConnection = m_pConnection;

        ///命令字串
        m_pCommand->CommandText = (_bstr_t)strSql;

        ///执行命令，取得记录集
        m_pRecordset = m_pCommand->Execute(&vNULL,&vNULL,adCmdText);

        //确实，vNULL中的intVal是执行操作所影响的行数
        m_nResultRow = 0;
        m_nResultRow = vNULL.intVal;
    }
    catch(_com_error e)
    {
        m_nResultRow = 0;
        return FALSE;
    }
    return TRUE;
}


//判断字段存在
bool CPData::CheckFields(CString strSql)
{
	try
	{
		m_pRecordset->CursorLocation=adUseClient;    //设置游标位置,设置为客户端形式,否则GetRecordCount()返回值不对
		m_pRecordset->Open(_variant_t(strSql),_variant_t((IDispatch *)m_pConnection,true),adOpenDynamic,adLockOptimistic,adCmdText);
		m_pRecordset->Close();
	}
	catch(_com_error)
	{
		return false;
	}
	return true;
}

///////////////////////////////////////////////////////////////////////
///查询操作
///////////////////////////////////////////////////////////////////////
int CPData::Select(CString strSql)
{
    try
    {
        m_nResultCol=0;
        m_nResultRow=0;
        m_pRecordset->CursorLocation=adUseClient;    //设置游标位置,设置为客户端形式,否则GetRecordCount()返回值不对
        m_pRecordset->Open(_variant_t(strSql),_variant_t((IDispatch *)m_pConnection,true),adOpenDynamic,adLockOptimistic,adCmdText);
        m_nResultCol = m_pRecordset->Fields->GetCount();//得到查询结果的列数
        m_nResultRow = m_pRecordset->GetRecordCount();  //得到查询结果的行数
    }
    catch(_com_error&e)
    {
        AfxMessageBox(e.Description()+"D");
        return -1;
    }
    return 1;
}

//查询语句，负责对仅仅查询一个字段的情况进行处理
//结果存放在CStringArray类型的变量pResult中
int CPData::Select(CString strSql,CStringArray& Result)
{
    if(Select(strSql)!=0)
    {
        Result.RemoveAll();
        _variant_t value;
        for(int i=0;i<m_nResultRow;i++)
        {
            for(int j=0;j<m_nResultCol;j++)
            {
                value=m_pRecordset->Fields->Item[(long)(/*i*m_nResultCol+*/j)]->Value;
                if(value.vt==3||value.vt==14)
                {
                    CString strTrans;
                    strTrans.Format("%ld",value.intVal);
                    Result.Add(strTrans);
                }else if(value.vt==7)
                {
					COleDateTime time = value.date;
                    CString strTemp;
                    strTemp.Format("%d-%d-%d %s",time.GetYear(),time.GetMonth(),time.GetDay(),time.Format("%H:%M:%S"));
                    Result.Add(strTemp);
                }else{
					CString strTemp(value.bstrVal);
					Result.Add( strTemp );
				}
            }
            m_pRecordset->MoveNext();
        }
        m_pRecordset->Close();
        return m_nResultCol;
    }
    else
    {
        m_pRecordset->Close();
        return -1;
    }
}

BOOL CPData::SelectMulitCol(CString strSql,CStringArray& Result)
{
    if(Select(strSql)!=0)
    {
        Result.RemoveAll();
        _variant_t value;
        for(int i=0;i<m_nResultRow;i++)
        {
            for(int j=0;j<m_nResultCol;j++)
            {
                value=m_pRecordset->Fields->Item[(long)(/*i*m_nResultCol+*/j)]->Value;
                if(value.vt==3||value.vt==14)
                {
                    CString strTrans;
                    strTrans.Format("%ld",value.intVal);
                    Result.Add(strTrans);
                }
                else
                    if(value.vt==7)
                    {
                        COleDateTime time = value.date;
                        CString strTemp;
                        strTemp.Format("%d-%d-%d %s",time.GetYear(),time.GetMonth(),time.GetDay(),time.Format("%H:%M:%S"));
                        Result.Add(strTemp);
                    }
                    else
					{
						CString strTemp(value.bstrVal);
						Result.Add( strTemp );
					}
            }
            m_pRecordset->MoveNext();
        }
        m_pRecordset->Close();
        return TRUE;
    }
    else
    {
        m_pRecordset->Close();
        return FALSE;
    }
}

//打开整张表
BOOL CPData::OpenTable(CString strTable)
{
    try
    {
        m_nResultCol=0;
        m_nResultRow=0;
        m_pRecordset->CursorLocation=adUseClient;    //设置游标位置,设置为客户端形式,否则GetRecordCount()返回值不对
        m_pRecordset->Open(_variant_t(strTable),_variant_t((IDispatch *)m_pConnection,true),adOpenDynamic,adLockOptimistic,adCmdTable);
        m_nResultCol = m_pRecordset->Fields->GetCount();//得到查询结果的列数
        m_nResultRow = m_pRecordset->GetRecordCount();  //得到查询结果的行数
    }
    catch(_com_error&e)
    {
        AfxMessageBox(e.Description()+"E");
        return FALSE;
    }
    return TRUE;
}

BOOL CPData::OpenTable(CString strTable,CStringArray& Result, int& nCol)
{
    if(OpenTable(strTable)!=0)
    {
        Result.RemoveAll();
        _variant_t value;
        for(int i=0;i<m_nResultRow;i++)
        {
            for(int j=0;j<m_nResultCol;j++)
            {
                value=m_pRecordset->Fields->Item[(long)(/*i*m_nResultCol+*/j)]->Value;
                if(value.vt==3||value.vt==14)
                {
                    CString strTrans;
                    strTrans.Format("%ld",value.intVal);
                    Result.Add(strTrans);
                }
                else
                    if(value.vt==7)
                    {
                        COleDateTime time = value.date;
                        CString strTemp;
                        strTemp.Format("%d-%d-%d %s",time.GetYear(),time.GetMonth(),time.GetDay(),time.Format("%H:%M:%S"));
                        Result.Add(strTemp);
                    }
                    else
					{
						CString strTemp(value.bstrVal);
						Result.Add( strTemp );
					}
            }
            m_pRecordset->MoveNext();
        }
        m_pRecordset->Close();
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/////////////////////////////////////////////////////////////////////////////
///关闭结果集
/////////////////////////////////////////////////////////////////////////////
void CPData::CloseRecordset()
{
    if(m_pRecordset->State != adStateClosed)
        m_pRecordset->Close();
}
BOOL CPData::ExecuteTrans(CStringArray& aSql)
{
    try{
        INT_PTR nNum = aSql.GetSize();
        m_pConnection->BeginTrans();
        for(int i=0;i<nNum;i++)
        {
            CString strSql = aSql.GetAt(i);
            m_pConnection->Execute((_bstr_t)aSql.GetAt(i),NULL,adCmdText);
        }
        m_pConnection->CommitTrans();
        return TRUE;
    }
    catch(_com_error& e)
    {
        m_pConnection->RollbackTrans();
        AfxMessageBox(e.Description()+"F");
        return FALSE;
    }
}
