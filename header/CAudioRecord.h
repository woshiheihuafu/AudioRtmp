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
	//�̺߳���
	void run();
	//��ʼ¼��
	bool Init();
	//����¼��
	void stop();
	//
	//XData push();
	//
public:
	QAudioInput* input = NULL;
	QIODevice* io = NULL;
};

