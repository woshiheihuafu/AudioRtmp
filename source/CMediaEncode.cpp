#include "CMediaEncode.h"
#if defined WIN32 || defined _WIN32
#include<Windows.h>
#endif

//获取CPU数量
static int GetCpuNum()
{
#if defined WIN32 || defined _WIN32
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	return (int)sysInfo.dwNumberOfProcessors;
#elif defined __linux__
	return (int)sysconf(_SC_NPROCESSORS_ONLN);
#elif defined __APPLE__
	int numCPU = 0;
	int mib[4];
	size_t len = sizeof(numCPU);

	mib[0] = CTL_HW;
	mib[1] = HW_AVAILCPU;

	sysctl(mib, 2, &numCPU, &len, NULL, 0);
	if (numCPU < 1)
	{
		mib[1] = HW_NCPU;
		sysctl(mib, 2, &numCPU, &len, NULL, 0);
		if (numCPU < 1)
		{
			numCPU = 1;
		}
	}
	return (int)numCPU;
#else
	return 1;
#endif
}
//初始化像素格式转换上下文
bool CMediaEncode::InitScale()
{
	///   初始化像素格式转换上下文
	vsc = sws_getCachedContext(vsc,
		inWidth, inHeight, AV_PIX_FMT_BGR24,	 //源宽、高、像素格式
		outWidth,outHeight, AV_PIX_FMT_YUV420P,//目标宽、高、像素格式
		SWS_BICUBIC,  // 尺寸变化使用算法
		0, 0, 0
	);
	if (!vsc)
	{
		cout << "sws_getCachedContext failed!" << endl;
		return false;
	}
	///   初始化输出的数据结构
	yuv = av_frame_alloc();
	yuv->format = AV_PIX_FMT_YUV420P;
	yuv->width = inWidth;
	yuv->height = inHeight;
	yuv->pts = 0;
	//   分配yuv空间
	int ret = av_frame_get_buffer(yuv, 32);
	if (ret != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(ret, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}

XData CMediaEncode::RGBToYUV(XData data)
{
	XData r;
	r.pts = data.pts;

	///rgb to yuv
	//输入的数据结构
	uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
	//indata[0] bgrbgrbgr
	//plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr 
	indata[0] = (uint8_t*)data.data;
	int insize[AV_NUM_DATA_POINTERS] = { 0 };
	//一行（宽）数据的字节数
	insize[0] = inWidth * inPixSize;
	int h = sws_scale(vsc, indata, insize, 0, inHeight, //源数据
		yuv->data, yuv->linesize);
	if (h <= 0)
	{
		return r;
	}
	yuv->pts = data.pts;
	r.data = (char*)yuv;
	int* p = yuv->linesize;
	//r.size = 0;
	while ((*p))
	{
		r.size += (*p) * outHeight;
		p++;
	}
	return r;
	
}
//视频编码器初始化
bool CMediaEncode::InitVideoCodec()
{
	///4 初始化编码上下文
	if (!(vc = CreateCodec(AV_CODEC_ID_H264)))
	{
		return false;
	}
	vc->bit_rate = 50 * 1024 * 8;//压缩后每秒视频的bit位大小 50kB
	vc->width = outWidth;
	vc->height = outHeight;
	//vc->time_base = { 1,outFps };
	vc->framerate = { outFps,1 };

	//画面组的大小，多少帧一个关键帧
	vc->gop_size = 50;
	vc->max_b_frames = 0;
	vc->pix_fmt = AV_PIX_FMT_YUV420P;
	//d 打开编码器上下文
	return OpenCodec(&vc);
}
//音频编码器初始化
bool CMediaEncode::InitAudioCodec()
{
	/// 4.初始化音频编码器
	if (!(ac = CreateCodec(AV_CODEC_ID_AAC)))
	{
		return false;
	}
	//ac->codec_id 传入了codec，已经赋值过了
	ac->bit_rate = 40000;
	ac->sample_rate = sampleRate;//编码成什么样本率
	ac->sample_fmt = AV_SAMPLE_FMT_FLTP;
	ac->channels = channels;
	ac->channel_layout = av_get_default_channel_layout(channels);
	
	//打开音频编码器
	return OpenCodec(&ac);
}
//视频编码
XData CMediaEncode::EncodeVideo(XData frame)
{
	av_packet_unref(&vPack);
	XData r;
	if (frame.size <= 0 || !frame.data) return r;
	AVFrame* p = (AVFrame*)frame.data;
	///h264编码
	int re = avcodec_send_frame(vc, p);
	if (re != 0)
		return r;

	re = avcodec_receive_packet(vc, &vPack);
	if(re != 0 || vPack.size <= 0)
		return r;
	r.data = (char*)&vPack;
	r.size = vPack.size;
	r.pts = frame.pts;
	return r;
	
}
//音频编码
long long lastPts = 0;
XData CMediaEncode::EncodeAudio(XData frame)
{
	XData r;
	if (frame.size <= 0 || !frame.data) return r;
	AVFrame* p = (AVFrame*)frame.data;
	if (lastPts == p->pts)
	{
		p->pts += 1300;
	}
	lastPts = p->pts;
	int re = avcodec_send_frame(ac, p);
	if (re != 0) return r;

	av_packet_unref(&aPack);
	re = avcodec_receive_packet(ac, &aPack);
	if (re != 0) return r;

	//cout << aPack.size << " " << flush;
	r.data = (char*)&aPack;
	r.size = aPack.size;
	r.pts = frame.pts;
	return r;
}
//音频重采样上下文初始化
bool CMediaEncode::InitResample()
{
	/// 2.音频重采样 上下文初始化
	asc = NULL;
	asc = swr_alloc_set_opts(asc,
		av_get_default_channel_layout(channels),
		(AVSampleFormat)outSampleFmt,
		sampleRate,
		av_get_default_channel_layout(channels),
		(AVSampleFormat)inSampleFmt,
		sampleRate, NULL, NULL);//输出格式
	if (!asc)
	{
		cout << "swr_alloc_set_opts error!" << endl;
		return false;
	}
	int re = swr_init(asc);
	if (re != 0)
	{
		cout << "swr_init error!" << endl;
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));
		cout << buf << endl;
		return false;
	}
	cout << "音频重采样上下文初始化成功!" << endl;

	/// 3.音频重采样输出空间分配
	pcm = av_frame_alloc();
	pcm->format = outSampleFmt;
	pcm->channels = channels;
	pcm->channel_layout = av_get_default_channel_layout(channels);
	pcm->nb_samples = nb_sample;//一帧音频一通道的采样数量
	re = av_frame_get_buffer(pcm, 0);//给pcm分配空间
	if (re != 0)
	{
		cout << "swr_init error!" << endl;
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));
		cout << buf << endl;
		return false;
	}
	return true;
}
//重采样
XData CMediaEncode::Resample(XData data)
{
	XData r;
	const uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
	indata[0] = (uint8_t*)data.data;
	int len = swr_convert(asc, pcm->data, pcm->nb_samples,//输出参数，输出存储地址
		indata, pcm->nb_samples);
	if (len <= 0)
	{
		return r;
	}
	pcm->pts = data.pts;
	r.data = (char*)pcm;
	r.size = pcm->nb_samples * pcm->channels * 2;
	r.pts = data.pts;
	return r;
}

void CMediaEncode::close()
{
	if (vsc)
	{
		sws_freeContext(vsc);
		vsc = NULL;
	}
	if (asc)
	{
		swr_free(&asc);
	}
	if (yuv)
	{
		av_frame_free(&yuv);//自动赋NULL
	}
	if (vc)
	{
		avcodec_free_context(&vc);
	}
	if (pcm)
	{
		av_frame_free(&pcm);
	}
	vpts = 0;
	av_packet_unref(&vPack);
	apts = 0;
	av_packet_unref(&aPack);
}

AVCodecContext* CMediaEncode::CreateCodec(AVCodecID id)
{
	/// 4.初始化编码器上下文
	AVCodec* codec = avcodec_find_encoder(id);
	if (!codec)
	{
		cout << "avcodec_find_encoder error!" << endl;
		getchar();
		return NULL;
	}
	cout << "avcodec_find_encoder sucess!" << endl;
	AVCodecContext* c = avcodec_alloc_context3(codec);//音视频频编码器上下文
	if (!c)
	{
		cout << "avcodec_alloc_context3 error!" << endl;
		getchar();
		return NULL;

	}
	cout << "avcodec_alloc_context3 sucess!" << endl;


	//配置音频编码器
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	c->thread_count = GetCpuNum();
	c->time_base = { 1,1000000 };
	return c;
}

bool CMediaEncode::OpenCodec(AVCodecContext** c)
{
	//打开音频,视频编码器
	int re = avcodec_open2(*c, 0, 0);
	if (re != 0)
	{
		cout << "swr_init error!" << endl;
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf));
		cout << buf << endl;
		avcodec_free_context(c);
		return false;
	}
	cout << "音频编码器打开成功!" << endl;
	return true;
}
