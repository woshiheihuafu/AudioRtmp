#pragma once
class XData
{
public:
	char* data = 0;
	int size = 0;
	long long pts = 0;//��ʾʱ���
	void Drop();
	XData();
	//�����ռ䲢���� data������
	XData(char* data,int size,long long p = 0);
	virtual ~XData();
};
//��ȡ��ǰʱ�������΢�룩
long long GetCurTime();
