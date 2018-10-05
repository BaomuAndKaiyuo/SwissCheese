#include "ntddk.h"
#define __MODULE__ "SwissCheese"


EXTERN_C
NTSTATUS
WriteWhatWhereHandler(
	PIRP Irp,
	PIO_STACK_LOCATION IoStackLocation);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, WriteWhatWhereHandler)
#endif

/// <summary>
/// Routine for device io control to this device
/// </summary>
/// <param name="Irp">Irp for this IO request</param>
/// <param name="IoStackLocation">pointer to IO_STACK_LOCATION</param>
/// <returns>NTSTATUS</returns>

NTSTATUS
WriteWhatWhereHandler(
	PIRP Irp,
	PIO_STACK_LOCATION IoStackLocation)
{

	DbgPrint("[m] %s!%s Irp = %p, IoStackLocation = %p\n", __MODULE__, __FUNCTION__, Irp, IoStackLocation);

	ULONG BytesReturned = 0;
	NTSTATUS Status = STATUS_UNSUCCESSFUL;

	ULONG BufferLength = IoStackLocation->Parameters.DeviceIoControl.InputBufferLength;

	//we need at least a pointer and 1-byte data
	if (BufferLength < (sizeof(PVOID) + sizeof(UCHAR)))
	{
		DbgPrint("[-] %s!%s invalid BufferLength = %x\n", __MODULE__, __FUNCTION__, BufferLength);
		BytesReturned = 0;
		Status = STATUS_INVALID_PARAMETER;
		
		return Status;
	}

	DbgPrint("[m] %s!%s BufferLength = %x\n", __MODULE__, __FUNCTION__, BufferLength);

	PVOID Where = *((PVOID*)Irp->AssociatedIrp.SystemBuffer);
	PUCHAR What = (PUCHAR)Irp->AssociatedIrp.SystemBuffer + sizeof(PVOID);
	
	RtlCopyMemory(Where, What, BufferLength - sizeof(PVOID));
	
	return STATUS_SUCCESS;
}

