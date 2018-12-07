#include <ntddk.h>
#include <wdm.h>

#pragma once

#define OLS_TYPE 40000

#define OLS_DRIVER_ID							_T("Driver0")

#define OLS_DRIVER_MAJOR_VERSION				1
#define OLS_DRIVER_MINOR_VERSION				2
#define OLS_DRIVER_REVISION						0
#define OLS_DRIVER_RELESE						5

#define OLS_DRIVER_VERSION \
	((OLS_DRIVER_MAJOR_VERSION << 24) | (OLS_DRIVER_MINOR_VERSION << 16) \
	| (OLS_DRIVER_REVISION << 8) | OLS_DRIVER_RELESE) 

#define IOCTL_OLS_GET_DRIVER_VERSION \
	CTL_CODE(OLS_TYPE, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_OLS_READ_MSR \
	CTL_CODE(OLS_TYPE, 0x821, METHOD_BUFFERED, FILE_ANY_ACCESS)


#pragma pack(push,4)

#pragma pack(pop)