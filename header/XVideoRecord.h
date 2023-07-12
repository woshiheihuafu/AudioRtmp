#pragma once
#include<iostream>
using namespace std;
#include "XDataThread.h"
#include <qthread.h>
#include"XFilter.h"
#include<vector>
class XVideoRecord:public XDataThread
{
public:
	int width = 0;
	int height = 0;
	int fps = 0;
public:
	static XVideoRecord* GetInstance(unsigned char index = 0);

	virtual bool Init(int camIndex = 0) = 0;
	virtual bool Init(const char* url) = 0;
	virtual void stop() = 0;
	inline void AddFilter(XFilter* f)
	{
		fMutex.lock();
		filters.push_back(f);
		fMutex.unlock();
	}
	~XVideoRecord();
protected:
	//
	QMutex fMutex;
	vector<XFilter*> filters;
	XVideoRecord();

};

