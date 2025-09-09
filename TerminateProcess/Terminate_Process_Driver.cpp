#include <ntifs.h>
#include <ntddk.h>

#define PROCESS_TERMINATE (0x0001)
#define IOCTL_GET_PID CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_TERMINATE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

void ProcessUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS ProcessCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS ProcessDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);

// DriverEntry
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    KdPrint(("Process Termination Driver Loaded\n"));

    // Setup driver routines
    DriverObject->DriverUnload = ProcessUnload;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = ProcessCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = ProcessCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ProcessDeviceControl;

    // Create device and symbolic link
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\TerminationProcess");
    UNICODE_STRING symLinkName = RTL_CONSTANT_STRING(L"\\??\\TerminationProcessLink");

    PDEVICE_OBJECT deviceObject;
    NTSTATUS status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);

    if (!NT_SUCCESS(status)) {
        KdPrint(("Failed to Create Device\n"));
        return status;
    }

    status = IoCreateSymbolicLink(&symLinkName, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        KdPrint(("Failed to Create Symbolic Link\n"));
        return status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS ProcessDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;

    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_TERMINATE_PROCESS:
    {
        if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(ULONG)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }

        ULONG ProcessId = *(ULONG*)Irp->AssociatedIrp.SystemBuffer;
        KdPrint(("Attempting to terminate process PID: %lu\n", ProcessId));

        PEPROCESS Process;
        status = PsLookupProcessByProcessId((HANDLE)ProcessId, &Process);
        if (!NT_SUCCESS(status)) {
            KdPrint(("Failed to find process PID %lu: 0x%X\n", ProcessId, status));
            break;
        }

        HANDLE ProcessHandle;
        status = ObOpenObjectByPointer(
            Process,
            OBJ_KERNEL_HANDLE,
            NULL,
            PROCESS_TERMINATE,
            *PsProcessType,
            KernelMode,
            &ProcessHandle
        );

        // dereference the process object
        ObDereferenceObject(Process);

        if (!NT_SUCCESS(status)) {
            KdPrint(("Failed to open handle to process PID %lu: 0x%X\n", ProcessId, status));
            break;
        }

        // Terminate the process
        status = ZwTerminateProcess(ProcessHandle, STATUS_SUCCESS);
        ZwClose(ProcessHandle);

        if (NT_SUCCESS(status)) {
            KdPrint(("Successfully terminated process PID: %lu\n", ProcessId));
        }
        else {
            KdPrint(("Failed to terminate process PID %lu: 0x%X\n", ProcessId, status));
        }
        break;
    }
    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        KdPrint(("Unknown IOCTL received: 0x%X\n", stack->Parameters.DeviceIoControl.IoControlCode));
        break;
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

// Unload
void ProcessUnload(PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING symLinkName = RTL_CONSTANT_STRING(L"\\??\\TerminationProcessLink");

    IoDeleteSymbolicLink(&symLinkName);
    IoDeleteDevice(DriverObject->DeviceObject);

    KdPrint(("Process Termination Driver Unloaded\n"));
}

// Create/Close
NTSTATUS ProcessCreateClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);

    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);

    return STATUS_SUCCESS;
}