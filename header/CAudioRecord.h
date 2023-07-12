#pragma once
#include "XAudioRecord.h"
#include <QtCore/QCoreApplication>
#include <qaudioinput.h>
#include <qthread.h>
#include <qmutex.h>
#include <iostream>
#include <list>
using namespace std;

class CAudioRecord :public XAudioRecord
{
public:
	CAudioRecord();
	~CAudioRecord();
	//线程函数
	void run();
	//开始录制
	bool Init();
	//结束录制
	void stop();
	//
	//XData push();
	//
public:
	QAudioInput* input = NULL;
	QIODevice* io = NULL;
};

