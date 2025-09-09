# 🚀Termination-Process-Driver
A Windows kernel-mode driver that allows privileged process termination from userland applications. This project demonstrates Windows driver development, IOCTL communication, and process management techniques.

📋 Features
Kernel-Mode Process Termination: Bypass userland restrictions for process management

Secure IOCTL Communication: Safe communication between userland and kernel-mode

Process Identification: Find processes by name and PID

Admin Privilege Enforcement: Requires elevated privileges for operation

🏗️ Architecture

Userland Application (TerminateProcessUserLand.exe)

         │
         │ IOCTL Communication
         ▼

Kernel Driver (TerminateProcess.sys)

         │
         │ Kernel API Calls
         ▼
    
    Windows Kernel
    
         │
         │ Process Management
         ▼
    
    Target Process

🔧 Technical Details

Driver Type: Windows Kernel-Mode Driver (WDM)

Communication: IOCTL with buffered method

APIs Used:

  PsLookupProcessByProcessId

  ObOpenObjectByPointer

  ZwTerminateProcess

  CreateToolhelp32Snapshot (userland)

Installation

Clone the repository

Open solution in Visual Studio

Build both driver and userland application

Enable test signing: bcdedit /set testsigning on

Install driver: sc create TerminProc type= kernel binPath= [path]\TerminateProcess.sys

Start driver: sc start TerminProc

🔗 Related Resources

[Windows Driver Documentation](https://docs.microsoft.com/en-us/windows-hardware/drivers/)

[WDK Getting Started](https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/)

[Sending Commands From Your Userland Program to Your Kernel Driver using IOCTL](https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/sending-commands-from-userland-to-your-kernel-driver-using-ioctl)

