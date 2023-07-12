#pragma once
#include "XRtmp.h"
extern"C"
{
#include<libavformat/avformat.h>
}
#include <iostream>
#include <string>
#pragma comment(lib,"avformat.lib")
#include<iostream>
#include <string>
using namespace std;
class CRtmp :public XRtmp
{
public:
	//��ʼ����װ��
	bool Init(const char* url);
	//�ر�
	void close();
	//�����Ƶ����ʧ�ܷ���-1���ɹ�����������
	int AddStream(const AVCodecContext* c);
	//���ͷ�װͷ
	bool SendHead();
	//����
	bool SendFrame(XData pack, int streamIndex);
public:
	//rtmp flv ��װ��
	AVFormatContext* ic = NULL;
	string url;
	//��Ƶ������
	const AVCodecContext* vc = NULL;
	//��Ƶ������
	const AVCodecContext* ac = NULL;
	//��Ƶ��
	AVStream* vs = NULL;
	//��Ƶ��
	AVStream* as = NULL;
};

