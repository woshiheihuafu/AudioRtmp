#include "CRtmp.h"
//初始化封装其
bool CRtmp::Init(const char* url)
{
	int re = avformat_alloc_output_context2(&ic, 0, "flv", url);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	this->url = url;
	return true;
}

void CRtmp::close()
{
	if (ic)
	{
		avformat_close_input(&ic);
		vs = NULL;
		url = "";
	}
	vc = NULL;
}
//添加视频流
int CRtmp::AddStream(const AVCodecContext* c)
{
	if (!c) return -1;
	//b 添加视频流 
	AVStream* st = avformat_new_stream(ic, NULL);
	if (!st)
	{
		cout << "avformat_new_stream failed" << endl;
		return -1;
	}
	st->codecpar->codec_tag = 0;
	//从编码器复制参数
	avcodec_parameters_from_context(st->codecpar, c);
	av_dump_format(ic, 0, url.c_str(), 1);
	if (c->codec_type == AVMEDIA_TYPE_VIDEO)
	{
		vc = c;
		vs = st;
	}
	else if (c->codec_type == AVMEDIA_TYPE_AUDIO)
	{
		ac = c;
		as = st;
	}
	return st->index;
}
//发送封装头
bool CRtmp::SendHead()
{
	///打开rtmp 的网络输出IO
	int re = avio_open(&ic->pb, url.c_str(), AVIO_FLAG_WRITE);
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}

	//写入封装头
	re = avformat_write_header(ic, NULL);//会将vs的 time_base 改掉
	if (re != 0)
	{
		char buf[1024] = { 0 };
		av_strerror(re, buf, sizeof(buf) - 1);
		cout << buf << endl;
		return false;
	}
	return true;
}
//推流
bool CRtmp::SendFrame(XData data, int streamIndex)
{
	if (!data.data || data.size <= 0)return false;
	AVPacket* pack = (AVPacket*)data.data;
	pack->stream_index = streamIndex;
	AVRational stime;
	AVRational dtime;
	//判断是音频还是视频
	if (vs && vc && pack->stream_index == vs->index)
	{
		stime = vc->time_base;
		dtime = vs->time_base;
	}
	else if (as && ac && pack->stream_index == as->index)
	{
		stime = ac->time_base;
		dtime = as->time_base;
	}
	else
	{
		return false;
	}
	//推流
	pack->pts = av_rescale_q(pack->pts, stime, dtime);
	pack->dts = av_rescale_q(pack->dts, stime, dtime);
	pack->duration = av_rescale_q(pack->duration, stime, dtime);
	int re = av_interleaved_write_frame(ic, pack);
	if (re == 0)
	{
		cout << "#" << flush;
		return true;
	}
	return false;
}
