#ifndef _NE_Blocks_HDR_
#define _NE_Blocks_HDR_

#include <vector>
#include "Stream.h"

struct NETableInfo16_16 {
    // Address of Table
    uint16_t tblAddress;
    // Size of Table
    uint16_t tblSize;
};

struct NETableInfo32_16 {
    // Address of Table
    uint32_t tblAddress;
    // Size of Table
    uint16_t tblSize;
};

enum DataSegmntFlag { NOAUTODATA, SINGLEDATA, MULTIPLEDATA };
enum ProcessorOpMode { REAL_MODE, PROTECTD_MODE };
struct NETargetOS {
    bool unknown;
    bool windows;
    bool reserved;
};

struct NEModuleFlags {
    DataSegmntFlag dataSegmnt;
    ProcessorOpMode opMode;
    bool entryOnFirstSegmnt;
    bool linkTimeErrors;
    bool nonConformingModule;
    bool libraryModule;
};

struct NEWindowsFlags {
    bool v2x_protectedMode;
    bool v2x_proportionalFonts;
    bool fastLoadArea;
};

struct SegmentOff16 {
    uint16_t segmntIndex;
    uint16_t address;
};

class NEInfoBlock {
    uint16_t offNEHeader=0;
    NEModuleFlags moduleFlags;
    uint16_t autoDataSegmntIndex;
    uint16_t nMovableEntryPts;
    uint16_t nResourceSegments;
    uint16_t shiftCount;

    uint8_t linker_version;
    uint8_t linker_revision; 

    // Entry Table
    NETableInfo16_16 entryTable;
    // Module-Reference Table
    NETableInfo16_16 modRefTblInfo;
    // Nonresident-Name Table
    NETableInfo32_16 nonResidNameTblInfo;
    // SegmentTable
    NETableInfo16_16 segmntTblInfo;

    // Address of Resident-Name Table
    uint16_t residNameTblAddr;
    // Address of Resource Table
    uint16_t resourceTblAddr;
    // Address of Imported-Name Table
    uint16_t importdNameTblAddr;

    uint16_t szLocalHeap;
    uint16_t szStack;
    SegmentOff16 ipValue;
    SegmentOff16 spValue;
    
    NETargetOS targetOS;
    NEWindowsFlags winFlags;
    NETableInfo16_16 fastLoadArea;
    uint16_t expectdWinVersion;

public:
    NEInfoBlock(IStream& file);
    
    uint16_t GetAutoDataSegIndex() const;
    NETableInfo16_16 GetEntryTblInfo() const;
    uint16_t GetExpWinVersion() const;
    NETableInfo16_16 GetFastLoadAreaInfo() const;
    uint16_t GetImportedNameTblAddress() const;
    SegmentOff16 GetIP() const;
    NETableInfo16_16 GetModRefTableInfo() const;
    uint16_t GetNbrMovableEntryPts() const;
    uint16_t GetNbrResourceSegments() const;
    NETableInfo32_16 GetNonResidentNameTblInfo() const;
    uint16_t GetResidentNameTblAddr() const;
    uint16_t GetResourceTblAddr() const;
    uint16_t GetInfoBlkOffset() const;
    uint16_t GetLocalHeapInitialSize() const;
    NETableInfo16_16 GetSegmentTblInfo() const;
    uint32_t GetSectorOffset(uint16_t index) const;
    uint16_t GetShiftCount() const;
    uint16_t GetStackInitialSize() const;
    SegmentOff16 GetStackPtr() const;

    uint8_t Linker_GetRevisionNbr() const;
    uint8_t Linker_GetVersionNbr() const;
};

struct NESegmentInfo {
    uint16_t offData;
    uint16_t szData;
    bool dataSegment;
    bool ldrHasAllocdMem;
    bool loaded;
    bool moveable;
    bool pure;
    bool preload;
    bool writable;
    bool relocInfo;
    bool discardable;
    uint16_t minAllocSz;
};

class NESegmentTbl {
    std::vector<NESegmentInfo> entries;
public:
    NESegmentTbl(IStream& file, NEInfoBlock& infoBlk);

    std::vector<NESegmentInfo> GetEntries() const;
};

struct NEEntry {
    // The entry is exported.
    bool exported;
    // The segment uses a global (shared) data segment.
    bool usesGlobalDataSgmnt;
    // Size of stack in words for ring transitions.
    uint8_t wszStack;
    // Associated segment.
    SegmentOff16 segment;
};

// class NEEntryTbl {
//     std::vector<NEEntry> entries;

// public:
//     NEEntryTbl(IStream& file, NEInfoBlock& infoBlk);
//     std::vector<NEEntry> GetNEEntries() const;    
// };

enum NERelocType { INTERNAL_REF, IMPORT_ORDINAL, IMPORT_NAME, OSFIXUP };
enum NERelocAddrType {
    RELOC_ADDR_LOWBYTE,
    RELOC_ADDR_SEL16,
    RELOC_ADDR_OFF16,
    RELOC_ADDR_OFF32,
    RELOC_ADDR_PTR32,
    RELOC_ADDR_PTR48
};

struct NERelocTarget {
    // Relocation address type
    NERelocAddrType addressType;
    // Offset of the reloc item within the segment
    uint16_t offRelocItem;
};

struct NERelocImportSrc {
    // Index to a module's reference table (why?)
    uint16_t modRefTblIndex;
    // Ordinal value or offset to imported-name table
    uint16_t srcPos;
};

struct NERelocItem {
    // Reloc Target - Where should relocation be applied?
    NERelocTarget relocTarget;
    // Reloc Source Type (imported ordinal, name or internal ref)
    NERelocType relocType;
    // Reloc Source - Where is the resource to be attached to reloc target?
    union {
        NERelocImportSrc importType;
        SegmentOff16 internalRef;
    } relocSrc;
};

class NERelocTbl {
    std::vector<NERelocItem> entries;

public:
    NERelocTbl(IStream& file, NEInfoBlock& infoBlk, NESegmentInfo& segInfo);
    
    std::vector<NERelocItem> GetRelocEntries() const;
};

#endif // _NE_Blocks_HDR_