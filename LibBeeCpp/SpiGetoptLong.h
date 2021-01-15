#pragma once

namespace spi {

    enum class OPTTYPE
    {
        VALUE,
        BOOL
    };

    class GetoptLong
    {
    public:

        using optionCallback_t = std::function<void(std::wstring_view)>;

        std::optional<wchar_t>  _opt;
        wchar_t*                _optLong;
        OPTTYPE                 _type;
        wchar_t*                _desc;
        optionCallback_t        _onOptionCallback;

        GetoptLong(std::optional<wchar_t> opt, wchar_t* optLong, OPTTYPE type, wchar_t* desc, optionCallback_t onOptionCallback);

        static bool Parse(int argc, wchar_t** argv, const std::vector<GetoptLong>& opts, std::vector<std::wstring_view>* arguments, const optionCallback_t& onOptionUnknown);
        static void PrintOptions(const std::vector<GetoptLong> opts);
    };
}