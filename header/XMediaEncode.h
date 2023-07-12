#pragma once
#include "XData.h"
//����Ƶ����ӿ���
class AVCodecContext;
enum XSampleFormat {

	X_S16 = 1,         ///< signed 16 bits
	X_FLTP = 8       ///< float, planar
};

class XMediaEncode
{
public:
	//�������
	int inWidth = 1280;
	int inHeight = 720;
	int inPixSize = 3;
	int channels = 2;
	int sampleRate = 44100;
	int inSampleFmt = X_S16;

	//�������
	int outWidth = 1280;
	int outHeight = 720;
	int bitRate = 50 * 1024 * 8;//������
	int outFps = 25;
	int nb_sample = 1024;
	int outSampleFmt = X_FLTP;
public:
	//��������������
	static XMediaEncode* GetInstance(unsigned char index = 0);//��ഴ��255������
	//��ʼ�����ظ�ʽת��������
	virtual bool InitScale() = 0;
	//����Ƶ�ز��������ĳ�ʼ��
	virtual bool InitResample() = 0;
	//��Ƶ��������ʼ��
	virtual bool InitVideoCodec() = 0;
	//��Ƶ��������ʼ��
	virtual bool InitAudioCodec() = 0;
	//��Ƶ����
	virtual XData EncodeVideo(XData frame) = 0;
	//��Ƶ����
	virtual XData EncodeAudio(XData frame) = 0;
	//�ز���
	virtual XData Resample(XData da) = 0;
	//RGB to YUV
	virtual XData RGBToYUV(XData rgb) = 0;

	virtual ~XMediaEncode();
public:
	AVCodecContext* vc = 0;//��Ƶ������������
	AVCodecContext* ac = 0;//��Ƶ������������
protected:
	XMediaEncode();//�����û���������

};

