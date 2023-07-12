#pragma once
#include<list>
#include<qthread.h>
#include"XData.h"
#include<qmutex.h>
class XDataThread:public QThread
{
public:
	//�������ֵ������ɾ����ɵ�����

	int maxList = 100;
	XDataThread();
public:

	virtual ~XDataThread();
	//�б��β����
	virtual void push(XData da);

	//��ȡ�����ݣ�ͷ��,����������Ҫ����drop����
	virtual XData pop();

	//��ʼ�߳�
	virtual bool start();

	//�˳��̣߳����ȴ��ͳ��˳�
	virtual void stop();

	//��������
	virtual void clear();
protected:
	//��Ž������ݣ��Ƚ��ȳ�
	std::list<XData> datas;
	//���ݳ���
	int dataCount = 0;
	//������� datas
	QMutex mutex;
	//�����߳��˳�
	bool isRun = false;

};

