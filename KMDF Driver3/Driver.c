#include <ntddk.h>
#include <stddef.h>
#include <devioctl.h>
#include <intrin.h>
#include "Header.h"

static ULONG refCount;

NTSTATUS DriverEntry (IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status;
	UNICODE_STRING  ntDeviceName;
	UNICODE_STRING  win32DeviceName;
	PDEVICE_OBJECT  deviceObject = NULL;

#if DBG
	DbgPrint("=Example= In DriverEntry.");
	DbgPrint("=Example= RegistryPath = %ws.", RegistryPath->Buffer);
#endif



	RtlInitUnicodeString(&ntDeviceName, NT_DEVICE_NAME);

	status = IoCreateDevice(
		DriverObject,					// Our Driver Object
		0,								// We don't use a device extension
		&ntDeviceName,					// Device name 
		OLS_TYPE,						// Device type
		FILE_DEVICE_SECURE_OPEN,		// Device characteristics
		FALSE,							// Not an exclusive device
		&deviceObject);

	if (!NT_SUCCESS(status))
	{
		refCount = (ULONG)-1;
		return status;
	}
	else
	{
		refCount = 0;
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = OlsDispatch;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = OlsDispatch;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = OlsDispatch;
	DriverObject->DriverUnload = Unload;

	RtlInitUnicodeString(&win32DeviceName, DOS_DEVICE_NAME);

	status = IoCreateSymbolicLink(&win32DeviceName, &ntDeviceName);

	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(deviceObject);
	}

	return status;
}

NTSTATUS OlsDispatch (IN PDEVICE_OBJECT fdo, IN	PIRP pIrp)
{
	PIO_STACK_LOCATION pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
		NTSTATUS status;

	ULONG ControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;


#if DBG
	DbgPrint("-Example- In DeviceControlRoutine (fdo= %X)\n", fdo);
	DbgPrint("-Example- DeviceIoControl: IOCTL %x.", ControlCode);
#endif

	pIrp->IoStatus.Information = 0;
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);

	status = STATUS_NOT_IMPLEMENTED;

	switch (pIrpStack->MajorFunction)
	{
	case IRP_MJ_CREATE:
		if (refCount != (ULONG)-1) { refCount++; }
		status = STATUS_SUCCESS;
		break;
	case IRP_MJ_CLOSE:
		if (refCount != (ULONG)-1) { refCount--; }
		status = STATUS_SUCCESS;
		break;

	case IRP_MJ_DEVICE_CONTROL:
		switch (pIrpStack->Parameters.DeviceIoControl.IoControlCode)
		{
		case IOCTL_OLS_GET_DRIVER_VERSION:
			*(PULONG)pIrp->AssociatedIrp.SystemBuffer = OLS_DRIVER_VERSION;
			pIrp->IoStatus.Information = 4;
			status = STATUS_SUCCESS;
			break;

		case IOCTL_OLS_READ_MSR:
			status = ReadMSR(
				pIrp->AssociatedIrp.SystemBuffer,
				//pIrpStack->Parameters.DeviceIoControl.InputBufferLength,
				pIrp->AssociatedIrp.SystemBuffer,
				//pIrpStack->Parameters.DeviceIoControl.OutputBufferLength,
				(ULONG*)&pIrp->IoStatus.Information
				);
			break;

		}
		break;
	}

	pIrp->IoStatus.Status = status;

	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;
}

VOID Unload (PDRIVER_OBJECT DriverObject)
{
	PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;
	UNICODE_STRING win32NameString;

	PAGED_CODE();

	RtlInitUnicodeString(&win32NameString, DOS_DEVICE_NAME);

	IoDeleteSymbolicLink(&win32NameString);

	if (deviceObject != NULL)
	{
		IoDeleteDevice(deviceObject);
	}
}


NTSTATUS ReadMSR(void	*lpInBuffer,
	void	*lpOutBuffer,
	ULONG	*lpBytesReturned)
{
	__try
	{
		ULONGLONG data =  __readmsr(*(ULONG*)lpInBuffer);
		memcpy((PULONG)lpOutBuffer, &data, 8);
		*lpBytesReturned = 8;
		return STATUS_SUCCESS;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		*lpBytesReturned = 0;
		return STATUS_UNSUCCESSFUL;
	}
}



