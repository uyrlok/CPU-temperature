#include <ntddk.h>
#include <devioctl.h>
#include "Header1.h"

#define NT_DEVICE_NAME	L"\\Device\\Driver0"
#define DOS_DEVICE_NAME	L"\\DosDevices\\Driver0"




NTSTATUS	DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);

NTSTATUS	OlsDispatch(IN PDEVICE_OBJECT pDO, IN PIRP pIrp);

VOID		Unload(IN PDRIVER_OBJECT DriverObject);
 
NTSTATUS	ReadMSR(void *lpInBuffer,
					void *lpOutBuffer, 
					ULONG *lpBytesReturned);
