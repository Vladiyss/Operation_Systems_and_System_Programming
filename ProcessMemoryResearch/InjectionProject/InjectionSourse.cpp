#include <iostream>
#include <Windows.h>
#include <string>
#include <intsafe.h>
#include <tlhelp32.h>

using namespace std;

DWORD GetProcessToInjectID(const char *processName) 
{
	PROCESSENTRY32 processEntry;
	SecureZeroMemory(&processEntry, sizeof(processEntry));
	processEntry.dwSize = sizeof(PROCESSENTRY32);

	DWORD processID = 0;
	HANDLE currentSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	bool found = false;
	if (Process32First(currentSnapshot, &processEntry))
	{
		while ((Process32Next(currentSnapshot, &processEntry) != ERROR_NO_MORE_FILES) && !found) 
		{
			if (strcmp(processEntry.szExeFile, processName) == 0) 
			{
				processID = processEntry.th32ProcessID;
				found = true;
			}
		}
	}

	if (currentSnapshot != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(currentSnapshot);
	}
	return processID;
}

bool InjectDllIntoProcess(DWORD pID, string dllName)
{
	bool success = false;

	HANDLE processToInjectHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	if (processToInjectHandle == NULL)
	{
		cout << "Can't open a process\n" << endl;
	}
	else 
	{
		LPVOID exportFunctionAddress = (LPVOID)GetProcAddress(GetModuleHandle("kernel32"), "LoadLibraryA");
		if (exportFunctionAddress == NULL) 
		{
			cout << "Can't find LoadLibraryA function\n" << endl;
		}
		else 
		{
			LPVOID dllAddress = VirtualAllocEx(processToInjectHandle, NULL, dllName.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			if (dllAddress == NULL)
			{
				cout << "Can't allocate space in specified process\n" << endl;
			}
			else 
			{
				int number = WriteProcessMemory(processToInjectHandle, dllAddress, dllName.c_str(), dllName.length() + 1, NULL);
				if (number == 0) 
				{
					cout << "Can't write in a VAS of specified process\n" << endl;
				}
				else 
				{
					HANDLE remoteThread = CreateRemoteThread(processToInjectHandle, NULL, 65536, (LPTHREAD_START_ROUTINE)exportFunctionAddress, dllAddress, 0, NULL);
					if (remoteThread == NULL)
					{
						cout << "Can't create remote thread in a specified process\n" << endl;
					}
					else 
					{
						success = true;
					}
				}
			}
		}
	}

	if (processToInjectHandle != INVALID_HANDLE_VALUE) 
	{
		CloseHandle(processToInjectHandle);
	}
	return success;
}

void main()
{
	const char *processToInjectName = "ProbeProject.exe";
	DWORD processToInjectID = GetProcessToInjectID(processToInjectName);

	if (processToInjectID != 0) 
	{
		cout << "ProcessToInjectName - " << processToInjectName << "\n" << endl;
		cout << "ProcessToInjectID - " << processToInjectID << "\n" << endl;
		if (!InjectDllIntoProcess(processToInjectID, "ExecutableDll.dll")) 
		{
			cout << "Can't embedd dll\n" << endl;
		}
		else 
		{
			cout << "Dll was successfully embedded\n" << endl;
		}
	}
	else 
	{
		cout << "Can't find a process" << endl;
	}

	char consoleChar = getchar();
}
