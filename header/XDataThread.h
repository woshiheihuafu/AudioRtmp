#pragma once
#include<list>
#include<qthread.h>
#include"XData.h"
#include<qmutex.h>
class XDataThread:public QThread
{
public:
	//链表最大值，超出删除最旧的数据

	int maxList = 100;
	XDataThread();
public:

	virtual ~XDataThread();
	//列表结尾插入
	virtual void push(XData da);

	//读取旧数据，头部,返回数据需要调用drop清理
	virtual XData pop();

	//开始线程
	virtual bool start();

	//退出线程，并等待贤臣退出
	virtual void stop();

	//清理数据
	virtual void clear();
protected:
	//存放交互数据，先进先出
	std::list<XData> datas;
	//数据长度
	int dataCount = 0;
	//互斥访问 datas
	QMutex mutex;
	//处理线程退出
	bool isRun = false;

};

