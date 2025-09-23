# 🚀Termination-Process-Driver
A Windows kernel-mode driver that allows privileged process termination from userland applications. This project demonstrates Windows driver development, IOCTL communication, and process management techniques.

Features : 
Kernel-Mode Process Termination: Bypass userland restrictions for process management

Secure IOCTL Communication: Safe communication between userland and kernel-mode

Process Identification: Find processes by name and PID

Admin Privilege Enforcement: Requires elevated privileges for operation

Architecture :

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

Technical Details :

Driver Type: Windows Kernel-Mode Driver (WDM)

Communication: IOCTL with buffered method

APIs Used:

         PsLookupProcessByProcessId

         ObOpenObjectByPointer

         ZwTerminateProcess

         CreateToolhelp32Snapshot (userland)

Installation :

Clone the repository

Enable test signing: 

         bcdedit /set testsigning on

Install driver: 

         sc create serviceName type= kernel binPath= [path]\TerminateProcess.sys

Start driver: 

         sc start serviceName

Proof Of Concept :

<img width="1210" height="269" alt="Screenshot 2025-09-09 235727" src="https://github.com/user-attachments/assets/887aa5a3-0883-4fe7-be4d-cd5bde45d654" />


https://github.com/user-attachments/assets/65e166b3-5009-44ab-8f94-1762dcf78a9b

Related Resources :

[Windows Driver Documentation](https://docs.microsoft.com/en-us/windows-hardware/drivers/)

[WDK Getting Started](https://docs.microsoft.com/en-us/windows-hardware/drivers/gettingstarted/)

[Sending Commands From Your Userland Program to Your Kernel Driver using IOCTL](https://www.ired.team/miscellaneous-reversing-forensics/windows-kernel-internals/sending-commands-from-userland-to-your-kernel-driver-using-ioctl)

[Kernel Process Terminator](https://medium.com/@s12deff/kernel-process-terminator-6e2b6794a312)

