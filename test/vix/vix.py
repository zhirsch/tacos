"""
A Pythonic wrapper around the VIX library.
"""
from __future__ import with_statement
import sys
import ctypes

from vixconst import *

# Load the vix shared library
if sys.platform == 'linux2':
    _vixdll = ctypes.CDLL('libvixAllProducts.so')
elif sys.platform == 'win32':
    ctypes.windll.kernel32.SetDllDirectoryA(r'C:\Program Files\VMware\VMware VIX\ws-5\32bit')
    _vixdll = ctypes.CDLL('vix.dll')
else:
    raise OSError("Unsupported operating system: %s" % sys.platform)

class _VixHandle(ctypes.c_int):
    def __enter__(self):
        return self
    def __exit__(self, *exc_info):
        self.Release()
    def __del__(self):
        self.Release()
    def Release(self):
        if self.value != VIX_INVALID_HANDLE:
            _vixdll.Vix_ReleaseHandle(self)
    close = Release

class _Job(_VixHandle):
    def Wait(self, *args):
        L = [self] + list(args) + [VIX_PROPERTY_NONE]
        err = _vixdll.VixJob_Wait(*L)
        if err != VIX_OK:
            raise VixException(err)

class Snapshot(_VixHandle):
    pass

# Define the return type of each of the functions
_vixdll.VixHost_Connect.restype = _Job
_vixdll.VixHost_Disconnect.restype = None
_vixdll.VixVM_Open.restype = _Job
_vixdll.VixVM_PowerOn.restype = _Job
_vixdll.VixVM_PowerOff.restype = _Job
_vixdll.VixVM_WaitForToolsInGuest.restype = _Job
_vixdll.VixVM_LoginInGuest.restype = _Job
_vixdll.VixVM_RunProgramInGuest.restype = _Job
_vixdll.VixVM_GetNamedSnapshot.restype = ctypes.c_longlong # VixError
_vixdll.VixVM_RevertToSnapshot.restype = _Job
_vixdll.VixVM_CreateDirectoryInGuest.restype = _Job
_vixdll.VixVM_CopyFileFromHostToGuest.restype = _Job
_vixdll.VixVM_EnableSharedFolders.restype = _Job
_vixdll.VixVM_AddSharedFolder.restype = _Job
_vixdll.VixJob_Wait.restype = ctypes.c_longlong # VixError
_vixdll.Vix_GetErrorText.restype = ctypes.c_char_p # const char *

class Host(object):
    """
    A host that has a version of VMware installed on it.
    """
    def __init__(self):
        """
        Create a new instance.
        """
        self._hosth = _VixHandle(VIX_INVALID_HANDLE)

    def __del__(self):
        self.close()

    def Connect(self, apiVersion=VIX_API_VERSION,
                hostType=VIX_SERVICEPROVIDER_DEFAULT,
                hostName=None, hostPort=0,
                userName=None, password=None, options=0):
        """
        Open a connection to a VIX host. Defaults to any local installation with
        the current user's credentials.
        """
        with _vixdll.VixHost_Connect(apiVersion, hostType, hostName, hostPort, userName, password, options,
                                     VIX_INVALID_HANDLE, None, None) as jobh:
            jobh.Wait(VIX_PROPERTY_JOB_RESULT_HANDLE, ctypes.byref(self._hosth))

    def Disconnect(self):
        """
        Close the connection to the VIX host.
        """
        _vixdll.VixHost_Disconnect(self._hosth)
        self._hosth = _VixHandle(VIX_INVALID_HANDLE)

    close = Disconnect

    def OpenVM(self, vmxFilePathName):
        """
        Open a virtual machine.
        Return a new instance of the ``VM`` class.
        """
        with _vixdll.VixVM_Open(self._hosth, vmxFilePathName, None, None) as jobh:
            vmh = _VixHandle(VIX_INVALID_HANDLE)
            jobh.Wait(VIX_PROPERTY_JOB_RESULT_HANDLE, ctypes.byref(vmh))
            return VM(vmh)

class VM(object):
    def __init__(self, vmh):
        """
        Create a new instance.
        """
        self._vmh = vmh

    def __del__(self):
        self.close()

    def PowerOn(self, powerOnOptions):
        """
        Power on the virtual machine.
        """
        jobh = _vixdll.VixVM_PowerOn(self._vmh, powerOnOptions, VIX_INVALID_HANDLE, None, None)
        jobh.Wait()

    def PowerOff(self):
        """
        Power off the virtul machine.
        """
        jobh = _vixdll.VixVM_PowerOff(self._vmh, 0, None, None)
        jobh.Wait()

    def WaitForToolsInGuest(self, timeoutInSeconds=0):
        """
        Wait for the guest tools to start up in the guest.
        """
        jobh = _vixdll.VixVM_WaitForToolsInGuest(self._vmh, timeoutInSeconds, None, None)
        jobh.Wait()

    def LoginInGuest(self, userName, password):
        """
        Authenticate the user in the guest.
        """
        jobh = _vixdll.VixVM_LoginInGuest(self._vmh, userName, password, 0, None, None)
        jobh.Wait()

    def RunProgramInGuest(self, guestProgramName, commandLineArgs, options=0):
        """
        Run a program in the guest.
        """
        jobh = _vixdll.VixVM_RunProgramInGuest(self._vmh, guestProgramName, commandLineArgs, options,
                                               VIX_INVALID_HANDLE, None, None)
        pid = ctypes.c_longlong()
        elapsedTime = ctypes.c_int()
        exitCode = ctypes.c_int()
        jobh.Wait(VIX_PROPERTY_JOB_RESULT_PROCESS_ID,
                  ctypes.byref(pid),
                  VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME,
                  ctypes.byref(elapsedTime),
                  VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE,
                  ctypes.byref(exitCode))
        return (pid, elapsedTime, exitCode)

    def GetNamedSnapshot(self, name):
        """
        Get an existing snapshot by name.
        """
        snapshoth = Snapshot()
        err = _vixdll.VixVM_GetNamedSnapshot(self._vmh, name, ctypes.byref(snapshoth))
        if err != VIX_OK:
            raise VixException(err)
        return snapshoth

    def RevertToSnapshot(self, snapshot, options=0):
        """
        Revert to an existing snapshot.
        """
        jobh = _vixdll.VixVM_RevertToSnapshot(self._vmh, snapshot, options,
                                              VIX_INVALID_HANDLE, None, None)
        jobh.Wait()

    def CreateDirectoryInGuest(self, path):
        """
        Create a directory in the guest.
        """
        jobh = _vixdll.VixVM_CreateDirectoryInGuest(self._vmh, path,
                                                    VIX_INVALID_HANDLE, None, None)
        jobh.Wait()

    def CopyFileFromHostToGuest(self, hostPathName, guestPathName):
        """
        Copy a file from the host to the guest.
        """
        jobh = _vixdll.VixVM_CopyFileFromHostToGuest(self._vmh, hostPathName, guestPathName,
                                                     0, VIX_INVALID_HANDLE, None, None)
        jobh.Wait()

    def EnableSharedFolders(self, enabled):
        """
        Enable shared folders in the guest.
        """
        jobh = _vixdll.VixVM_EnableSharedFolders(self._vmh, enabled, 0, None, None)
        jobh.Wait()

    def AddSharedFolder(self, shareName, hostPathName, flags=0):
        """
        Add a shared folder in the guest.
        """
        jobh = _vixdll.VixVM_AddSharedFolder(self._vmh, shareName, hostPathName, flags,
                                             None, None)
        jobh.Wait()

    def close(self):
        self._vmh.Release()
        self._vmh = _VixHandle(VIX_INVALID_HANDLE)

class VixException(Exception):
    """
    Represents an error that occurred in the VIX library.
    """
    def __init__(self, errcode, *args, **kwargs):
        Exception.__init__(self, *args, **kwargs)
        self.errcode = errcode

    def __str__(self):
        return "%s (%s)" % (_vixdll.Vix_GetErrorText(self.errcode, None), self.errcode)
