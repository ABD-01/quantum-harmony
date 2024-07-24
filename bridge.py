from ctypes.wintypes import *
from ctypes import *

###############################################################################
##                         WINDOWS KERNEL APIS 
###############################################################################

# _kernel32 = WinDLL("kernel32", use_last_error=True)
_kernel32 = windll.kernel32

# minwinbase.h
class _SECURITY_ATTRIBUTES(Structure):
    _fields_ = [
        ('nLength', DWORD),
        ('lpSecurityDescriptor', LPVOID),
        ('bInheritHandle', BOOL),
    ]
LPSECURITY_ATTRIBUTES = POINTER(_SECURITY_ATTRIBUTES)

# WinBase.h
class DCB(Structure):
    _fields_ = [
        ('DCBlength', DWORD),
        ('BaudRate', DWORD),
        ('fBinary', DWORD, 1),
        ('fParity', DWORD, 1),
        ('fOutxCtsFlow', DWORD, 1),
        ('fOutxDsrFlow', DWORD, 1),
        ('fDtrControl', DWORD, 2),
        ('fDsrSensitivity', DWORD, 1),
        ('fTXContinueOnXoff', DWORD, 1),
        ('fOutX', DWORD, 1),
        ('fInX', DWORD, 1),
        ('fErrorChar', DWORD, 1),
        ('fNull', DWORD, 1),
        ('fRtsControl', DWORD, 2),
        ('fAbortOnError', DWORD, 1),
        ('fDummy2', DWORD, 17),
        ('wReserved', WORD),
        ('XonLim', WORD),
        ('XoffLim', WORD),
        ('ByteSize', BYTE),
        ('Parity', BYTE),
        ('StopBits', BYTE),
        ('XonChar', CHAR),
        ('XoffChar', CHAR),
        ('ErrorChar', CHAR),
        ('EofChar', CHAR),
        ('EvtChar', CHAR),
        ('wReserved1', WORD),
    ]
LPDCB = POINTER(DCB)

class COMMTIMEOUTS(Structure):
    _fields_ = [
        ('ReadIntervalTimeout', DWORD),
        ('ReadTotalTimeoutMultiplier', DWORD),
        ('ReadTotalTimeoutConstant', DWORD),
        ('WriteTotalTimeoutMultiplier', DWORD),
        ('WriteTotalTimeoutConstant', DWORD),
    ]
LPCOMMTIMEOUTS = POINTER(COMMTIMEOUTS)

INVALID_HANDLE_VALUE            = HANDLE(-1).value

FORMAT_MESSAGE_FROM_SYSTEM      = 0x00001000
FORMAT_MESSAGE_IGNORE_INSERTS   = 0x00000200

GENERIC_READ                    = 0x80000000
GENERIC_WRITE                   = 0x40000000
GENERIC_EXECUTE                 = 0x20000000
GENERIC_ALL                     = 0x10000000

CREATE_NEW                      = 1
CREATE_ALWAYS                   = 2
OPEN_EXISTING                   = 3
OPEN_ALWAYS                     = 4
TRUNCATE_EXISTING               = 5

CBR_115200                      = 115200

NOPARITY                        = 0
ODDPARITY                       = 1
EVENPARITY                      = 2
MARKPARITY                      = 3
SPACEPARITY                     = 4

ONESTOPBIT                      = 0
ONE5STOPBITS                    = 1
TWOSTOPBITS                     = 2

GetLastError                    = _kernel32.GetLastError
GetLastError.restype            = DWORD
GetLastError.argtypes           = []

FormatMessageA                  = _kernel32.FormatMessageA
FormatMessageA.restype          = DWORD

FormatMessageW                  = _kernel32.FormatMessageW
FormatMessageW.restype          = DWORD

try:
    FormatMessage               = FormatMessageW
except:
    FormatMessage               = FormatMessageA

CreateFileA                     = _kernel32.CreateFileA
CreateFileA.argtypes            = [LPCSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE]
CreateFileA.restype             = HANDLE

CreateFileW                     = _kernel32.CreateFileW
CreateFileW.argtypes            = [LPCWSTR, DWORD, DWORD, LPSECURITY_ATTRIBUTES, DWORD, DWORD, HANDLE]
CreateFileW.restype             = HANDLE

try:
    CreateFile                  = CreateFileW
except:
    CreateFile                  = CreateFileA
    
GetCommState                    = _kernel32.GetCommState
GetCommState.restype            = BOOL
GetCommState.argtypes           = [HANDLE, LPDCB]

SetCommState                    = _kernel32.SetCommState
SetCommState.restype            = BOOL
SetCommState.argtypes           = [HANDLE, LPDCB]

SetCommTimeouts                 = _kernel32.SetCommTimeouts
SetCommTimeouts.restype         = BOOL
SetCommTimeouts.argtypes        = [HANDLE, LPCOMMTIMEOUTS]

CloseHandle                     = _kernel32.CloseHandle
CloseHandle.restype             = BOOL
CloseHandle.argtypes            = [HANDLE]


ReadFile                        = _kernel32.ReadFile
ReadFile.restype               = BOOL
ReadFile.argtypes              = [HANDLE, LPVOID, DWORD, LPDWORD, LPVOID]

WriteFile                       = _kernel32.WriteFile
WriteFile.restype              = BOOL
WriteFile.argtypes             = [HANDLE, LPCVOID, DWORD, LPDWORD, LPVOID]


###############################################################################
##                         SERIAL APIS 
###############################################################################

def CreateFileSerialPort(portName: str):
    try:
        portName = "\\\\.\\%s" % portName
        handle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE, 0, None, OPEN_EXISTING, 0, None)
        if handle == INVALID_HANDLE_VALUE:
            return INVALID_HANDLE_VALUE
        dcbSerialParams = DCB()
        dcbSerialParams.DCBlength = sizeof(DCB)
        if not GetCommState(handle, byref(dcbSerialParams)):
            CloseHandle(handle)
            return INVALID_HANDLE_VALUE
        dcbSerialParams.BaudRate = CBR_115200
        dcbSerialParams.ByteSize = 8
        dcbSerialParams.StopBits = ONESTOPBIT
        dcbSerialParams.Parity = NOPARITY
        if not SetCommState(handle, byref(dcbSerialParams)):
            CloseHandle(handle)
            return INVALID_HANDLE_VALUE
        timeout = COMMTIMEOUTS()
        timeout.ReadIntervalTimeout = 50
        timeout.ReadTotalTimeoutConstant = 50
        timeout.ReadTotalTimeoutMultiplier = 10
        timeout.WriteTotalTimeoutConstant = 50
        timeout.WriteTotalTimeoutMultiplier = 10
        if not SetCommTimeouts(handle, byref(timeout)):
            CloseHandle(handle)
            return INVALID_HANDLE_VALUE
        return handle
    except:
        return INVALID_HANDLE_VALUE

###############################################################################
##                         PROGRESS APIS 
###############################################################################
import socket
import queue
import threading

PORT_NO = 42337
class Progess(threading.Thread):
    def __init__(self, portNo: int = PORT_NO, timeout: int = 10):
        super(Progess, self).__init__()
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        if timeout > 0:
            self.sock.settimeout(timeout)
        self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.sock.bind(("127.0.0.1", portNo))
        self.progress_queue = queue.Queue(maxsize=10)
        self.last_progress = 0
        self.stop_event = threading.Event()
        self.daemon = True
        

    def run(self) -> None:
        while not self.stop_event.is_set():
            try:
                data, addr = self.sock.recvfrom(16)
                if data:
                    data = int(data[0] << 8 | data[1])
                    if self.progress_queue.full():
                        self.progress_queue.get()
                    self.progress_queue.put(data)
            except socket.timeout:
                pass
            except Exception as e:
                break
    
    def get_progress(self):
        if not self.progress_queue.empty():
            self.last_progress = self.progress_queue.get()
        return self.last_progress

    def stop(self):
        self.stop_event.set()
        self.join(timeout=1)
        self.sock.close()
        