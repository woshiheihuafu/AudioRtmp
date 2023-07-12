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
//    format.setSampleRate(16000);                        //设置采样率
//    format.setChannelCount(1);                          //设定声道数目，mono(平声道)的声道数目是1；stero(立体声)的声道数目是2
//    format.setSampleSize(16);                           //设置采样大小
//    format.setCodec("audio/pcm");                       //设置编码器，"audio/pcm"在所有的平台都支持
//    format.setSampleType(QAudioFormat::SignedInt);      //设置采样类型
//    format.setByteOrder(QAudioFormat::LittleEndian);    //设定高低位的，LittleEndian（低位优先）/LargeEndian(高位优先)
//    QAudioDeviceInfo info = QAudioDeviceInfo::defaultInputDevice();
//
//    if (!info.isFormatSupported(format))                 //格式是否支持;
//    {
//        format = info.nearestFormat(format);
//    }
//
//    QAudioInput* inPut = new QAudioInput(format);            //输出音频
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