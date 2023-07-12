#include "XDataThread.h"

XDataThread::XDataThread()
{
}

XDataThread::~XDataThread()
{
}
//���б��β����
void XDataThread::push(XData da)
{
    mutex.lock();
    if (datas.size() > maxList)
    {
        datas.front().Drop();
        datas.pop_front();
    }
    datas.push_back(da);
    mutex.unlock();
}
//��ȡ�б������������
XData XDataThread::pop()
{
    mutex.lock();
    if (datas.empty())
    {
        mutex.unlock();
        return XData();
    }
    XData da = datas.front();
    datas.pop_front();
    mutex.unlock();

    return da;
}

bool XDataThread::start()
{
    isRun = true;
    QThread::start();
    return true;
}

void XDataThread::stop()
{
    isRun = false;
    wait();
}
//��������
void XDataThread::clear()
{
    mutex.lock();
    while (!datas.empty())
    {
        datas.front().Drop();
        datas.pop_front();
    }
    mutex.unlock();
}
