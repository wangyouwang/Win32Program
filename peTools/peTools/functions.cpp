#include "functions.h"
#include <tchar.h>


#ifdef UNICODE

//将TCHAR转为char   
//*tchar是TCHAR类型指针，*_char是char类型指针   
void TcharToChar (const TCHAR* tchar, char* _char)  
{  
    int iLength;  
	//获取字节长度   
	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);  
	//将tchar值赋给_char    
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);   
}

//char转为TCHAR   
void CharToTchar (const char* _char, TCHAR* tchar)  
{  
	int iLength;
	iLength = MultiByteToWideChar (CP_ACP, 0, _char, strlen(_char) + 1, NULL, 0);  
	MultiByteToWideChar (CP_ACP, 0, _char, strlen(_char) + 1, tchar, iLength);  
} 

#else

//将TCHAR转为char   
//*tchar是TCHAR类型指针，*_char是char类型指针   
void TcharToChar (const TCHAR* tchar, char* _char)  
{  
    strcpy(_char, tchar); 
}

//char转为TCHAR   
void CharToTchar (const char* _char, TCHAR* tchar)  
{  
	strcpy(tchar, (char*)_char);
} 
#endif