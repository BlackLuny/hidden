#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <thread>
#include <mutex>
#include <stdio.h>

#include "../HiddenLib/HiddenLib.h"

using namespace std;

class CHandle
{
private:
	DWORD m_error;
	HANDLE m_handle;

public:
	CHandle(HANDLE handle) : m_handle(handle), m_error(::GetLastError()) { }
	~CHandle() { if (m_handle != INVALID_HANDLE_VALUE) ::CloseHandle(m_handle); }

	HANDLE get() { return m_handle; }
	DWORD error() { return m_error; }
};

void do_regmon_tests(HidContext context)
{
	HidStatus  hid_status;
	HidObjId objId[3];

	wcout << L"--------------------------------" << endl;
	wcout << L"Registry monitor tests result:" << endl;
	wcout << L"--------------------------------" << endl;

	try
	{
		// Test 1
		wcout << L"hiding HARDWARE\\ACPI\\DSDT\\PTLTD_" << endl;

		hid_status = Hid_AddHiddenRegKey(context, HidRegRootTypes::RegHKLM, L"HARDWARE\\ACPI\\DSDT\\PTLTD_", &objId[0]);
		if (!HID_STATUS_SUCCESSFUL(hid_status))
		{
			wcout << L"Error, Hid_AddHiddenRegKey() failed with code: " << HID_STATUS_CODE(hid_status) << endl;
			throw exception();
		}
		wcout << L" successful!" << endl;

	}
	catch (exception&)
	{
		wcout << L" failed!" << endl;
	}
}

void disable_wow64_redirection()
{
#ifndef _AMD64
	BOOL wow64 = FALSE;
	PVOID value;

	IsWow64Process(GetCurrentProcess(), &wow64);

	if (wow64)
		Wow64DisableWow64FsRedirection(&value);
#endif
}

int wmain(int argc, wchar_t* argv[])
{
	HidContext hid_context;
	HidStatus  hid_status;

	srand((int)time(0));

	hid_status = Hid_Initialize(&hid_context);
	if (!HID_STATUS_SUCCESSFUL(hid_status))
	{
		cout << "Error, HiddenLib initialization failed with code: " << HID_STATUS_CODE(hid_status) << endl;
		return 1;
	}

	disable_wow64_redirection();

	do_regmon_tests(hid_context);

	Hid_Destroy(hid_context);

	return 0;
}
