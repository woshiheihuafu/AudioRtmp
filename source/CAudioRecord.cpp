#include "CAudioRecord.h"

CAudioRecord::CAudioRecord()
{
}

CAudioRecord::~CAudioRecord()
{
    
}
//�߳̿�ʼ
void CAudioRecord::run()
{
    cout << "������Ƶ¼���߳�!" << endl;
    int readSize = nb_samples * channels * sampleByte;
    char* buf = new char[readSize];
    while (isRun)
    {

        //��ȡ��¼����Ƶ
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
        long long pts = GetCurTime();//��ȡ����΢����
        //�Ѿ���ȡһ֡��Ƶ
        push(XData(buf,readSize,pts));

    }
    delete buf;
    cout << "�˳���Ƶ¼���߳�!" << endl;
}
//��ʼ¼��
bool CAudioRecord::Init()
{
    stop();
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);//������
    fmt.setChannelCount(channels);//������
    fmt.setSampleSize(sampleByte * 8);;//�����ֽ�
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);//С�˴洢
    fmt.setSampleType(QAudioFormat::UnSignedInt);//�޷���
    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
    if (!info.isFormatSupported(fmt))
    {
        cout << "Audio Format not support!" << endl;
        fmt = info.nearestFormat(fmt);
    }
    cout << "Audio Format support!" << endl;
    input = new QAudioInput(fmt);
    //��ʼ¼����Ƶ
    io = input->start();
    if (!io)
    {
        return false;
    }
    return true;
}
//����¼��
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
