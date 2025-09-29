#include <cstring>
#include <fstream>
#include "./interfaces/libstdc++.h"
#include "../Core/interfaces/Stream.h"


IFile::IFile(const char *path)
    : _file(path, std::ios_base::binary)
{
    _path = new char[strlen(path) + 1];
    strcpy(_path, path);
}

IFile::~IFile() {
    delete[] _path;
}

bool IFile::Good() const {
    return _file.good();
}

void IFile::ExportSegment(
    std::string out_fname,
    std::streampos beg,
    std::streampos end)
{
    std::ofstream ofile(out_fname);
    std::streampos gptr(_file.tellg());
    SeekRead(beg, SEEKPOS_BEG);
    for(std::streamoff i=0; i<end-beg; ++i) ofile.put(_file.get());
    ofile.close();
}

IFile& IFile::ReadChar(char &ch)
{
    _file.get(ch);
    return *this;
}

IFile& IFile::ReadString(char *str, uint64_t sz)
{
    _file.read(str, sz);
    return *this;
}

IFile& IFile::ReadUInt8(uint8_t &value)
{
    // std::cout << "Read Offset: " << _file.tellg() << '\n';
    _file.get((char &)value);
    return *this;
}

IFile& IFile::ReadUInt16(uint16_t &value)
{
    // std::cout << "Read Offset: " << _file.tellg() << '\n';
    uint8_t buf[2];
    _file.read((char*)buf, 2);
    value = buf[1] << 8 | buf[0];
    return *this;
}

IFile& IFile::ReadUInt32(uint32_t &value)
{
    // std::cout << "Read Offset: " << _file.tellg() << '\n';
    uint8_t buf[4];
    _file.read((char*)buf, 4);
    for (char i = 0; i < 4; ++i)
        value |= buf[i] << i*8;
    return *this;
}

IFile& IFile::SeekRead(uint64_t addr, SeekPos pos)
{
    switch (pos)
    {
    case SEEKPOS_BEG:
        _file.seekg(addr, _file.beg);
        break;
    case SEEKPOS_CUR:
        _file.seekg(addr, _file.cur);
        break;
    case SEEKPOS_END:
        _file.seekg(addr, _file.end);
        break;
    }
    return *this;
}