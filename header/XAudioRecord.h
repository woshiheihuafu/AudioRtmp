#pragma once
#include<iostream>
#include"XDataThread.h"
enum XRecordType
{
	X_QT_AUDIO
};

class XAudioRecord:public XDataThread
{
public:
	//virtual XData push() = 0;
public:
	int channels = 2; //������
	int sampleRate = 44100; //������
	int sampleByte = 2; //�����ֽڴ�С
	int nb_samples = 1024;//һ֡��Ƶÿ��ͨ������������
	//��ʼ¼��
	virtual bool Init() = 0;
	//����¼��
	virtual void stop() = 0;

public:
	static XAudioRecord* GetInstance(XRecordType type = X_QT_AUDIO,unsigned char index = 0);


	virtual ~XAudioRecord();

private:
protected:
	XAudioRecord();
};

