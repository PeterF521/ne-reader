#include "interfaces/NEFile.h"

NESegment::NESegment(NESegmentInfo segInfo, NERelocTbl relTbl)
    : _segInfo(segInfo), _relTbl(relTbl) {}

NESegmentInfo
NESegment::GetSegmentInfo() const {
    return _segInfo;
}

NERelocTbl
NESegment::GetRelocTable() const {
    return _relTbl;
}

NEFile::NEFile(IStream& _file) : infoBlock(_file) {
    NESegmentTbl segTbl(_file, infoBlock);
    auto segs = segTbl.GetEntries();
    segTable.reserve(segs.size());
    for(NESegmentInfo segInfo : segs) {
        segTable.push_back(NESegment(segInfo,NERelocTbl(_file,infoBlock,segInfo)));
    }
}

NEInfoBlock
NEFile::GetInfoBlock() const {
    return infoBlock;
}

std::vector<NESegment>
NEFile::GetSegments() const {
    return segTable;
}

// NEEntryTbl
// NEFile::GetEntryTable() const {
//     return entryTable;
// }