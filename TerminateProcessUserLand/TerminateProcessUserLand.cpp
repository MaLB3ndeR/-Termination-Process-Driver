#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#define IOCTL_TERMINATE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

using namespace std;

DWORD GetPIDbyProcName(const string& procName) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return 0;
    } 
    PROCESSENTRY32W pe32 = { sizeof(PROCESSENTRY32W) };

    if (Process32FirstW(hSnap, &pe32)) {
        wstring wideProcName(procName.begin(), procName.end());
        do {
            if (_wcsicmp(pe32.szExeFile, wideProcName.c_str()) == 0) {
                CloseHandle(hSnap);
                return pe32.th32ProcessID;
            }
        } while (Process32NextW(hSnap, &pe32));
    }

    CloseHandle(hSnap);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <process_name>" << endl;
        return 1;
    }

    string procName = argv[1];

    // Get PID
    DWORD pid = GetPIDbyProcName(procName);
    if (pid == 0) {
        cout << "Process not found: " << procName << endl;
        return 1;
    }

    cout << "Found process " << procName << " with PID: " << pid << endl;

    // Open device
    HANDLE hDevice = CreateFile(L"\\\\.\\TerminationProcessLink", GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (hDevice == INVALID_HANDLE_VALUE) {
        cout << "Failed to open device, error: " << GetLastError() << endl;
        return 1;
    }

    // Send termination request
    DWORD bytesReturned;
    BOOL status = DeviceIoControl(hDevice, IOCTL_TERMINATE_PROCESS, &pid, sizeof(pid), NULL, 0, &bytesReturned, NULL);

    if (status) {
        cout << "Termination request sent for PID: " << pid << endl;
    }
    else {
        cout << "Failed to send termination request. Error: " << GetLastError() << endl;
    }

    CloseHandle(hDevice);
    return 0;
}