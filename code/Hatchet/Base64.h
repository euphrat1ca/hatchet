// Base64.h: CBase64 ¿‡∂®“Â.
//
#ifndef _BASE64_H__INCLUDED
#define _BASE64_H__INCLUDED

// class CBase64
class CBase64  
{
public:
	CBase64();
	virtual ~CBase64();

	virtual int Decode( LPCTSTR szDecoding, LPTSTR szOutput );
	virtual CString Encode(LPCTSTR szEncoding,int nSize);

protected:
	void WriteBits( UINT nBits, LPTSTR szOutput, int& lp );
	UINT ReadBits( int* pBitsRead, int& lp );

	int m_nInputSize;
	int m_nBitsRemaining;
	ULONG m_lBitStorage;
	LPCTSTR m_szInput;
	static CString m_sBase64Alphabet;
};

#endif 
