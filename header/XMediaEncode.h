#pragma once
#include "XData.h"
//音视频编码接口类
class AVCodecContext;
enum XSampleFormat {

	X_S16 = 1,         ///< signed 16 bits
	X_FLTP = 8       ///< float, planar
};

class XMediaEncode
{
public:
	//输入参数
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;
	int channels = 2;
	int sampleRate = 44100;
	int inSampleFmt = X_S16;

	//输出参数
	int outWidth = 1280;
	int outHeight = 720;
	int bitRate = 50 * 1024 * 8;//比特率
	int outFps = 25;
	int nb_sample = 1024;
	int outSampleFmt = X_FLTP;
public:
	//工厂的生产方法
	static XMediaEncode* GetInstance(unsigned char index = 0);//最多创建255个对象
	//初始化像素格式转换上下文
	virtual bool InitScale() = 0;
	//音视频重采样上下文初始化
	virtual bool InitResample() = 0;
	//视频编码器初始化
	virtual bool InitVideoCodec() = 0;
	//音频编码器初始化
	virtual bool InitAudioCodec() = 0;
	//视频编码
	virtual XData EncodeVideo(XData frame) = 0;
	//音频编码
	virtual XData EncodeAudio(XData frame) = 0;
	//重采样
	virtual XData Resample(XData da) = 0;
	//RGB to YUV
	virtual XData RGBToYUV(XData rgb) = 0;

	virtual ~XMediaEncode();
public:
	AVCodecContext* vc = 0;//视频编码器上下文
	AVCodecContext* ac = 0;//音频编码器上下文
protected:
	XMediaEncode();//限制用户创建对象

};

