# SwissCheese
THIS IS A DEMO SAMPLE driver with security holes (vulnerabilities) like a swiss cheese. Please refer to the list to understand the holes in this demo driver. To deploy and start this demo driver in the target computer, please attach a kernel debugger as there is no digital signature signed for this driver. Please carefully use this driver in your LAB. I have no responsibilties if this driver causes any harm on your computer.

To use this driver:
1. Copy SwissCheese.sys to C:\Windows\System32\Drivers
2. sc create SwissCheese type= kernel start= demand error= normal binPath= System32\Drivers\SwissCheese.sys DisplayName= SwissCheese
3. sc query SwissCheese
3. sc start SwissCheese

Holes:

  WriteWhatWhere: the driver writes the data (what) to the target kernel address (where) passed by IOCTL

