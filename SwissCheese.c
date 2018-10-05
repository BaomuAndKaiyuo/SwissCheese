// This is a demo driver with vulnerbale holes like swiss cheese

#include "ntddk.h"
#define __MODULE__ "SwissCheese"

#define DEVICE_TYPE_SWISSCHEESE   0xdead

#define DEVICE_NAME_SWISSCHEESE   L"\\Device\\SwissCheese"
#define SYMLINK_NAME_SWISSCHEESE  L"\\DosDevices\\SwissCheese"

#define IOCTL_WRITEWHATWHERE        CTL_CODE(DEVICE_TYPE_SWISSCHEESE, 0x800, METHOD_BUFFERED, FILE_ALL_ACCESS)

NTSTATUS
DriverEntry(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath);

VOID
IrpUnloadHandler(
	PDRIVER_OBJECT DriverObject);

NTSTATUS
IrpCreateCloseHandler(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp);

NTSTATUS
IrpDeviceIoCtlHandler(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp);

NTSTATUS
IrpNotImplementedHandler(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp);

EXTERN_C
NTSTATUS
WriteWhatWhereHandler(
	PIRP Irp,
	PIO_STACK_LOCATION IoStackLocation);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT,DriverEntry)
#pragma alloc_text(PAGE, IrpUnloadHandler)
#pragma alloc_text(PAGE, IrpCreateCloseHandler)
#pragma alloc_text(PAGE, IrpDeviceIoCtlHandler)
#pragma alloc_text(PAGE, IrpNotImplementedHandler)
#endif

/// <summary>
/// Driver Entry Point
/// </summary>
/// <param name="DriverObject">The pointer to DRIVER_OBJECT</param>
/// <param name="RegistryPath">The pointer to Unicode string specifying registry path</param>
/// <returns>NTSTATUS</returns>
NTSTATUS
DriverEntry(
	PDRIVER_OBJECT DriverObject,
	PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);

	//assign driver unload routine
	DriverObject->DriverUnload = IrpUnloadHandler;

	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = IrpNotImplementedHandler;
	}

	//assign major function routines
	DriverObject->MajorFunction[IRP_MJ_CREATE] = IrpCreateCloseHandler;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IrpCreateCloseHandler;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IrpDeviceIoCtlHandler;
	

	//initialize the device name and symbolic link name
	UNICODE_STRING DeviceName; 
	RtlInitUnicodeString(&DeviceName, DEVICE_NAME_SWISSCHEESE);
	
	UNICODE_STRING DosDeviceName;
	RtlInitUnicodeString(&DosDeviceName, SYMLINK_NAME_SWISSCHEESE);

	//create device
	NTSTATUS Status;
	PDEVICE_OBJECT DeviceObject = NULL;

	Status = IoCreateDevice(
				DriverObject,
				0,
				&DeviceName,
				DEVICE_TYPE_SWISSCHEESE,
				0,
				FALSE,
				&DeviceObject);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[-] %s!%s IoCreateDevice() failed: %08x\n", __MODULE__, __FUNCTION__, Status);
		goto Exit;
	}

	DbgPrint("[m] %s!%s DeviceName = %ws\n", __MODULE__, __FUNCTION__, DEVICE_NAME_SWISSCHEESE);

	Status = IoCreateSymbolicLink(&DosDeviceName, &DeviceName);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[-] %s!%s IoCreateSymbolicLink() failed: %08x\n", __MODULE__, __FUNCTION__, Status);
		goto Exit;
	}

	DbgPrint("[m] %s!%s SymbolicLinkName = %ws\n", __MODULE__, __FUNCTION__, SYMLINK_NAME_SWISSCHEESE);
	DbgPrint("[+] %s.sys loaded\n", __MODULE__);

	return STATUS_SUCCESS;

Exit:
	if (DeviceObject) 
	{
		IoDeleteDevice(DeviceObject);
	}

	return Status;
}//DriverEntry()

 /// <summary>
 /// Driver Unload Routine
 /// </summary>
 /// <param name="DriverObject">The pointer to DRIVER_OBJECT</param>
 /// <returns>No return value</returns>

VOID
IrpUnloadHandler(
	PDRIVER_OBJECT DriverObject)
{
	PAGED_CODE();

	//delete symbolic link
	NTSTATUS Status;
	
	UNICODE_STRING DosDeviceName;
	RtlInitUnicodeString(&DosDeviceName, SYMLINK_NAME_SWISSCHEESE);

	Status = IoDeleteSymbolicLink(&DosDeviceName);

	if (!NT_SUCCESS(Status))
	{
		DbgPrint("[-] %s!%s IoDeleteSymbolicLink() failed: %08x\n", __MODULE__, __FUNCTION__, Status);
	}

	//unload the driver
	IoDeleteDevice(DriverObject->DeviceObject);
	DbgPrint("[+] %s.sys unloaded\n", __MODULE__);
}//IrpUnloadHandler()

 /// <summary>
 /// Routine for non-implemented majoe functions
 /// </summary>
 /// <param name="DriverObject">The pointer to DRIVER_OBJECT</param>
 /// <returns>NTSTATUS</returns>

NTSTATUS IrpNotImplementedHandler(
	PDEVICE_OBJECT DeviceObject, 
	PIRP Irp) 
{
	//invalid call actually

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();

	DbgPrint("[m] %s!%s DeviceObject = %p, IRP = %p\n", __MODULE__, __FUNCTION__, DeviceObject, Irp);

	Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	Irp->IoStatus.Information = 0;

	// Complete the request
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_NOT_SUPPORTED;
}//IrpNotImplementedHandler()

 /// <summary>
 /// Routine for create and close device
 /// </summary>
 /// <param name="DriverObject">The pointer to DEVICE_OBJECT</param>
 /// <param name="Irp">Irp for this IO request</param>
 /// <returns>NTSTATUS</returns>

NTSTATUS
IrpCreateCloseHandler(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();

	DbgPrint("[m] %s!%s DeviceObject = %p, IRP = %p\n", __MODULE__, __FUNCTION__, DeviceObject, Irp);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}//IrpCreateCloseHandler()

 /// <summary>
 /// Routine for device io control to this device
 /// </summary>
 /// <param name="DriverObject">The pointer to DEVICE_OBJECT</param>
 /// <param name="Irp">Irp for this IO request</param>
 /// <returns>NTSTATUS</returns>

NTSTATUS
IrpDeviceIoCtlHandler(
	PDEVICE_OBJECT DeviceObject,
	PIRP Irp)
{

	UNREFERENCED_PARAMETER(DeviceObject);
	PAGED_CODE();

	NTSTATUS Status = STATUS_NOT_SUPPORTED;
	PIO_STACK_LOCATION IoStackLocation;
	ULONG IoControlCode = 0;

	DbgPrint("[m] %s!%s DeviceObject = %p, IRP = %p\n", __MODULE__, __FUNCTION__, DeviceObject, Irp);

	//retrieve the current I/O Stack Location from the IRP
	IoStackLocation = IoGetCurrentIrpStackLocation(Irp);
	IoControlCode = IoStackLocation->Parameters.DeviceIoControl.IoControlCode;

	//process the IOCTL code
	switch (IoControlCode)
	{
		case IOCTL_WRITEWHATWHERE:
			DbgPrint("[m] %s!%s WRITEWHATWHERE\n", __MODULE__, __FUNCTION__);
			Status = WriteWhatWhereHandler(Irp, IoStackLocation);
			break;
		default:
			DbgPrint("[-] %s!%s invalid IoControlCode = %x\n", __MODULE__, __FUNCTION__, IoControlCode);
			Status = STATUS_INVALID_DEVICE_REQUEST;
			break;
	}

	Irp->IoStatus.Status = Status;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return Status;
}//IrpDeviceIoCtlHandler()