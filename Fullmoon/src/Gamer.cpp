#include "Game.h"
void Game::music(){
	/*
	https://msdn.microsoft.com/en-us/library/dd757161(d=vs.85).aspx
	http://blog.163.com/guobin0517@126/blog/static/50841978201092974353636/
	MCIERROR mciSendString(
	LPCTSTR lpszCommand, 　　　//MCI命令字符串
	LPTSTR　lpszReturnString,　//存放反馈信息的缓冲区
	UINT　　cchReturn, 　　　　//缓冲区的长度
	HANDLE　hwndCallback 　　　//回调窗口的句柄，一般为NULL
	); //若成功则返回0，否则返回错误码。
	//分析mciSendString返回的错误码
	BOOL mciGetErrorString(
	DWORD　fdwError,　　　//函数mciSendString返回的错误码
	LPTSTR lpszErrorText, //接收描述错误的字符串的缓冲区
	UINT　 cchErrorText);　 //缓冲区的大小
	*/
	/*使用MCI API，源文件中需要包含头文件 Mmsystem.h，
	在Project->Settings->Link->Object/libray module中加入库 Winmm.lib。
	VS2008在源文件加上#include "mmsystem.h"
	#pragma comment(lib,"winmm.lib")*/
	mciSendString(_T("open res//cunzhuang.mid Alias Music"), NULL, 0, NULL);/*打开文件*/
	mciSendString(_T("setaudio Music volume to 500"), NULL, 0, NULL);/*设置音量大小*/
	mciSendString(_T("play Music from 0"), NULL, 0, NULL);/*从头开始播放*/
	mciSendString(TEXT("play Music repeat"), NULL, 0, NULL);/*循环播放*/
	//mciSendString("stop Music", NULL, 0, NULL);/*停止*/
	//mciSendString("Game::close Music", NULL, 0, NULL);/*关闭*/
}