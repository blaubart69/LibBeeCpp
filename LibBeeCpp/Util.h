#pragma once

ULONGLONG TwoDWORDsToULongLong(DWORD low, DWORD high);
bool isFiletimeEqual(_In_ FILETIME a, _In_ FILETIME b);
BOOL ConvertFiletimeToLocalTime(const FILETIME* filetime, SYSTEMTIME* localTime);
std::wstring FiletimeToString(const FILETIME* ft);
bool starts_with_case_insensitive(_In_ std::wstring_view str, _In_ std::wstring_view prefix);
bool ends_with_case_insensitive(const std::wstring& str, const std::wstring& suffix);
bool starts_with_any_case_insensitive(_In_ std::wstring_view str, _In_ const std::vector<std::wstring>& prefix_list);
bool isReadonly(_In_ DWORD dwFileAttributes);
DWORD clearReadonlyFlag(_In_ DWORD dwFileAttributes);
std::wstring_view GetFilenameOnly(std::wstring_view fullFilename);
void cutStringIfLongerThan(std::wstring& str, const size_t maxlen);
void REG_MULTI_SZ_to_vector_of_strings(std::wstring_view data, std::vector<std::wstring>& strings);
void rtrim(std::wstring& s, const wchar_t charToTrim);
void ltrim(std::wstring& s, const wchar_t charToTrim);
bool isDirectory(const DWORD dwFileAttributes);
bool isFile(const DWORD dwFileAttributes);
DWORD align_to_DWORD(DWORD length);
DWORD ReadFileToVector(const HANDLE fp, std::vector<BYTE>* vec, const DWORD chunkLenRead);