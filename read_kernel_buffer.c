// read_kernel_buffer.c - user app to read the kernel buffer via DeviceIoControl
#include <windows.h>
#include <stdio.h>

#define IOCTL_READ_KERNEL_BUFFER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_READ_DATA)

int main(void) {
    HANDLE h = CreateFileA("\\\\.\\SilentMem", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (h == INVALID_HANDLE_VALUE) {
        printf("Failed to open device (error %u)\n", GetLastError());
        return 1;
    }

    CHAR buffer[256] = {0};
    DWORD returned = 0;
    BOOL ok = DeviceIoControl(h, IOCTL_READ_KERNEL_BUFFER, NULL, 0, buffer, sizeof(buffer), &returned, NULL);
    if (!ok) {
        printf("DeviceIoControl failed %u\n", GetLastError());
    } else {
        printf("Read %u bytes: \"%s\"\n", returned, buffer);
    }

    CloseHandle(h);
    return 0;
}
