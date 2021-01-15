#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TestLibMsgProps
{
	TEST_CLASS(TestReadFileToVector)
	{
	public:
		DWORD CreateTestFile(const std::string& content, HANDLE* fp)
		{
			WCHAR tmpFilename[MAX_PATH];
			if ( UINT unique; unique = GetTempFileNameW(
				L"c:\\temp"
				, L"pst"
				, 0
				, tmpFilename) == 0 ) 
			{
				return GetLastError();
			}
			else if ((*fp = CreateFileW(
				tmpFilename
				, GENERIC_READ | GENERIC_WRITE
				, FILE_SHARE_READ | FILE_SHARE_WRITE
				, NULL
				, OPEN_EXISTING
				, FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE
				, NULL)) == INVALID_HANDLE_VALUE)
			{
				return GetLastError();
			}
			else if (DWORD written; !WriteFile(*fp, content.data(), content.length(), &written, NULL))
			{
				return GetLastError();
			}
			else
			{
			}

			return 0;
		}
		TEST_METHOD(simple)
		{
			std::string in{ "Bernhard" };

			HANDLE fp;
			Assert::AreEqual<DWORD>   (0, CreateTestFile(in, &fp));
			Assert::AreNotEqual<DWORD>(INVALID_SET_FILE_POINTER, SetFilePointer(fp, 0, NULL, FILE_BEGIN));

			std::vector<BYTE> vec;
			Assert::AreEqual<DWORD>(0, ReadFileToVector(fp, &vec, 3));
			Assert::IsTrue(CloseHandle(fp) == TRUE);

			Assert::AreEqual(in.length(), vec.size());

			Assert::IsTrue(std::equal(in.begin(), in.end(), vec.begin(), vec.end(),
				[](const char i, const BYTE v) {
					return i == v;
				}));
		}
	};
}
