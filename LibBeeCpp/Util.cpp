#include "pch.h"

ULONGLONG TwoDWORDsToULongLong(DWORD low, DWORD high)
{
	return ((ULONGLONG)high << 32) | (ULONGLONG)low;
}
bool isFiletimeEqual(_In_ FILETIME a, _In_ FILETIME b)
{
	return
		a.dwLowDateTime == b.dwLowDateTime
		&& b.dwHighDateTime == b.dwHighDateTime;
}
// ~3 == 0xFFFFFFFC  0b11111..111100
// 0+3 == 3       => 0b0000...000011  => 0
// 1+3 == 4       => 0b0000...000100  => 4
// 2+3 == 5       => 0b0000...000101  => 4
// 3+3 == 6       => 0b0000...000110  => 4
// 4+3 == 7       => 0b0000...000111  => 4
// 5+3 == 8       => 0b0000...001000  => 8
//	alignToDWORDboundary()
// l = (l + 3) & ~3;	// UNALIGNED
DWORD align_to_DWORD(DWORD length)
{
	return (length + (sizeof(DWORD) - 1)) & ~((sizeof(DWORD) - 1));
}
BOOL ConvertFiletimeToLocalTime(const FILETIME* filetime, SYSTEMTIME* localTime)
{
	SYSTEMTIME UTCSysTime;
	if (!FileTimeToSystemTime(filetime, &UTCSysTime))
	{
		return FALSE;
	}

	if (!SystemTimeToTzSpecificLocalTime(NULL, &UTCSysTime, localTime))
	{
		return FALSE;
	}

	return TRUE;
}
std::wstring FiletimeToString(const FILETIME* ft)
{
	if (ft->dwLowDateTime == 0xffffffff && ft->dwHighDateTime == 0xffffffff)
	{
		return std::wstring(L"n/a");
	}

	SYSTEMTIME local;
	if (!ConvertFiletimeToLocalTime(ft, &local))
	{
		return std::wstring(L"err");
	}

	WCHAR buf[32];

	swprintf_s(buf, 32, L"%04u-%02u-%02u %02u:%02u:%02u"
		, local.wYear, local.wMonth, local.wDay
		, local.wHour, local.wMinute, local.wSecond);

	return std::wstring(buf);
}
void prependLongFileNotationToUNCIfNecessary(std::wstring& filename)
{
	if (filename.length() < 3)
	{
		return;
	}

	if (filename[0] == L'\\' && filename[1] == L'\\')
	{
		if (filename[2] == L'?')
		{
			return;
		}
		/*                          012345           */
		/*                          \\server\share   */
		/*                    \\?\UNC\server\share	 */
		filename[0]             = L'C';
		filename.insert(0, L"\\?\\UN");
	}
	else
	{
		// no UNC filename
	}
}
bool starts_with_case_insensitive(_In_ std::wstring_view str, _In_ std::wstring_view prefix)
{
	if (str.length() < prefix.length())
	{
		return false;
	}

	auto IterPos = std::mismatch(
		prefix.begin(), prefix.end(),
		   str.begin(),	   str.end(),
		[](const wchar_t& a, const wchar_t& b) {
			return std::toupper(a) == std::toupper(b);
		});

	return IterPos.first == prefix.end();
}
bool starts_with_any_case_insensitive(_In_ std::wstring_view str, _In_ const std::vector<std::wstring>& prefix_list)
{
	for (auto& prefix : prefix_list)
	{
		if (starts_with_case_insensitive(str, prefix))
		{
			return true;
		}
	}

	return false;
}
bool ends_with_case_insensitive(const std::wstring& str, const std::wstring& suffix)
{
	if (str.length() < suffix.length())
	{
		return false;
	}

	auto IterPos = std::mismatch(
		suffix.rbegin(), suffix.rend(),
		   str.rbegin(),    str.rend(),
		[](const wchar_t& a, const wchar_t& b) {
			return std::toupper(a) == std::toupper(b);
		});

	return IterPos.first == suffix.rend();

}
bool isReadonly(_In_ DWORD dwFileAttributes)
{
	return (dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0;
}
DWORD clearReadonlyFlag(_In_ DWORD dwFileAttributes)
{
	return dwFileAttributes & ~(FILE_ATTRIBUTE_READONLY);
}
std::wstring_view GetFilenameOnly(std::wstring_view fullFilename)
{
	size_t pos = fullFilename.find_last_of(L'\\');
	if (pos == std::string::npos)
	{
		return fullFilename;
	}
	else
	{
		return fullFilename.substr(pos + 1);
	}

}
void cutStringIfLongerThan(std::wstring& str, const size_t maxlen)
{
	if (maxlen < 4)
	{
		return;
	}
	if (str.length() > maxlen)
	{
		str.resize(maxlen);
		str.replace(str.end() - 3, str.end(), 3, L'.');
	}
}
void REG_MULTI_SZ_to_vector_of_strings(std::wstring_view data, std::vector<std::wstring>& strings)
{
	size_t i = 0;
	while ( i < data.length() && data[i] != L'\0') {
		i += strings.emplace_back(&data[i]).length() + 1;
	}
}
void rtrim(std::wstring& s, const wchar_t charToTrim) 
{
	auto pos = std::find_if(
		  s.rbegin()
		, s.rend()
		, [charToTrim](wchar_t ch) { return ch != charToTrim; });

	s.erase(pos.base(), s.end());
}
void ltrim(std::wstring& s, const wchar_t charToTrim)
{
	auto pos = std::find_if(
		s.begin(),
		s.end(),
		[charToTrim](wchar_t ch) {
			return ch != charToTrim;
		}
	);

	s.erase(s.begin(), pos);
}
//--------------------------------------------
bool isDirectory(const DWORD dwFileAttributes)
//--------------------------------------------
{
	return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}
//--------------------------------------------
bool isFile(const DWORD dwFileAttributes)
//--------------------------------------------
{
	return (dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}
//-----------------------------------------------------------------------------------------------
DWORD ReadFileToVector(const HANDLE fp, std::vector<BYTE>* vec, const DWORD chunkLenRead = 4096)
//-----------------------------------------------------------------------------------------------
{
	vec->clear();
	size_t size = 0;

	for (;;)
	{
		vec->resize(size + chunkLenRead);

		DWORD bytesRead;
		if (!ReadFile(fp, vec->data() + size, chunkLenRead, &bytesRead, NULL))
		{
			return GetLastError();
		}
		else if (bytesRead == 0)
		{
			break;
		}
		else
		{
			size += bytesRead;
		}
	}

	vec->resize(size);

	return ERROR_SUCCESS;
}
