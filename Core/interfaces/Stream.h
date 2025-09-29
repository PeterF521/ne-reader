#ifndef _Stream_HDR_
#define _Stream_HDR_

#include <cstdint>

enum SeekPos { SEEKPOS_BEG, SEEKPOS_CUR, SEEKPOS_END };

class IStream {
public:
    virtual bool Good() const=0;

    // virtual IStream& ReadUInt64(uint64_t& value)=0;
    // virtual IStream& ReadInt64(int64_t& value)=0;
    virtual IStream& ReadUInt32(uint32_t& value)=0;
    // virtual IStream& ReadInt32(int32_t& value)=0;
    virtual IStream& ReadUInt16(uint16_t& value)=0;
    // virtual IStream& ReadInt16(int16_t& value)=0;
    virtual IStream& ReadUInt8(uint8_t& value)=0;

    virtual IStream& ReadChar(char& ch)=0;
    virtual IStream& ReadString(char* str, uint64_t sz)=0;
    virtual IStream& SeekRead(uint64_t addr, SeekPos pos=SEEKPOS_CUR)=0;
};

#endif // _Stream_HDR_