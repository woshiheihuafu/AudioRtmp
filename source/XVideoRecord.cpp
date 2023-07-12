#include "XVideoRecord.h"
#include "CVicdoeRecord.h"
XVideoRecord* XVideoRecord::GetInstance(unsigned char index)
{
    static CVicdoeRecord xc[255];
    return &xc[index];
}

XVideoRecord::~XVideoRecord()
{
}

XVideoRecord::XVideoRecord()
{
}
