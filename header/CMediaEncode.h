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
	//��ʼ�����ظ�ʽת��������
	bool InitScale();
	XData RGBToYUV(XData rgb);
	//��Ƶ��������ʼ��
	bool InitVideoCodec();
	//��Ƶ��������ʼ��
	bool InitAudioCodec();
	//��Ƶ����,����ֵ�������������
	XData EncodeVideo(XData frame);
	//��Ƶ����,����ֵ�������������
	XData EncodeAudio(XData frame);
	//��Ƶ�ز��������ĳ�ʼ��
	bool InitResample();
	//�ز���
	XData Resample(XData data);
	//�ر�
	void close();
public:
private:
	AVCodecContext* CreateCodec(AVCodecID id);
	bool OpenCodec(AVCodecContext** c);
	SwsContext* vsc = NULL;//���ظ�ʽת��������
	AVFrame* yuv = NULL;//�����YUV
	AVPacket vPack = { 0 };//��Ƶpack
	AVPacket aPack = { 0 };//��Ƶpack
	int vpts = 0;
	int apts = 0;
	SwrContext* asc = NULL;//��Ƶ�ز���������
	AVFrame* pcm = NULL;//�ز������pcm
};

