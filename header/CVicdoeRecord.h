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
	//线程函数
	void run();
	//系统相机初始化
	bool Init(int camIndex = 0);
	//rtsp相机初始化
	bool Init(const char* url);
	//关闭相机
	void stop();
	//清理空间
	CVicdoeRecord();
	~CVicdoeRecord();
};

