#include "CAudioRecord.h"

CAudioRecord::CAudioRecord()
{
}

CAudioRecord::~CAudioRecord()
{
    
}
//线程开始
void CAudioRecord::run()
{
    cout << "进入音频录制线程!" << endl;
    int readSize = nb_samples * channels * sampleByte;
    char* buf = new char[readSize];
    while (isRun)
    {

        //读取已录制音频
        if (input->bytesReady() < readSize)
        {
            QThread::msleep(1);
            continue;
        }
        int size = 0;
        while (size != readSize)
        {
            int len = io->read(buf + size, readSize - size);
            if (len < 0) break;
            size += len;
        }
        if (size != readSize) continue;
        long long pts = GetCurTime();//获取的是微妙数
        //已经读取一帧音频
        push(XData(buf,readSize,pts));

    }
    delete buf;
    cout << "退出音频录制线程!" << endl;
}
//开始录制
bool CAudioRecord::Init()
{
    stop();
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);//采样率
    fmt.setChannelCount(channels);//声道数
    fmt.setSampleSize(sampleByte * 8);;//采样字节
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);//小端存储
    fmt.setSampleType(QAudioFormat::UnSignedInt);//无符号
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(fmt))
    {
        cout << "Audio Format not support!" << endl;
        fmt = info.nearestFormat(fmt);
    }
    cout << "Audio Format support!" << endl;
    input = new QAudioInput(fmt);
    //开始录制音频
    io = input->start();
    if (!io)
    {
        return false;
    }
    return true;
}
//结束录制
void CAudioRecord::stop()
{
    XDataThread::stop();
    if (input)
        input->stop();
    if (io)
        io->close();
    input = NULL;
    io = NULL;
}


//XData CAudioRecord::push()
//{
//    return XData();
//}
