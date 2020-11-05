#include "pch.h"
#include <iostream>
#include <vector>
#include <Windows.h>

using namespace std;

void WINAPI TestDll() 
{
	cout << "Test message\n" << endl;
}

void WINAPI ChangeString(const char *oldString, const char *newString) 
{
	HANDLE currentProcess = GetCurrentProcess();
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	MEMORY_BASIC_INFORMATION VASMemInfo;

	size_t changeStringLength = strlen(oldString);
	vector<char> memBuffer;
	char *startMem = (char *)sysInfo.lpMinimumApplicationAddress;

	while (startMem < sysInfo.lpMaximumApplicationAddress) 
	{
		size_t queryInfoSize = VirtualQueryEx(currentProcess, startMem, &VASMemInfo, sizeof(VASMemInfo));
		if (queryInfoSize != 0)
		{
			if ((VASMemInfo.State == MEM_COMMIT) && (VASMemInfo.Protect == PAGE_READWRITE)) 
			{
				memBuffer.resize(VASMemInfo.RegionSize);
				startMem = (char *)VASMemInfo.BaseAddress;
				SIZE_T  transferedBytes;
				int number = ReadProcessMemory(currentProcess, startMem, &memBuffer[0], VASMemInfo.RegionSize, &transferedBytes);
				if (number != 0)
				{
					for (SIZE_T i = 0; i < (transferedBytes - changeStringLength); i++)
					{
						int result = memcmp(oldString, &memBuffer[i], changeStringLength);
						if (result == 0) 
						{
							char *changeAddress = (char *)startMem + i;
							for (SIZE_T j = 0; j < changeStringLength; j++)
							{
								changeAddress[i] = newString[i];
							}
						}
					}
				}
			}

			startMem += VASMemInfo.RegionSize;
		}

		
	}
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		ChangeString("name", "word");
		break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

