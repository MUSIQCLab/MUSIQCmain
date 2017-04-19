import ctypes

dllpath = "C:\\Documents and Settings\\Administrator\\My Documents\\VIs\\magiq\\PulserLib.dll"

#dll = ctypes.WinDLL(dllpath)
dll = ctypes.CDLL(dllpath)

slot = ctypes.c_uint16(4)

#dll.mw3Init( slot )
#dll.mw3Reset( slot )

dll.pulserInit()

version = dll.pulserGetFwVersion( slot )
print 'Firmware version',version

dll.mw3Init( slot )
dll.mw3ShutDown( slot )

