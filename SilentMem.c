// SilentMem.c 
#include <ntddk.h>

#define DRIVER_TAG 'mSiL'

// IOCTL code
#define IOCTL_READ_KERNEL_BUFFER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_DATA)

UNICODE_STRING gDeviceName = RTL_CONSTANT_STRING(L"\\Device\\SilentMem");
UNICODE_STRING gSymLinkName = RTL_CONSTANT_STRING(L"\\DosDevices\\SilentMem");

CHAR gKernelBuffer[] = "Educational kernel buffer - read-only demo";

VOID UnloadDriver(_In_ PDRIVER_OBJECT DriverObject)
{
    UNREFERENCED_PARAMETER(DriverObject);
    IoDeleteSymbolicLink(&gSymLinkName);
    IoDeleteDevice(DriverObject->DeviceObject);
    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "SilentMem: Unloaded\n");
}

NTSTATUS DispatchCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS DispatchIoControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    PIO_STACK_LOCATION  stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    ULONG outBytes = 0;

    if (stack->Parameters.DeviceIoControl.IoControlCode == IOCTL_READ_KERNEL_BUFFER) {
        // METHOD_BUFFERED: SystemBuffer points to user buffer
        PVOID userBuffer = Irp->AssociatedIrp.SystemBuffer;
        ULONG userBufferLength = stack->Parameters.DeviceIoControl.OutputBufferLength;

        SIZE_T toCopy = min((SIZE_T)userBufferLength, sizeof(gKernelBuffer));
        if (toCopy > 0 && userBuffer != NULL) {
            RtlCopyMemory(userBuffer, gKernelBuffer, toCopy);
            outBytes = (ULONG)toCopy;
            status = STATUS_SUCCESS;
        } else {
            status = STATUS_BUFFER_TOO_SMALL;
            outBytes = 0;
        }
    }

    Irp->IoStatus.Status = status;
    Irp->IoStatus.Information = outBytes;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);
    NTSTATUS status;
    PDEVICE_OBJECT deviceObject = NULL;

    status = IoCreateDevice(
        DriverObject,
        0,
        &gDeviceName,
        FILE_DEVICE_UNKNOWN,
        0,
        FALSE,
        &deviceObject
    );

    if (!NT_SUCCESS(status)) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "SilentMem: IoCreateDevice failed 0x%X\n", status);
        return status;
    }

    status = IoCreateSymbolicLink(&gSymLinkName, &gDeviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(deviceObject);
        DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_ERROR_LEVEL, "SilentMem: IoCreateSymbolicLink failed 0x%X\n", status);
        return status;
    }

    // Initialize dispatch table
    for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; ++i) {
        DriverObject->MajorFunction[i] = DispatchCreateClose;
    }
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoControl;
    DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]  = DispatchCreateClose;
    DriverObject->DriverUnload = UnloadDriver;

    DbgPrintEx(DPFLTR_IHVDRIVER_ID, DPFLTR_INFO_LEVEL, "SilentMem: Driver loaded\n");
    return STATUS_SUCCESS;
}
