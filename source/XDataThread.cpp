#include "XDataThread.h"

XDataThread::XDataThread()
{
}

XDataThread::~XDataThread()
{
}
//在列表结尾插入
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
//读取列表中最早的数据
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
//清理数据
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
