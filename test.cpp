//#include <iostream>
//using namespace std;
//#include <QtCore/QCoreApplication>
//#include <qaudioinput.h>
//#include <qthread.h>
//#include <qaudiooutput.h>
//#include<Windows.h>
//int main(int argc, char* argv[])
//{
//    QCoreApplication a(argc, argv);
//    QAudioFormat format;
//    format.setSampleRate(16000);                        //���ò�����
//    format.setChannelCount(1);                          //�趨������Ŀ��mono(ƽ����)��������Ŀ��1��stero(������)��������Ŀ��2
//    format.setSampleSize(16);                           //���ò�����С
//    format.setCodec("audio/pcm");                       //���ñ�������"audio/pcm"�����е�ƽ̨��֧��
//    format.setSampleType(QAudioFormat::SignedInt);      //���ò�������
//    format.setByteOrder(QAudioFormat::LittleEndian);    //�趨�ߵ�λ�ģ�LittleEndian����λ���ȣ�/LargeEndian(��λ����)
//    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
//
//    if (!info.isFormatSupported(format))                 //��ʽ�Ƿ�֧��;
//    {
//        format = info.nearestFormat(format);
//    }
//
//    QAudioInput* inPut = new QAudioInput(format);            //�����Ƶ
//    QAudioOutput* output = new QAudioOutput(format);
//    QIODevice *inPutDevice = inPut->start();
//    QIODevice* outPutDevice = output->start();
//    while (1)
//    {
//        QByteArray audio = inPutDevice->readAll();
//        outPutDevice->write(audio);
//        printf("running");
//        Sleep(100);
//    }
//
//
//    return a.exec();
//}