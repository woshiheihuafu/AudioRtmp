#include "XAudioRecord.h"
#include "CAudioRecord.h"
XAudioRecord* XAudioRecord::GetInstance(XRecordType type, unsigned char index)
{
    static CAudioRecord record[255];
    return &record[index];  
}

XAudioRecord::~XAudioRecord()
{
}

XAudioRecord::XAudioRecord()
{
}
