https://github.com/wangyouwang/Win32Program.git


VS2010工程升级到VS2017，需要解决下述问题：

一些升级上来的工程内，rc 文件包含了 afxres.h，vs2015/2017 内又找不到（位置在 C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\atlmfc\include\afxres.h）
可以直接在 rc 文件内将包含 afxres.h 字样的都删除，一共两条，一个在 #undef APSTUDIO_READONLY_SYMBOLS 上面，一个在 “2 TEXTINCLUDE”  内
在最上面加一行 #include <winres.h> 即可，这文件在 C:\Program Files (x86)\Windows Kits\8.1\Include\um\winres.h

