#include "functions.h"
#include <tchar.h>


#ifdef UNICODE

//��TCHARתΪchar   
//*tchar��TCHAR����ָ�룬*_char��char����ָ��   
void TcharToChar (const TCHAR* tchar, char* _char)  
{  
    int iLength;  
	//��ȡ�ֽڳ���   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
	//��tcharֵ����_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}

//charתΪTCHAR   
void CharToTchar (const char* _char, TCHAR* tchar)  
{  
	int iLength;
	iLength = MultiByteToWideChar (CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);  
	MultiByteToWideChar (CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);  
} 

#else

//��TCHARתΪchar   
//*tchar��TCHAR����ָ�룬*_char��char����ָ��   
void TcharToChar (const TCHAR* tchar, char* _char)  
{  
    strcpy(_char, tchar); 
}

//charתΪTCHAR   
void CharToTchar (const char* _char, TCHAR* tchar)  
{  
	strcpy(tchar, (char*)_char);
} 
#endif