#include "pch.h"

namespace spi
{
    using args_t = std::vector<std::wstring_view>;

    GetoptLong::GetoptLong(std::optional<wchar_t> opt, wchar_t* optLong, OPTTYPE type, wchar_t* desc, optionCallback_t onOptionCallback)
        : _opt(opt), _optLong(optLong), _type(type), _desc(desc), _onOptionCallback(onOptionCallback)
    {
    }

    void GetoptLong::PrintOptions(std::vector<GetoptLong> opts)
    {
        /*
        * Options:
            -b, --basedir=VALUE        appends this basedir to all filenames in the file
            -n, --dryrun               show what would be deleted
            -h, --help                 show this message and exit
        */
        std::wstring line;
        for (const GetoptLong& o : opts)
        {
            line.assign(L"  ");
            if (o._opt.has_value())
            {
                line.push_back(L'-');
                line.push_back(o._opt.value());
                line.push_back(L',');
            }
            else
            {
                line.append(L"   ");
            }

            line.append(L" --");
            line.append(o._optLong);
            if (o._type == spi::OPTTYPE::VALUE)
            {
                line.append(L"=VALUE");
            }

            wprintf(L"%-30s%s\n", line.c_str(), o._desc);
        }
    }

    void ParseShort(args_t::const_iterator* arg, const args_t::const_iterator argEnd, const std::vector<GetoptLong>& opts, const GetoptLong::optionCallback_t& onOptionUnknown)
    {
        const std::wstring_view::const_iterator cEnd = (*arg)->end();
        for (auto c = (*arg)->begin() + 1; c != cEnd; ++c)
        {
            auto foundOpt = std::find_if(
                opts.begin()
                , opts.end()
                , [&c](const GetoptLong& o) {
                return o._opt.has_value() ? o._opt.value() == *c : false;
            });

            if (foundOpt == opts.end())
            {
                onOptionUnknown(std::wstring_view(c, c));
            }
            else
            {
                if (foundOpt->_type == spi::OPTTYPE::BOOL)
                {
                    foundOpt->_onOptionCallback(nullptr);
                }
                else if (foundOpt->_type == spi::OPTTYPE::VALUE)
                {
                    if (c < cEnd)
                    {
                        foundOpt->_onOptionCallback(std::wstring_view{ c,cEnd });
                    }
                    else
                    {
                        ++(*arg);
                        if (*arg != argEnd && !(*arg)->starts_with(L"-") && !(*arg)->starts_with(L"--"))
                        {
                            foundOpt->_onOptionCallback(*(*arg));
                        }
                        else
                        {
                            foundOpt->_onOptionCallback({});
                        }
                    }
                    break;
                }
            }
        }
    }
    void ParseLong(args_t::const_iterator* arg, const args_t::const_iterator argEnd, const std::vector<GetoptLong>& opts, const GetoptLong::optionCallback_t& onOptionUnknown)
    {
        auto c = (*arg)->begin() + 2;
        auto cEnd = (*arg)->end();

        auto f = std::find(c, cEnd, L'=');
        std::wstring_view longOptName{ c,f };

        auto foundOpt = std::find_if(
            opts.begin(), opts.end(),
            [&longOptName](const GetoptLong& o) {
            return longOptName._Equal(o._optLong);
        });

        if (foundOpt == opts.end())
        {
            onOptionUnknown(longOptName);
            return;
        }

        if (foundOpt->_type == spi::OPTTYPE::BOOL)
        {
            foundOpt->_onOptionCallback(nullptr);
        }
        else if (foundOpt->_type == spi::OPTTYPE::VALUE)
        {
            if (f == cEnd)
            {
                if (++(*arg) == argEnd)
                {
                    // long value option with no value
                }
                else
                {
                    foundOpt->_onOptionCallback(**arg);
                }
            }
            else
            {
                foundOpt->_onOptionCallback(std::wstring_view{ f + 1, cEnd });
            }
        }
    }
    bool parse_internal(const args_t args, const std::vector<GetoptLong>& opts, std::vector<std::wstring_view>* arguments, const GetoptLong::optionCallback_t& onOptionUnknown)
    {
        args_t::const_iterator arg = args.begin();
        while (arg != args.end())
        {
            if (arg->starts_with(L"--"))
            {
                if (arg->length() > 2)
                {
                    ParseLong(&arg, args.end(), opts, onOptionUnknown);
                }
                else
                {
                    for (; arg != args.end(); arg++)
                    {
                        arguments->push_back(*arg);
                    }
                    break;
                }
            }
            else if (arg->starts_with(L"-"))
            {
                if (arg->length() > 1)
                {
                    ParseShort(&arg, args.end(), opts, onOptionUnknown);
                }
                else
                {
                    // option is just a '-'. hm?
                }
            }
            else
            {
                arguments->push_back(*arg);
            }
            ++arg;
        }
        return true;
    }
    bool GetoptLong::Parse(int argc, wchar_t** argv, const std::vector<GetoptLong>& opts, std::vector<std::wstring_view>* arguments, const optionCallback_t& onOptionUnknown)
    {
        arguments->clear();

        args_t args(argc);
        for (int i = 0; i < argc; ++i)
        {
            args.push_back( std::wstring_view{ argv[i] } );
        }

        return parse_internal(args, opts, arguments, onOptionUnknown);
    }


}