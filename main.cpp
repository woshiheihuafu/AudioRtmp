#include <iostream>
using namespace std;
#include <QtCore/QCoreApplication>
#include <qaudioinput.h>
#include <qthread.h>
#include<XMediaEncode.h>
#include<XRtmp.h>
#include"XAudioRecord.h"
#include"XVideoRecord.h"
#include"XFilter.h"

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);
    /// 1.打开设备
    const char* outUrl = "rtmp://192.168.150.128/live/123456";
    int sampleRate = 44100;
    int channels = 2;
    int sampleByte = 2;
    int nbSample = 1024;
    /// 1.1打开摄像机
    XVideoRecord* vr = XVideoRecord::GetInstance();
    ///  创建过滤器
    XFilter* xf = XFilter::Get();
    xf->Set("d", 9);
    if (!vr->Init(0))
    {
        cout << "1.1 打开摄像机失败！" << endl;
        getchar();
        return -1;
    }
    vr->start();
    cout << "1.1 打开摄像机成功!" << endl;
    /// 1.2qt音频开始录制
    XAudioRecord* ar = XAudioRecord::GetInstance();
    ar->channels = channels;
    ar->sampleRate = sampleRate;
    ar->sampleByte = sampleByte;
    ar->nb_samples = nbSample;
    if (!ar->Init())
    {
        cout << "1.2 打开音频录制失败！" << endl;
        getchar();
        return -1;
    }
    ar->start();

    cout << "1.2 打开音频录制成功！" << endl;
    ///2.音视频编码类
    int re = 0;
    XMediaEncode* me = XMediaEncode::GetInstance();
    /// 2.1视频格式转换
    //初始化格式转换上下文
    //初始化输出的数据结构
    me->inWidth = vr->width;
    me->inHeight = vr->height;
    me->outWidth = vr->width;
    me->outHeight = vr->height;
    if (!me->InitScale())
    {
        cout << "2.1 初始化视频像素转换上下文失败！" << endl;
        getchar();
        return -1;
    }
    cout << "2.1 初始化视频像素转换上下文成功！" << endl;
    /// 2.2音频重采样 上下文初始化
    me->channels = channels;
    me->nb_sample = 1024;
    me->sampleRate = sampleRate;
    me->inSampleFmt = X_S16;
    me->outSampleFmt = X_FLTP;

    if (!me->InitResample())
    {
        cout << "2.2 初始化音频重采样上下文失败！" << endl;
        getchar();
        return -1;
    }
    cout << "2.2 初始化音频重采样上下文成功！" << endl;
    
    /// 3.音视频编码器初始化
    /// 3.1 初始化音频编码器
    if (!me->InitAudioCodec())
    {
        cout << "3.1 初始化音频编码器失败！" << endl;
        getchar();
        return -1;
    }
    cout << "3.1 初始化音频编码器成功！" << endl;
    /// 3.2 初始化视频编码器
    if (!me->InitVideoCodec())
    {
        cout << "3.2 初始化视频编码器失败！" << endl;
        getchar();
        return -1;
    }
    cout << "3.2 初始化视频编码器成功！" << endl;
    /// 4. 输出封装器和音频流配置
    /// 4.1 创建输出封装器上下文
    XRtmp* xr = XRtmp::GetInstance(0);
    if (!xr->Init(outUrl))
    {
        getchar();
        return -1;
    }
    /// 4.2 添加视频流
    int vIndex = 0;
    if ((vIndex = xr->AddStream(me->vc)) == -1)
    {
        cout << "4.2 添加视频流失败！" << endl;
        getchar();
        return -1;
    }
    cout << "4.2 添加视频流成功！" << endl;
    /// 4.3 添加音频流 
    int aIndex = 0;
    if ((aIndex = xr->AddStream(me->ac)) == -1)
    {
        cout << "4.3 添加音频流失败！" << endl;
        getchar();
        return -1;
    }
    cout << "4.3 添加音频流成功！" << endl;
    /// 5. 打开rtmp 的网络输出IO
    if (!xr->SendHead())
    {
        cout << "5. 打开网路失败!" << endl;
        getchar();
        return -1;
    }
    int apts = 0;
    //清理音视频之前数据

    ar->clear();
    vr->clear();
    long long begintime = GetCurTime();
    for (;;)
    {
        /// 6. 一次读取一帧音视频
        XData audioData = ar->pop();
        XData videoData = vr->pop();

        if (audioData.size <= 0 && videoData.size <= 0)
        {
            QThread::msleep(1);
            continue;
        }
        //6.1 处理音频
        if (audioData.size > 0)
        {
            audioData.pts = audioData.pts - begintime;
            //重采样源数据
            XData pcm = me->Resample(audioData);
            audioData.Drop();
            //pts运算
            // sample_rate / nb_sample =  一帧音频的秒数
            // timebase pts = sec * timebase.den;
            XData pkt = me->EncodeAudio(pcm);
            if (pkt.size > 0)
            {
                ///推流
                if (xr->SendFrame(pkt,aIndex))
                {
                    cout << "#" << flush;
                }
            }
        }
        //6.2 处理视频
        if (videoData.size > 0)
        {
            videoData.pts = videoData.pts - begintime;
            XData test = videoData;
            XData yuv = me->RGBToYUV(videoData);
            videoData.Drop();
            XData pkt = me->EncodeVideo(yuv);
            
            if (pkt.size > 0)
            {
                ///推流
                if (xr->SendFrame(pkt,vIndex))
                {
                    cout << "@" << flush;
                }
            }
        }
    }
    return a.exec();
}
