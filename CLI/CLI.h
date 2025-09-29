#ifndef _CLI_H_
#define _CLI_H_

#include <cstdint>
#include <ostream>
#include <stack>
#include <string>
#include <sstream>
#include <vector>

// Range of uint16_t integers, for <range-set> parsing.
// [begin, end]
struct Uint16_Range {
    uint16_t begin; // Smallest integer of range.
    uint16_t end;   // Greatest integer of range.
};

struct CLIArgs {
    // Were CLI args parsed sucessfully?
    bool valid=true;
    // Error message
    // Empty if valid is true.
    std::string msg;

    // General options
    bool show_helpMsg=false; // -h
    std::string output_file; // -o <file_path>

    // NE File Global Options
    bool dump_NEInfoBlock=false; // -I
    bool dump_NEResourceTbl=false; // -R
    bool dump_NESegmentTbl=false; // -S
    std::vector<Uint16_Range> disasm_SegCode; // -d <segments>

    // NE File path
    std::string path_NEFile;

    CLIArgs(int argc, char* argv[]);
};

enum Number_Bases {
    BINARY,
    DEC,  // Decimal (base 10)
    HEX,  // Hexadecimal (base 16)
    OCTAL // Octal (base 8)
};

class CLI_OutputHelper {
protected:
    // Output stream
    std::ostream& _os;

    void _SetBase(const Number_Bases bases);

public:
    CLI_OutputHelper(std::ostream& _os);

    // Leaves the [current line] and creates `extra_lines`
    // lines before a [new line].
    // Example:
    // ```
    // [current line]
    // [empty line]*extra_lines
    // [new line]
    // ```
    CLI_OutputHelper& NewLine(uint32_t extra_lines=0);

    // Write `ident` whitespaces.
    CLI_OutputHelper& WriteIdent(uint8_t ident);

    // > Formatted output
    // Write integers using a specific base
    CLI_OutputHelper& WriteInt(const uint8_t val, const Number_Bases base=DEC);
    CLI_OutputHelper& WriteInt(const uint16_t val, const Number_Bases base=DEC);
    CLI_OutputHelper& WriteInt(const uint32_t val, const Number_Bases base=DEC);
    CLI_OutputHelper& WriteInt(const uint64_t val, const Number_Bases base=DEC);
    CLI_OutputHelper& WriteInt(const int16_t val, const Number_Bases base=DEC);
    CLI_OutputHelper& WriteInt(const int32_t val, const Number_Bases base=DEC);
    CLI_OutputHelper& WriteInt(const int64_t val, const Number_Bases base=DEC);
    // Write string
    CLI_OutputHelper& WriteString(const char* str);
    CLI_OutputHelper& WriteString(const std::string str);
};

class List_OutputHelper : public CLI_OutputHelper {
    void _WriteEntryPrefix() {
        std::string prefix;
        prefix.insert(0, _ident_mult*_currentLvl, ' ');
        
        _os << prefix << _bullet_stack.top();
        if(!_bullet_stack.top().empty())
            _os << ' ';
    }

protected:
    // Stack of bullets for each level;
    std::stack<std::string> _bullet_stack;
    // Current topic level, used for bullet selection and identation.
    uint8_t _currentLvl=0;
    // Multiplier used for topic identation.
    uint8_t _ident_mult;

public:
    List_OutputHelper(std::ostream& os, const uint8_t ident_mult=2);

    // Level control methods
    // Increase level, using `bullet` as prefix for each entry.
    List_OutputHelper& Increase_Level(const std::string bullet="");
    uint8_t GetLevel();
    // Decrease `count` levels.
    List_OutputHelper& Decrease_Level(uint8_t count=1);

    // Write single entry
    List_OutputHelper& WriteSingleEntry(const std::string str);

    // Write pair entry
    List_OutputHelper& WritePairEntry(const std::string str1, const std::string str2);
    List_OutputHelper& WritePairEntry(
        const std::string str, const uint8_t val,
        const Number_Bases base=DEC, const std::string suffix="");
    List_OutputHelper& WritePairEntry(
        const std::string str, const uint16_t val,
        const Number_Bases base=DEC, const std::string suffix="");
    List_OutputHelper& WritePairEntry(
        const std::string str, const uint32_t val,
        const Number_Bases base=DEC, const std::string suffix="");
    List_OutputHelper& WritePairEntry(
        const std::string str, const uint64_t val,
        const Number_Bases base=DEC, const std::string suffix="");
    List_OutputHelper& WritePairEntry(
        const std::string str, const int16_t val,
        const Number_Bases base=DEC, const std::string suffix="");
    List_OutputHelper& WritePairEntry(
        const std::string str, const int32_t val,
        const Number_Bases base=DEC, const std::string suffix="");
    List_OutputHelper& WritePairEntry(
        const std::string str, const int64_t val,
        const Number_Bases base=DEC, const std::string suffix="");
};

// // Implement later
// class Table_OutputHelper : public CLI_OutputHelper {
// protected:
//     std::string _tblTitle; // Table title
//     uint32_t _maxRowSize=0, _maxColumnSize=0;

// public:
//     Table_OutputHelper();
//     Table_OutputHelper(const std::string tblTitle, const uint32_t maxRowSize, const uint32_t maxColumnSize);

//     // TODO: missing methods
// };

#endif