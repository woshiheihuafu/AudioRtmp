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
    /// 1.���豸
    const char* outUrl = "rtmp://192.168.150.128/live/123456";
    int sampleRate = 44100;
    int channels = 2;
    int sampleByte = 2;
    int nbSample = 1024;
    /// 1.1�������
    XVideoRecord* vr = XVideoRecord::GetInstance();
    ///  ����������
    XFilter* xf = XFilter::Get();
    xf->Set("d", 9);
    if (!vr->Init(0))
    {
        cout << "1.1 �������ʧ�ܣ�" << endl;
        getchar();
        return -1;
    }
    vr->start();
    cout << "1.1 ��������ɹ�!" << endl;
    /// 1.2qt��Ƶ��ʼ¼��
    XAudioRecord* ar = XAudioRecord::GetInstance();
    ar->channels = channels;
    ar->sampleRate = sampleRate;
    ar->sampleByte = sampleByte;
    ar->nb_samples = nbSample;
    if (!ar->Init())
    {
        cout << "1.2 ����Ƶ¼��ʧ�ܣ�" << endl;
        getchar();
        return -1;
    }
    ar->start();

    cout << "1.2 ����Ƶ¼�Ƴɹ���" << endl;
    ///2.����Ƶ������
    int re = 0;
    XMediaEncode* me = XMediaEncode::GetInstance();
    /// 2.1��Ƶ��ʽת��
    //��ʼ����ʽת��������
    //��ʼ����������ݽṹ
    me->inWidth = vr->width;
    me->inHeight = vr->height;
    me->outWidth = vr->width;
    me->outHeight = vr->height;
    if (!me->InitScale())
    {
        cout << "2.1 ��ʼ����Ƶ����ת��������ʧ�ܣ�" << endl;
        getchar();
        return -1;
    }
    cout << "2.1 ��ʼ����Ƶ����ת�������ĳɹ���" << endl;
    /// 2.2��Ƶ�ز��� �����ĳ�ʼ��
    me->channels = channels;
    me->nb_sample = 1024;
    me->sampleRate = sampleRate;
    me->inSampleFmt = X_S16;
    me->outSampleFmt = X_FLTP;

    if (!me->InitResample())
    {
        cout << "2.2 ��ʼ����Ƶ�ز���������ʧ�ܣ�" << endl;
        getchar();
        return -1;
    }
    cout << "2.2 ��ʼ����Ƶ�ز��������ĳɹ���" << endl;
    
    /// 3.����Ƶ��������ʼ��
    /// 3.1 ��ʼ����Ƶ������
    if (!me->InitAudioCodec())
    {
        cout << "3.1 ��ʼ����Ƶ������ʧ�ܣ�" << endl;
        getchar();
        return -1;
    }
    cout << "3.1 ��ʼ����Ƶ�������ɹ���" << endl;
    /// 3.2 ��ʼ����Ƶ������
    if (!me->InitVideoCodec())
    {
        cout << "3.2 ��ʼ����Ƶ������ʧ�ܣ�" << endl;
        getchar();
        return -1;
    }
    cout << "3.2 ��ʼ����Ƶ�������ɹ���" << endl;
    /// 4. �����װ������Ƶ������
    /// 4.1 ���������װ��������
    XRtmp* xr = XRtmp::GetInstance(0);
    if (!xr->Init(outUrl))
    {
        getchar();
        return -1;
    }
    /// 4.2 �����Ƶ��
    int vIndex = 0;
    if ((vIndex = xr->AddStream(me->vc)) == -1)
    {
        cout << "4.2 �����Ƶ��ʧ�ܣ�" << endl;
        getchar();
        return -1;
    }
    cout << "4.2 �����Ƶ���ɹ���" << endl;
    /// 4.3 �����Ƶ�� 
    int aIndex = 0;
    if ((aIndex = xr->AddStream(me->ac)) == -1)
    {
        cout << "4.3 �����Ƶ��ʧ�ܣ�" << endl;
        getchar();
        return -1;
    }
    cout << "4.3 �����Ƶ���ɹ���" << endl;
    /// 5. ��rtmp ���������IO
    if (!xr->SendHead())
    {
        cout << "5. ����·ʧ��!" << endl;
        getchar();
        return -1;
    }
    int apts = 0;
    //��������Ƶ֮ǰ����

    ar->clear();
    vr->clear();
    long long begintime = GetCurTime();
    for (;;)
    {
        /// 6. һ�ζ�ȡһ֡����Ƶ
        XData audioData = ar->pop();
        XData videoData = vr->pop();

        if (audioData.size <= 0 && videoData.size <= 0)
        {
            QThread::msleep(1);
            continue;
        }
        //6.1 ������Ƶ
        if (audioData.size > 0)
        {
            audioData.pts = audioData.pts - begintime;
            //�ز���Դ����
            XData pcm = me->Resample(audioData);
            audioData.Drop();
            //pts����
            // sample_rate / nb_sample =  һ֡��Ƶ������
            // timebase pts = sec * timebase.den;
            XData pkt = me->EncodeAudio(pcm);
            if (pkt.size > 0)
            {
                ///����
                if (xr->SendFrame(pkt,aIndex))
                {
                    cout << "#" << flush;
                }
            }
        }
        //6.2 ������Ƶ
        if (videoData.size > 0)
        {
            videoData.pts = videoData.pts - begintime;
            XData test = videoData;
            XData yuv = me->RGBToYUV(videoData);
            videoData.Drop();
            XData pkt = me->EncodeVideo(yuv);
            
            if (pkt.size > 0)
            {
                ///����
                if (xr->SendFrame(pkt,vIndex))
                {
                    cout << "@" << flush;
                }
            }
        }
    }
    return a.exec();
}
