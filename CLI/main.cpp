#include <cstdint>
#include <cstring>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>

#include "../Core/interfaces/NE_Blocks.h"
#include "../Core/interfaces/NEFile.h"
#include "../DataStreams/interfaces/libstdc++.h"
#include "CLI.h"

// Help message dumped when:
// - User input is invalid.
// - -h option is passed.
const char HELP_MSG[] =
"Syntax: nereader-cli [options] <filename> [options]\n\n"
"General options\n"
"  -h               Display this message\n"
"  -o <file_path>   Redirect output to <file_path>\n\n"
"NE File global options\n"
"  -E             Dump Entry Table\n"
"  -I             Dump Info Block\n"
"  -R             Dump Resource Table\n"
"  -S             Dump Segment Table\n"
"  -d <segments>  Disassemble code from <segments>,\n"
"                 where <segments> follows <range-set> syntax.\n";
// TODO: write <range-set> syntax message

void dump_NEInfoBlock(std::ostream& os, const NEInfoBlock& infoBlk) {
    // Offset of NE Info Block.
    // Used for offset computations.
    uint16_t off_infoBlk = infoBlk.GetInfoBlkOffset();
    List_OutputHelper out_list(os);

    out_list
        .WriteSingleEntry("NE Info Block Dump")
        .Increase_Level(">")
        .WritePairEntry("Linker version", infoBlk.Linker_GetVersionNbr(), HEX)
        .WritePairEntry("Linker revision", infoBlk.Linker_GetRevisionNbr(), HEX);
    
    NETableInfo16_16 entryTblInfo(infoBlk.GetEntryTblInfo());
    out_list
        .WriteSingleEntry("Entry Table")
        .Increase_Level("-")
            .WritePairEntry("Offset", entryTblInfo.tblAddress+off_infoBlk, HEX)
            .WritePairEntry("Size", entryTblInfo.tblSize, DEC, "bytes")
        .Decrease_Level()

        .WritePairEntry("Number of movable entry points", infoBlk.GetNbrMovableEntryPts())
        // TODO: print special flags from 0xC offset
        .WritePairEntry("Automatic data segment number", infoBlk.GetAutoDataSegIndex())
        .WritePairEntry("Initial size of the local heap", infoBlk.GetLocalHeapInitialSize(), DEC, "bytes")
        .WritePairEntry("Initial size of the stack", infoBlk.GetStackInitialSize(), DEC, "bytes");
    
    SegmentOff16 ip(infoBlk.GetIP());
    out_list
        .WriteSingleEntry("Program entry point (first IP value)")
        .Increase_Level("-")
            .WritePairEntry("Segment", ip.segmntIndex)
            .WritePairEntry("Offset", ip.address, HEX)
        .Decrease_Level();
        
    SegmentOff16 sp(infoBlk.GetStackPtr());
    out_list
        .WriteSingleEntry("Stack pointer")
        .Increase_Level("-")
            .WritePairEntry("Segment", sp.segmntIndex)
            .WritePairEntry("Offset", sp.address, HEX)
        .Decrease_Level();

    NETableInfo16_16 segTable(infoBlk.GetSegmentTblInfo());
    out_list
        .WriteSingleEntry("Segment table")
        .Increase_Level("-")
            .WritePairEntry("Address", segTable.tblAddress+off_infoBlk, HEX)
            .WritePairEntry("Number of entries", segTable.tblSize)
        .Decrease_Level();

    NETableInfo16_16 modRefTable(infoBlk.GetModRefTableInfo());
    out_list
        .WriteSingleEntry("Module-reference table")
        .Increase_Level("-")
            .WritePairEntry("Address", modRefTable.tblAddress+off_infoBlk, HEX)
            .WritePairEntry("Number of entries", modRefTable.tblSize)
        .Decrease_Level()
        .WritePairEntry("Imported-name table address", infoBlk.GetImportedNameTblAddress()+off_infoBlk, HEX)
        .WritePairEntry("Resident-name table address", infoBlk.GetResidentNameTblAddr()+off_infoBlk, HEX)
        .WritePairEntry("Resource table address", infoBlk.GetResourceTblAddr()+off_infoBlk, HEX)
        .WritePairEntry("Number of resource segments", infoBlk.GetNbrResourceSegments());
    
    NETableInfo32_16 nonResidNameTable(infoBlk.GetNonResidentNameTblInfo());
    out_list
        .WriteSingleEntry("Nonresident-name table")
        .Increase_Level("-")
            .WritePairEntry("Address", nonResidNameTable.tblAddress, HEX)
            .WritePairEntry("Size", nonResidNameTable.tblSize, DEC, "bytes")
        .Decrease_Level();
    
    uint16_t shiftCount = infoBlk.GetShiftCount();
    uint16_t sectorSize = 1<<shiftCount;
    out_list
        .WriteSingleEntry("Logical sector")
        .Increase_Level("-")
            .WritePairEntry("Shift count", shiftCount)
            .WritePairEntry("Size", sectorSize, HEX, "bytes")
        .Decrease_Level();

    NETableInfo16_16 fastLoadArea(infoBlk.GetFastLoadAreaInfo());
    out_list
        .WriteSingleEntry("Fast-load area")
        .Increase_Level("-")
            .WritePairEntry("Address", fastLoadArea.tblAddress*sectorSize, HEX)
            .WritePairEntry("Size", fastLoadArea.tblSize*sectorSize, DEC, "bytes")
        .Decrease_Level();
}

int main(int argc, char* argv[]) {
    // Get CLI args
    CLIArgs args(argc, argv);
    
    if(args.show_helpMsg) {
        // Print help message and exit.
        std::cout << HELP_MSG;
        return 0;
    }

    // Open NE File.
    IFile file(args.path_NEFile.c_str());
    
    if(!file.Good()) {
        auto errcode=errno;
        std::cerr
        << "The file could not be opened.\n"
        << strerror(errcode) << '\n';
        return errcode;
    }

    NEFile ne_file(file);

    if(args.dump_NEInfoBlock) dump_NEInfoBlock(std::cout, ne_file.GetInfoBlock());

    return 0;
}