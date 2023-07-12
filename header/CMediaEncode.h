#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <Windows.h>
extern"C"
{
#include<libswresample/swresample.h>
#include<libswscale/swscale.h>
#include<libavcodec/avcodec.h>
#include<libavformat/avformat.h>
}
#include <iostream>
#include <string>
using namespace std;
using namespace cv;
#pragma comment(lib,"opencv_world320d.lib")
#pragma comment(lib,"swscale.lib")
#pragma comment(lib,"avcodec.lib")
#pragma comment(lib,"avformat.lib")
#pragma comment(lib,"avutil.lib")
#pragma comment(lib,"swresample.lib")
#include "XMediaEncode.h"
class CMediaEncode :public XMediaEncode
{
public:
	//初始化像素格式转换上下文
	bool InitScale();
	XData RGBToYUV(XData rgb);
	//视频编码器初始化
	bool InitVideoCodec();
	//音频编码器初始化
	bool InitAudioCodec();
	//视频编码,返回值无需调用者清理
	XData EncodeVideo(XData frame);
	//音频编码,返回值无需调用者清理
	XData EncodeAudio(XData frame);
	//音频重采样上下文初始化
	bool InitResample();
	//重采样
	XData Resample(XData data);
	//关闭
	void close();
public:
private:
	AVCodecContext* CreateCodec(AVCodecID id);
	bool OpenCodec(AVCodecContext** c);
	SwsContext* vsc = NULL;//像素格式转换上下文
	AVFrame* yuv = NULL;//输出的YUV
	AVPacket vPack = { 0 };//视频pack
	AVPacket aPack = { 0 };//音频pack
	int vpts = 0;
	int apts = 0;
	SwrContext* asc = NULL;//音频重采样上下文
	AVFrame* pcm = NULL;//重采样输出pcm
};

