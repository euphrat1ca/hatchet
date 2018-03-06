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
#include "StdAfx.h"
/*****************************************************************
CPFile.h
******************************************************************/
class CPData  
{
public:
    
    //默认初始化构造函数
    CPData();
    
    //传入连接的构造函数
    CPData(_ConnectionPtr pConnection);
    
    //析构函数
    virtual ~CPData();
public:
    
	BOOL Create(CString strFileName);

    //连接数据库
	BOOL Connect(CString strFileName);
    
    //关闭数据库的连接
    void DisConnect();
    
	//判断字段存在
	bool CheckFields(CString strSql);

    //数据库查询语句，用来对数据库的各种字段进行查询
    //如果成功返回TRUE,否则返回FALSE.查询的结果存储在类的共有成员变量m_pRecordset中
    //查询结果的行数和列数分别保存在共有成员变量m_nResultRow和m_nResultCol中
    int Select(CString strSql);
    
    //查询语句，负责对仅仅查询一个字段的情况进行处理
    //结果存放在CStringArray类型的变量pResult中
    int Select(CString strSql,CStringArray& Result);
    
    //对多个字段进行查询
    BOOL SelectMulitCol(CString strSql,CStringArray& Result);
    
    //打开表
    BOOL OpenTable(CString strTable);
    
    //打开表
    BOOL OpenTable(CString strTable,CStringArray& Result, int& nCol);
    
    //进行其它的更新操作
    BOOL Execute(CString strSql);
public:
    BOOL ExecuteTrans(CStringArray& aSql);    
    
    //关闭结果集合
    void CloseRecordset();
    
    //得到操作结果的列数
    long GetResultCol();
    
    //得到操作结果的条数
    long GetResultRow();
    
    //得到操作结果
    _RecordsetPtr GetResult();
private:
    
    //数据库操作返回的结果条数
    long m_nResultRow;
    
    //返回的_RecordsetPtr中列数
    long m_nResultCol;
    
    //连接指针
    _ConnectionPtr m_pConnection;
    
    //命令执行指针
    _CommandPtr m_pCommand;
    
    //结果集指针
    _RecordsetPtr m_pRecordset;
};