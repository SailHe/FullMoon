#include "Game.h"
void Game::music(){
	/*
	https://msdn.microsoft.com/en-us/library/dd757161(d=vs.85).aspx
	http://blog.163.com/guobin0517@126/blog/static/50841978201092974353636/
	MCIERROR mciSendString(
	LPCTSTR lpszCommand, ������//MCI�����ַ���
	LPTSTR��lpszReturnString,��//��ŷ�����Ϣ�Ļ�����
	UINT����cchReturn, ��������//�������ĳ���
	HANDLE��hwndCallback ������//�ص����ڵľ����һ��ΪNULL
	); //���ɹ��򷵻�0�����򷵻ش����롣
	//����mciSendString���صĴ�����
	BOOL mciGetErrorString(
	DWORD��fdwError,������//����mciSendString���صĴ�����
	LPTSTR lpszErrorText, //��������������ַ����Ļ�����
	UINT�� cchErrorText);�� //�������Ĵ�С
	*/
	/*ʹ��MCI API��Դ�ļ�����Ҫ����ͷ�ļ� Mmsystem.h��
	��Project->Settings->Link->Object/libray module�м���� Winmm.lib��
	VS2008��Դ�ļ�����#include "mmsystem.h"
	#pragma comment(lib,"winmm.lib")*/
	mciSendString(_T("open res//cunzhuang.mid Alias Music"), NULL, 0, NULL);/*���ļ�*/
	mciSendString(_T("setaudio Music volume to 500"), NULL, 0, NULL);/*����������С*/
	mciSendString(_T("play Music from 0"), NULL, 0, NULL);/*��ͷ��ʼ����*/
	mciSendString(TEXT("play Music repeat"), NULL, 0, NULL);/*ѭ������*/
	//mciSendString("stop Music", NULL, 0, NULL);/*ֹͣ*/
	//mciSendString("Game::close Music", NULL, 0, NULL);/*�ر�*/
}