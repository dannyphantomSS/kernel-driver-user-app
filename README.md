# 🧠 Kernel Driver + User Application

## 📘 Overview

This project demonstrates a simple **Windows Kernel-Mode Driver** and a **User-Mode Application** communicating through an **IOCTL** interface.  
It’s designed as a learning example for beginners exploring **kernel ↔ user space communication** on Windows using the **Windows Driver Kit (WDK)**.

---

## 🗂️ Project Structure

| File / Directory | Description |
|------------------|--------------|
| `SilentMem.c` | Kernel driver source code — defines device, symbolic link, and IOCTL logic |
| `read_kernel_buffer.c` | User-mode console app that communicates with the driver using `DeviceIoControl` |
| *(Optional future files)* | Build scripts or Visual Studio project setup |

---

## ⚙️ Features

- Creates a kernel device: `\\Device\\SilentMem`  
- Creates a symbolic link: `\\DosDevices\\SilentMem` → accessible in user mode as `\\\\.\\SilentMem`  
- Defines an IOCTL (`IOCTL_READ_KERNEL_BUFFER`) to safely transfer data from kernel memory to user memory  
- Demonstrates **synchronous IOCTL handling** and **basic memory copying** with `RtlCopyMemory`  
- Clean, minimal structure — ideal as a base for more advanced driver projects

---

## 🧩 How It Works

1. The **driver** initializes and registers a device object and symbolic link.  
2. When the **user app** calls `DeviceIoControl()` with `IOCTL_READ_KERNEL_BUFFER`,  
   the driver’s `IRP_MJ_DEVICE_CONTROL` routine is invoked.  
3. The driver copies a small buffer from kernel memory into the user-supplied output buffer.  
4. The user-mode app receives the data and displays or processes it.

---

## 🧱 Building

### 🛠️ Kernel Driver
1. Install **Visual Studio** and the **Windows Driver Kit (WDK)**.  
2. Create a new *Kernel Mode Driver (KMDF or WDM)* project.  
3. Add `SilentMem.c` to the project.  
4. Configure:
   - Target platform: Windows 10 or 11  
   - Architecture: match your system (x64 recommended)  
   - Signing: enable **test mode** or self-sign for local testing  
5. Build → produces a `.sys` file.

### 💻 User Application
1. Create a **Console Application** in Visual Studio.  
2. Add `read_kernel_buffer.c` to the project.  
3. Link against `kernel32.lib`.  
4. Build for the same architecture as the driver.

---

## 🚀 Usage

1. **Load the driver:**
   ```bash
   sc create SilentMem type=kernel binPath=C:\Path\To\SilentMem.sys
   sc start SilentMem
