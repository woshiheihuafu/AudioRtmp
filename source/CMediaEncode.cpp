#include "CMediaEncode.h"
#if defined WIN32 || defined _WIN32
#include<Windows.h>
#endif

//��ȡCPU����
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
//��ʼ�����ظ�ʽת��������
bool CMediaEncode::InitScale()
{
	///   ��ʼ�����ظ�ʽת��������
	vsc = sws_getCachedContext(vsc,
		inWidth, inHeight, AV_PIX_FMT_BGR24,	 //Դ���ߡ����ظ�ʽ
		outWidth,outHeight, AV_PIX_FMT_YUV420P,//Ŀ����ߡ����ظ�ʽ
		SWS_BICUBIC,  // �ߴ�仯ʹ���㷨
		0, 0, 0
	);
	if (!vsc)
	{
		cout << "sws_getCachedContext failed!" << endl;
		return false;
	}
	///   ��ʼ����������ݽṹ
	yuv = av_frame_alloc();
	yuv->format = AV_PIX_FMT_YUV420P;
	yuv->width = inWidth;
	yuv->height = inHeight;
	yuv->pts = 0;
	//   ����yuv�ռ�
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
	//��������ݽṹ
	uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
	//indata[0] bgrbgrbgr
	//plane indata[0] bbbbb indata[1]ggggg indata[2]rrrrr 
	indata[0] = (uint8_t*)data.data;
	int insize[AV_NUM_DATA_POINTERS] = { 0 };
	//һ�У������ݵ��ֽ���
	insize[0] = inWidth * inPixSize;
	int h = sws_scale(vsc, indata, insize, 0, inHeight, //Դ����
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
//��Ƶ��������ʼ��
bool CMediaEncode::InitVideoCodec()
{
	///4 ��ʼ������������
	if (!(vc = CreateCodec(AV_CODEC_ID_H264)))
	{
		return false;
	}
	vc->bit_rate = 50 * 1024 * 8;//ѹ����ÿ����Ƶ��bitλ��С 50kB
	vc->width = outWidth;
	vc->height = outHeight;
	//vc->time_base = { 1,outFps };
	vc->framerate = { outFps,1 };

	//������Ĵ�С������֡һ���ؼ�֡
	vc->gop_size = 50;
	vc->max_b_frames = 0;
	vc->pix_fmt = AV_PIX_FMT_YUV420P;
	//d �򿪱�����������
	return OpenCodec(&vc);
}
//��Ƶ��������ʼ��
bool CMediaEncode::InitAudioCodec()
{
	/// 4.��ʼ����Ƶ������
	if (!(ac = CreateCodec(AV_CODEC_ID_AAC)))
	{
		return false;
	}
	//ac->codec_id ������codec���Ѿ���ֵ����
	ac->bit_rate = 40000;
	ac->sample_rate = sampleRate;//�����ʲô������
	ac->sample_fmt = AV_SAMPLE_FMT_FLTP;
	ac->channels = channels;
	ac->channel_layout = av_get_default_channel_layout(channels);
	
	//����Ƶ������
	return OpenCodec(&ac);
}
//��Ƶ����
XData CMediaEncode::EncodeVideo(XData frame)
{
	av_packet_unref(&vPack);
	XData r;
	if (frame.size <= 0 || !frame.data) return r;
	AVFrame* p = (AVFrame*)frame.data;
	///h264����
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
//��Ƶ����
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
//��Ƶ�ز��������ĳ�ʼ��
bool CMediaEncode::InitResample()
{
	/// 2.��Ƶ�ز��� �����ĳ�ʼ��
	asc = NULL;
	asc = swr_alloc_set_opts(asc,
		av_get_default_channel_layout(channels),
		(AVSampleFormat)outSampleFmt,
		sampleRate,
		av_get_default_channel_layout(channels),
		(AVSampleFormat)inSampleFmt,
		sampleRate, NULL, NULL);//�����ʽ
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
	cout << "��Ƶ�ز��������ĳ�ʼ���ɹ�!" << endl;

	/// 3.��Ƶ�ز�������ռ����
	pcm = av_frame_alloc();
	pcm->format = outSampleFmt;
	pcm->channels = channels;
	pcm->channel_layout = av_get_default_channel_layout(channels);
	pcm->nb_samples = nb_sample;//һ֡��Ƶһͨ���Ĳ�������
	re = av_frame_get_buffer(pcm, 0);//��pcm����ռ�
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
//�ز���
XData CMediaEncode::Resample(XData data)
{
	XData r;
	const uint8_t* indata[AV_NUM_DATA_POINTERS] = { 0 };
	indata[0] = (uint8_t*)data.data;
	int len = swr_convert(asc, pcm->data, pcm->nb_samples,//�������������洢��ַ
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
		av_frame_free(&yuv);//�Զ���NULL
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
	/// 4.��ʼ��������������
	AVCodec* codec = avcodec_find_encoder(id);
	if (!codec)
	{
		cout << "avcodec_find_encoder error!" << endl;
		getchar();
		return NULL;
	}
	cout << "avcodec_find_encoder sucess!" << endl;
	AVCodecContext* c = avcodec_alloc_context3(codec);//����ƵƵ������������
	if (!c)
	{
		cout << "avcodec_alloc_context3 error!" << endl;
		getchar();
		return NULL;

	}
	cout << "avcodec_alloc_context3 sucess!" << endl;


	//������Ƶ������
	c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
	c->thread_count = GetCpuNum();
	c->time_base = { 1,1000000 };
	return c;
}

bool CMediaEncode::OpenCodec(AVCodecContext** c)
{
	//����Ƶ,��Ƶ������
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
	cout << "��Ƶ�������򿪳ɹ�!" << endl;
	return true;
}
