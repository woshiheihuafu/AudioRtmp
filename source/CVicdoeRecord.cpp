#include "CVicdoeRecord.h"
cv::Mat get_window_bits(HWND h)
{
	cv::Mat result;
	HBITMAP bit = nullptr;
	RECT rect{ 0 };
	int width = 0, height = 0;
	HDC hdc = nullptr, hdc_compatible = nullptr;
	BITMAP info{ 0 };
	int channels = 0;
	if (h == nullptr)
		return result;

	do
	{
		if (GetWindowRect(h, &rect) == FALSE)
			break;

		if (rect.left < 0)
			rect.left = 0;

		if (rect.top < 0)
			rect.top = 0;

		width = rect.right - rect.left;
		height = rect.bottom - rect.top;

		hdc = GetWindowDC(h);
		if (hdc == nullptr)
			break;

		hdc_compatible = CreateCompatibleDC(hdc);
		if (hdc_compatible == nullptr)
			break;

		bit = CreateCompatibleBitmap(hdc, width, height);
		if (bit == nullptr)
			break;

		SelectObject(hdc_compatible, bit);

		if (BitBlt(hdc_compatible, 0, 0, width, height, hdc, 0, 0, SRCCOPY) == FALSE)
			break;

		if (GetObjectA(bit, sizeof(BITMAP), &info) == 0)
			break;

		channels = info.bmBitsPixel == 1 ? 1 : info.bmBitsPixel / 8;
		result.create(cv::Size(info.bmWidth, info.bmHeight), CV_MAKETYPE(CV_8U, channels));
		if (GetBitmapBits(bit, info.bmHeight * info.bmWidth * channels, result.data) == 0)
			break;
	} while (false);

	if (hdc)
		ReleaseDC(h, hdc);

	if (hdc_compatible)
		DeleteDC(hdc_compatible);

	if (bit)
		DeleteObject(bit);

	return result;
}

cv::Mat screen_shot_by_window(HWND h)
{
	while (1)
	{
		return get_window_bits(h);
	}

}

cv::Mat screen_shot_by_window(std::string name)
{
	while (1)
	{
		return get_window_bits(FindWindowA(0, name.c_str()));
	}

}
void CVicdoeRecord::run()
{
	cout << "进入视频录制线程!" << endl;
	Mat frame;
	while (isRun)
	{
		//frame = screen_shot_by_window(GetDesktopWindow());
		if (!cam.read(frame))
		{
			msleep(1);
			continue;
		}
		if (frame.empty())
		{
			msleep(1);
			continue;
		}
		
		fMutex.lock();
		for (int i = 0; i < filters.size(); i++)
		{
			Mat desFrame;
			filters[i]->Filter(&frame, &desFrame);
			frame = desFrame;
		}
		fMutex.unlock();
		XData da((char*)frame.data, frame.cols * frame.rows * frame.elemSize(), GetCurTime());
		push(da);
	}
}
//系统相机初始化
bool CVicdoeRecord::Init(int camIndex)
{
	/// 1 使用opencv打开系统相机
	cam.open(camIndex);
	if (!cam.isOpened())
	{
		cout << "cam open failed!" << endl;
		return false;
	}
	cout <<"camp：" << camIndex << " open success" << endl;
	//cam.set(CV_CAP_PROP_FPS, 30);//帧率 帧/秒
	width = cam.get(CAP_PROP_FRAME_WIDTH);
	height = cam.get(CAP_PROP_FRAME_HEIGHT);
	fps = cam.get(CV_CAP_PROP_FPS);
	return true;
}
//rtsp相机初始化
bool CVicdoeRecord::Init(const char* url)
{
	/// 1 使用opencv打开rtsp相机
	cam.open(url);
	if (!cam.isOpened())
	{
		cout << "cam open failed!" << endl;
		return false;
	}
	cout << "camp：" << url << " open success" << endl;
	cam.set(CV_CAP_PROP_FPS, 30);//帧率 帧/秒
	width = cam.get(CAP_PROP_FRAME_WIDTH);
	height = cam.get(CAP_PROP_FRAME_HEIGHT);
	fps = cam.get(CV_CAP_PROP_FPS);
	return true;
}
//关闭相机
void CVicdoeRecord::stop()
{
	XDataThread::stop();
	if (cam.isOpened())
	{
		cam.release();
	}
}

CVicdoeRecord::CVicdoeRecord()
{
}

CVicdoeRecord::~CVicdoeRecord()
{
}
