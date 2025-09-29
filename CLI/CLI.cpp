#include <cstdint>
#include <string>
#include <sstream>

#include "CLI.h"

CLIArgs::CLIArgs(int argc, char* argv[]) {
    // Any arguments?
    if(argc==1) {
        // No arguments, enable show_helpMsg
        show_helpMsg=true;
        return;
    }

    // Iterate over each argument
    // First argument (nereader-cli path) is ignored, because it's not useful.
    for(uint32_t arg_index=1; arg_index<argc; ++arg_index) {
        // Get argument string
        std::string arg(argv[arg_index]);
        // Stream for msg string
        std::ostringstream msg_stream(msg);

        // Is arg and option or NE file path?
        if(arg[0]=='-') {
            // It's a CLI option

            if(arg=="-h") {
                show_helpMsg=true;
            } else if(arg=="-o") {
                // Get <file_path>
                ++arg_index;
                arg=argv[arg_index];
                
                // TODO: is <file_path> valid?
                // For now, let's assume it is...
                output_file=arg;
            } else if(arg=="-I") {
                dump_NEInfoBlock=true;
            } else if(arg=="-R") {
                dump_NEResourceTbl=true;
            } else if(arg=="-S") {
                dump_NESegmentTbl=true;
            } else if(arg=="-d") {
                // Get raw <segments>
                ++arg_index;
                arg=argv[arg_index];

                // Parse it
                std::istringstream range_set_stream(arg);
                std::string raw_range;
                while(std::getline(range_set_stream, raw_range, ',')) {
                    std::istringstream range_stream(raw_range);
                    Uint16_Range range;
                    char delim;
                    range_stream >> range.begin >> delim >> range.end;

                    // TODO: accept a single segment number, without range notation.

                    // Is input valid?
                    if(!range_stream || delim!='-') {
                        // Syntax error!
                        valid=false;
                        msg_stream
                            << "Invalid syntax: range " << raw_range << " must be a dash-separated pair of integers.\n"
                            << "Given option: -d " << arg;
                        break;
                    }
                    if(range.begin>range.end) {
                        valid=false;
                        // Invalid Range!
                        msg_stream
                            << "Invalid range (" << raw_range << "): first element must be less or equal than second.\n"
                            << "Given option: -d " << arg;
                        show_helpMsg=true;
                        break;
                    }

                    // Insert range into disasm_SegCode
                    disasm_SegCode.push_back(range);
                }
            } else {
                valid=false;
                msg_stream
                    << "Unrecognized option: " << arg << "\n\n";
                show_helpMsg=true;
                break;
            }
        } else {
            // Must be the NE file path
            // Was it previously passed?
            if(!path_NEFile.empty()) {
                valid=false;
                msg_stream
                    << "Unexpected argument: " << arg;
                break;
            }

            // TODO: is <file_path> valid?
            // For now, let's assume it is...
            path_NEFile=arg;
        }
    }
}

CLI_OutputHelper::CLI_OutputHelper(std::ostream& os)
    : _os(os) {}

CLI_OutputHelper&
CLI_OutputHelper::NewLine(uint32_t extra_lines)
{
    for(uint32_t i=0; i<=extra_lines; ++i)
        _os.put('\n');
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteIdent(uint8_t ident)
{
    for(uint32_t i=0; i<ident; ++i) {
        _os.put(' ');
    }
    return *this;
}

void
CLI_OutputHelper::_SetBase(const Number_Bases base)
{
    switch(base) {
        case BINARY:
            _os.setstate(_os.badbit);
            break;
        case DEC:
            _os << std::dec;
            break;
        case HEX:
            _os << "0x";
            _os << std::hex;
            break;
        case OCTAL:
            _os << "0o";
            _os << std::oct;
            break;
    }
}

CLI_OutputHelper&
CLI_OutputHelper::WriteInt(const uint8_t val, const Number_Bases base)
{
    _SetBase(base);
    _os << (uint16_t)val;
    _SetBase(DEC);
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteInt(const uint16_t val, const Number_Bases base)
{
    _SetBase(base);
    _os << val;
    _SetBase(DEC);
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteInt(const int16_t val, const Number_Bases base)
{
    _SetBase(base);
    _os << val;
    _SetBase(DEC);
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteInt(const uint32_t val, const Number_Bases base)
{
    _SetBase(base);
    _os << val;
    _SetBase(DEC);
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteInt(const int32_t val, const Number_Bases base)
{
    _SetBase(base);
    _os << val;
    _SetBase(DEC);
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteInt(const uint64_t val, const Number_Bases base)
{
    _SetBase(base);
    _os << val;
    _SetBase(DEC);
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteInt(const int64_t val, const Number_Bases base)
{
    _SetBase(base);
    _os << val;
    _SetBase(DEC);
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteString(const char* str) {
    _os << str;
    return *this;
}

CLI_OutputHelper&
CLI_OutputHelper::WriteString(const std::string str) {
    _os << str;
    return *this;
}

List_OutputHelper::List_OutputHelper(
    std::ostream& os,
    const uint8_t ident_mult)
    : CLI_OutputHelper(os), _bullet_stack({""}), _ident_mult(ident_mult) {}

List_OutputHelper&
List_OutputHelper::Increase_Level(const std::string bullet) {
    _bullet_stack.push(bullet);
    _currentLvl++;
    return *this;
}

uint8_t
List_OutputHelper::GetLevel() {
    return _currentLvl;
}

List_OutputHelper&
List_OutputHelper::Decrease_Level(uint8_t count) {
    for(uint8_t i=0; i<count; ++i) {
        _bullet_stack.pop();
    }
    if(_currentLvl-count<0) _currentLvl=0;
    else _currentLvl-=count;
    return *this;
}

List_OutputHelper&
List_OutputHelper::WriteSingleEntry(const std::string str) {
    _WriteEntryPrefix();
    _os << str;
    NewLine();
    return *this;
}

List_OutputHelper&
List_OutputHelper::WritePairEntry(
    const std::string str1,
    const std::string str2
) {
    _WriteEntryPrefix();
    _os << str1 << ": " << str2;
    NewLine();
    return *this;
}

List_OutputHelper&
List_OutputHelper::WritePairEntry(
    const std::string str1,
    const uint8_t val,
    const Number_Bases base,
    const std::string suffix
) {
    _WriteEntryPrefix();
    _os << str1 << ": ";
    WriteInt(val, base);
    _os << ' ' << suffix;
    NewLine();
    return *this;
}

List_OutputHelper&
List_OutputHelper::WritePairEntry(
    const std::string str1,
    const uint16_t val,
    const Number_Bases base,
    const std::string suffix
) {
    _WriteEntryPrefix();
    _os << str1 << ": ";
    WriteInt(val, base);
    _os << ' ' << suffix;
    NewLine();
    return *this;
}

List_OutputHelper&
List_OutputHelper::WritePairEntry(
    const std::string str1,
    const int16_t val,
    const Number_Bases base,
    const std::string suffix
) {
    _WriteEntryPrefix();
    _os << str1 << ": ";
    WriteInt(val, base);
    _os << ' ' << suffix;
    NewLine();
    return *this;
}

List_OutputHelper&
List_OutputHelper::WritePairEntry(
    const std::string str1,
    const uint32_t val,
    const Number_Bases base,
    const std::string suffix
) {
    _WriteEntryPrefix();
    _os << str1 << ": ";
    WriteInt(val, base);
    _os << ' ' << suffix;
    NewLine();
    return *this;
}

List_OutputHelper&
List_OutputHelper::WritePairEntry(
    const std::string str1,
    const int32_t val,
    const Number_Bases base,
    const std::string suffix
) {
    _WriteEntryPrefix();
    _os << str1 << ": ";
    WriteInt(val, base);
    _os << ' ' << suffix;
    NewLine();
    return *this;
}

List_OutputHelper&
List_OutputHelper::WritePairEntry(
    const std::string str1,
    const uint64_t val,
    const Number_Bases base,
    const std::string suffix
) {
    _WriteEntryPrefix();
    _os << str1 << ": ";
    WriteInt(val, base);
    _os << ' ' << suffix;
    NewLine();
    return *this;
}

List_OutputHelper&
List_OutputHelper::WritePairEntry(
    const std::string str1,
    const int64_t val,
    const Number_Bases base,
    const std::string suffix
) {
    _WriteEntryPrefix();
    _os << str1 << ": ";
    WriteInt(val, base);
    _os << ' ' << suffix;
    NewLine();
    return *this;
}