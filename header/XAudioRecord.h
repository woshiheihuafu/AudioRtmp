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
	int channels = 2; //声道数
	int sampleRate = 44100; //样本率
	int sampleByte = 2; //样本字节大小
	int nb_samples = 1024;//一帧音频每个通道的样本数量
	//开始录制
	virtual bool Init() = 0;
	//结束录制
	virtual void stop() = 0;

public:
	static XAudioRecord* GetInstance(XRecordType type = X_QT_AUDIO,unsigned char index = 0);


	virtual ~XAudioRecord();

private:
protected:
	XAudioRecord();
};

