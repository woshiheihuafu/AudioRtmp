#pragma once
class AVCodecContext;
#include "XData.h"
class XRtmp
{
public:
	//工厂生产方法
	static XRtmp* GetInstance(unsigned char index = 0);
	//初始化封装其上下文
	virtual bool Init(const char* url) = 0;
	//添加视频或音频流,失败返回-1，成功返回索引流
	virtual int AddStream(const AVCodecContext* c) = 0;
	//打开RTMP网络IO,发送封装头
	virtual bool SendHead() = 0;
	//推流
	virtual bool SendFrame(XData pack,int streamIndex = 0) = 0;

	virtual ~XRtmp();
protected:
	XRtmp();
};

