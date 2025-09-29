#include "NE_Blocks.h"

class NESegment {
    NESegmentInfo _segInfo;
    NERelocTbl _relTbl;

public:
    NESegment(NESegmentInfo segInfo, NERelocTbl relTbl);
    NESegmentInfo GetSegmentInfo() const;
    NERelocTbl GetRelocTable() const;
};

class NEFile {
    NEInfoBlock infoBlock;
    std::vector<NESegment> segTable;
public:
    NEFile(IStream& file);
    NEInfoBlock GetInfoBlock() const;
    std::vector<NESegment> GetSegments() const;
};