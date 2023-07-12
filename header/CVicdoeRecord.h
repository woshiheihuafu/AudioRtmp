#pragma once
#include"XVideoRecord.h"
#include <Windows.h>
#include<opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include<iostream>
#pragma comment(lib,"opencv_world320d.lib")
using namespace std;
using namespace cv;
class CVicdoeRecord :public XVideoRecord
{
public:
	VideoCapture cam;
public:
	//�̺߳���
	void run();
	//ϵͳ�����ʼ��
	bool Init(int camIndex = 0);
	//rtsp�����ʼ��
	bool Init(const char* url);
	//�ر����
	void stop();
	//����ռ�
	CVicdoeRecord();
	~CVicdoeRecord();
};

