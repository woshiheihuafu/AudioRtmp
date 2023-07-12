#pragma once
class AVCodecContext;
#include "XData.h"
class XRtmp
{
public:
	//������������
	static XRtmp* GetInstance(unsigned char index = 0);
	//��ʼ����װ��������
	virtual bool Init(const char* url) = 0;
	//�����Ƶ����Ƶ��,ʧ�ܷ���-1���ɹ�����������
	virtual int AddStream(const AVCodecContext* c) = 0;
	//��RTMP����IO,���ͷ�װͷ
	virtual bool SendHead() = 0;
	//����
	virtual bool SendFrame(XData pack,int streamIndex = 0) = 0;

	virtual ~XRtmp();
protected:
	XRtmp();
};

