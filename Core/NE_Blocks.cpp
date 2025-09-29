#include <bitset>
#include <iostream>
#include "interfaces/NE_Blocks.h"

// Aux method - Get segment size
uint32_t GetSegmentLength(const NESegmentInfo& segInfo) {
    if(segInfo.szData==0) {
        if(segInfo.offData!=0) return 0x10000; // 64K
        return 0;
    }
    return segInfo.szData;
}

NEInfoBlock::NEInfoBlock(IStream& file) {
    char signature[2]{0};
    file
        // Seek to beginning of NE File Header
        .SeekRead(0x3c, SEEKPOS_BEG)
        .ReadUInt16(offNEHeader)
        .SeekRead(offNEHeader, SEEKPOS_BEG)
        // Get signature word
        .ReadString(signature, 2);  // Offset 0x0

    /* Test if signature is valid,
       otherwise throw an `InvalidFileSignature` exception.
    if(signature[0]!='N' || signature[1]!='E') {
        throw InvalidFileSignature("NE", signature);
    } */

    // Read Information Block Contents
    uint16_t wordFlag;  // Buffer for word flags;
    file
        .ReadUInt8(linker_version)          // Offset 0x2
        .ReadUInt8(linker_revision)         // Offset 0x3
        .ReadUInt16(entryTable.tblAddress)  // Offset 0x4
        .ReadUInt16(entryTable.tblSize)     // Offset 0x6
        .SeekRead(4)                        // Offset 0x8 (reserved)
        .ReadUInt16(wordFlag);              // Offset 0xc
    moduleFlags.dataSegmnt=(DataSegmntFlag)wordFlag;
    file
        .ReadUInt16(autoDataSegmntIndex)            // Offset 0xE
        .ReadUInt16(szLocalHeap)                    // Offset 0x10
        .ReadUInt16(szStack)                        // Offset 0x12
        .ReadUInt16(ipValue.address)                // Offset 0x14
        .ReadUInt16(ipValue.segmntIndex)
        .ReadUInt16(spValue.address)                // Offset 0x18
        .ReadUInt16(spValue.segmntIndex)
        .ReadUInt16(segmntTblInfo.tblSize)          // Offset 0x1c
        .ReadUInt16(modRefTblInfo.tblSize)          // Offset 0x1e 
        .ReadUInt16(nonResidNameTblInfo.tblSize)    // Offset 0x20
        .ReadUInt16(segmntTblInfo.tblAddress)       // Offset 0x22
        .ReadUInt16(resourceTblAddr)                // Offset 0x24
        .ReadUInt16(residNameTblAddr)               // Offset 0x26
        .ReadUInt16(modRefTblInfo.tblAddress)       // Offset 0x28
        .ReadUInt16(importdNameTblAddr)             // Offset 0x2a
        .ReadUInt32(nonResidNameTblInfo.tblAddress) // Offset 0x2c
        .ReadUInt16(nMovableEntryPts)               // Offset 0x30
        .ReadUInt16(shiftCount)                     // Offset 0x32
        .ReadUInt16(nResourceSegments)             // Offset 0x34
        .ReadUInt8((uint8_t&)wordFlag);             // Offset 0x36
    targetOS.unknown=wordFlag&1>0;
    targetOS.windows=wordFlag&4>0;
    targetOS.reserved=wordFlag&26>0;
    file.ReadUInt8((uint8_t&)wordFlag);  // Offset 0x37
    winFlags.v2x_protectedMode=wordFlag&2>0;
    winFlags.v2x_proportionalFonts=wordFlag&4>0;
    winFlags.fastLoadArea=wordFlag&8>0;
    file
        .ReadUInt16(fastLoadArea.tblAddress)    // Offset 0x38
        .ReadUInt16(fastLoadArea.tblSize)       // Offset 0x3a
        .SeekRead(2)                            // Offset 0x3c (ignored)
        .ReadUInt16(expectdWinVersion);         // Offset 0x3e
}

uint16_t
NEInfoBlock::GetAutoDataSegIndex() const {
    return autoDataSegmntIndex;
}

NETableInfo16_16
NEInfoBlock::GetEntryTblInfo() const {
    return entryTable;
}

uint16_t
NEInfoBlock::GetExpWinVersion() const {
    return expectdWinVersion;
}

NETableInfo16_16
NEInfoBlock::GetFastLoadAreaInfo() const {
    return fastLoadArea;
}

uint16_t
NEInfoBlock::GetImportedNameTblAddress() const {
    return importdNameTblAddr;
}

SegmentOff16
NEInfoBlock::GetIP() const {
    return ipValue;
}

uint16_t
NEInfoBlock::GetInfoBlkOffset() const {
    return offNEHeader;
}

uint16_t
NEInfoBlock::GetLocalHeapInitialSize() const {
    return szLocalHeap;
}

NETableInfo16_16
NEInfoBlock::GetModRefTableInfo() const {
    return modRefTblInfo;
}

uint16_t
NEInfoBlock::GetNbrMovableEntryPts() const {
    return nMovableEntryPts;
}

NETableInfo32_16
NEInfoBlock::GetNonResidentNameTblInfo() const {
    return nonResidNameTblInfo;
}

uint16_t
NEInfoBlock::GetNbrResourceSegments() const {
    return nResourceSegments;
}

uint16_t
NEInfoBlock::GetResidentNameTblAddr() const {
    return residNameTblAddr;
}

uint16_t
NEInfoBlock::GetResourceTblAddr() const {
    return resourceTblAddr;
}

NETableInfo16_16
NEInfoBlock::GetSegmentTblInfo() const {
    return segmntTblInfo;
}

uint32_t
NEInfoBlock::GetSectorOffset(uint16_t index) const {
    return (1<<shiftCount)*index;
}

uint16_t
NEInfoBlock::GetShiftCount() const {
    return shiftCount;
}

uint16_t
NEInfoBlock::GetStackInitialSize() const {
    return szStack;
}

SegmentOff16
NEInfoBlock::GetStackPtr() const {
    return spValue;
}

uint8_t
NEInfoBlock::Linker_GetRevisionNbr() const {
    return linker_revision;
}

uint8_t
NEInfoBlock::Linker_GetVersionNbr() const {
    return linker_version;
}

NESegmentTbl::
NESegmentTbl(IStream& file, NEInfoBlock& infoBlk) {
    NETableInfo16_16 segTable(infoBlk.GetSegmentTblInfo());
    file.SeekRead(segTable.tblAddress+infoBlk.GetInfoBlkOffset(), SEEKPOS_BEG);
    entries.reserve(segTable.tblSize);
    for(uint16_t si=0; si<segTable.tblSize; ++si) {
        NESegmentInfo segInfo;
        uint16_t flags;
        file
            .ReadUInt16(segInfo.offData)
            .ReadUInt16(segInfo.szData)
            .ReadUInt16(flags);
        // std::cout << "[debug] flags equals 0x" << flags << '\n';
        // std::cout << "[debug] bitset equals 0b" << std::bitset<16>(flags) << '\n';
        segInfo.dataSegment=flags&1;
        segInfo.ldrHasAllocdMem=flags&2;
        segInfo.loaded=flags&4;
        segInfo.moveable=flags&16;
        segInfo.pure=flags&32;
        segInfo.preload=flags&64;
        segInfo.writable=(flags&128)==0;
        segInfo.relocInfo=flags&256;
        segInfo.discardable=flags&4096;
        file.ReadUInt16(segInfo.minAllocSz);
        entries.push_back(segInfo);
    }
}

std::vector<NESegmentInfo>
NESegmentTbl::GetEntries() const {
    return entries;
}

NERelocTbl::NERelocTbl(IStream& file, NEInfoBlock& infoBlk, NESegmentInfo& segInfo) {
    // Get Reloc Table Offset and seek
    uint64_t off=infoBlk.GetSectorOffset(segInfo.offData);
    off+=GetSegmentLength(segInfo);
    file.SeekRead(off,SEEKPOS_BEG);
    
    // Get number of entries
    uint16_t nEntries;
    file.ReadUInt16(nEntries);
    entries.resize(nEntries);

    // Read each relocation entry
    uint8_t raw_enum_data;
    for(NERelocItem& entry : entries) {
        // Get relocation-address type
        file.ReadUInt8(raw_enum_data);
        switch(raw_enum_data) {
            case 0:
                entry.relocTarget.addressType=RELOC_ADDR_LOWBYTE;
                break;
            case 2:
                entry.relocTarget.addressType=RELOC_ADDR_SEL16;
                break;
            case 3:
                entry.relocTarget.addressType=RELOC_ADDR_PTR32;
                break;
            case 5:
                entry.relocTarget.addressType=RELOC_ADDR_OFF16;
                break;
            case 11:
                entry.relocTarget.addressType=RELOC_ADDR_PTR48;
                break;
            case 13:
                entry.relocTarget.addressType=RELOC_ADDR_OFF32;
                break;
        }

        // Get relocation source type
        file.ReadUInt8(raw_enum_data);
        switch(raw_enum_data) {
            case 0:
                entry.relocType=INTERNAL_REF;
                break;
            case 1:
                entry.relocType=IMPORT_ORDINAL;
                break;
            case 2:
                entry.relocType=IMPORT_NAME;
                break;
            case 3:
                entry.relocType=OSFIXUP;
                break;
        }
        file.ReadUInt16(entry.relocTarget.offRelocItem);
        switch(entry.relocType) {
            case INTERNAL_REF:
                file
                    // Index to segment or 0x0FF
                    .ReadUInt16(entry.relocSrc.internalRef.segmntIndex)
                    // Offset to segment or entry table ordinal value
                    .ReadUInt16(entry.relocSrc.internalRef.address);
                break;
            case IMPORT_NAME:
            case IMPORT_ORDINAL:
                file
                    // Index to module-reference table
                    .ReadUInt16(entry.relocSrc.importType.modRefTblIndex)
                    // Ordinal value or offset to imported function
                    .ReadUInt16(entry.relocSrc.importType.srcPos);
                break;
        }
    }
}

std::vector<NERelocItem>
NERelocTbl::GetRelocEntries() const {
    return entries;
}