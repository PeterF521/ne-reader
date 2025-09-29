#include <cstdint>
#include <fstream>
#include "../../Core/interfaces/Stream.h"

class IFile : public IStream {
    char *_path;
    std::ifstream _file;

public:
    IFile() = delete;
    IFile(const char *path);
    ~IFile();

    bool Good() const;

    void ExportSegment(
        std::string out_fname,
        std::streampos beg,
        std::streampos end);

    IFile& ReadChar(char& ch);
    IFile& ReadString(char *str, uint64_t sz);
    IFile& ReadUInt8(uint8_t& val);
    IFile& ReadUInt16(uint16_t& val);
    IFile& ReadUInt32(uint32_t& val);
    IFile& SeekRead(uint64_t addr, SeekPos pos = SEEKPOS_CUR);
};