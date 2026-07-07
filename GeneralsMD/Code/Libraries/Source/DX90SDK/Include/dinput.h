/**
 * DirectInput 8 - Stub header for compilation only
 */
#pragma once
#ifndef _DINPUT_H_
#define _DINPUT_H_

#include <windows.h>
#include <objbase.h>

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/*  Result codes                                                       */
/* ------------------------------------------------------------------ */

#ifndef DI_OK
#define DI_OK                       S_OK
#define DI_BUFFEROVERFLOW           S_FALSE
#define DI_POLLEDDEVICE             ((HRESULT)0x00000002L)
#define DI_DOWNLOADSKIPPED          ((HRESULT)0x00000003L)
#define DI_EFFECTRESTARTED          ((HRESULT)0x00000004L)
#define DI_TRUNCATED                ((HRESULT)0x00000008L)
#define DI_TRUNCATEDANDRESTARTED    ((HRESULT)0x0000000CL)

#define DIERR_INPUTLOST             MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x01FE)
#define DIERR_NOTACQUIRED           MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x01FC)
#define DIERR_OTHERAPPHASPRIO       E_ACCESSDENIED
#define DIERR_NOTINITIALIZED        MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0154)
#define DIERR_DEVICENOTREG          REGDB_E_CLASSNOTREG
#define DIERR_INVALIDPARAM          E_INVALIDARG
#define DIERR_NOINTERFACE           E_NOINTERFACE
#define DIERR_GENERIC               E_FAIL
#define DIERR_OUTOFMEMORY           E_OUTOFMEMORY
#define DIERR_UNSUPPORTED           E_NOTIMPL
#define DIERR_NOTFOUND              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0002)
#define DIERR_OBJECTNOTFOUND        MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0002)
#define DIERR_ACQUIRED              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x01FF)
#define DIERR_READONLY              E_ACCESSDENIED
#define DIERR_HANDLEEXISTS          E_ACCESSDENIED

// Additional DIERR codes not mapped to standard HRESULTs
#define DIERR_ALREADYINITIALIZED    MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0057)
#define DIERR_BADDRIVERVER          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x00C1)
#define DIERR_BETADIRECTINPUTVERSION MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0154)
#define DIERR_DEVICEFULL            MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0027)
#define DIERR_EFFECTPLAYING         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0200)
#define DIERR_HASEFFECTS            MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0201)
#define DIERR_INCOMPLETEEFFECT      MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0202)
#define DIERR_MAPFILEFAIL           MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0203)
#define DIERR_MOREDATA              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x00EA)
#define DIERR_NOAGGREGATION         CLASS_E_NOAGGREGATION
#define DIERR_NOTBUFFERED           MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0204)
#define DIERR_NOTDOWNLOADED         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0205)
#define DIERR_NOTEXCLUSIVEACQUIRED  MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x01FD)
#define DIERR_OLDDIRECTINPUTVERSION MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0153)
#define DIERR_REPORTFULL            MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0206)
#define DIERR_UNPLUGGED             MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, 0x0207)

// Device capabilities flags
#define DIDC_ATTACHED       0x00000001
#define DIDC_POLLEDDEVICE   0x00000002
#define DIDC_EMULATED       0x00000004
#define DIDC_POLLEDDATAFORMAT 0x00000008
#define DIDC_FORCEFEEDBACK  0x00000100
#define DIDC_FFATTACK       0x00000200
#define DIDC_FFFADE         0x00000400
#define DIDC_SATURATION     0x00000800
#define DIDC_POSNEGCOEFFICIENTS 0x00001000
#define DIDC_POSNEGSATURATION   0x00002000
#define DIDC_DEADBAND       0x00004000
#define DIDC_STARTDELAY     0x00008000
#define DIDC_ALIAS          0x00010000
#define DIDC_PHANTOM        0x00020000
#define DIDC_HIDDEN         0x00040000
#endif

/* ------------------------------------------------------------------ */
/*  Structures                                                         */
/* ------------------------------------------------------------------ */

#pragma pack(push, 4)

typedef struct DIDEVICEINSTANCEA {
    DWORD dwSize;
    GUID  guidInstance;
    GUID  guidProduct;
    DWORD dwDevType;
    CHAR  tszInstanceName[260];
    CHAR  tszProductName[260];
    GUID  guidFFDriver;
    WORD  wUsagePage;
    WORD  wUsage;
} DIDEVICEINSTANCEA, *LPDIDEVICEINSTANCEA;
typedef const DIDEVICEINSTANCEA* LPCDIDEVICEINSTANCEA;

typedef struct DIDEVICEINSTANCEW {
    DWORD dwSize;
    GUID  guidInstance;
    GUID  guidProduct;
    DWORD dwDevType;
    WCHAR tszInstanceName[260];
    WCHAR tszProductName[260];
    GUID  guidFFDriver;
    WORD  wUsagePage;
    WORD  wUsage;
} DIDEVICEINSTANCEW, *LPDIDEVICEINSTANCEW;
typedef const DIDEVICEINSTANCEW* LPCDIDEVICEINSTANCEW;

#ifdef UNICODE
typedef DIDEVICEINSTANCEW DIDEVICEINSTANCE;
typedef LPDIDEVICEINSTANCEW LPDIDEVICEINSTANCE;
typedef LPCDIDEVICEINSTANCEW LPCDIDEVICEINSTANCE;
#else
typedef DIDEVICEINSTANCEA DIDEVICEINSTANCE;
typedef LPDIDEVICEINSTANCEA LPDIDEVICEINSTANCE;
typedef LPCDIDEVICEINSTANCEA LPCDIDEVICEINSTANCE;
#endif

typedef struct DIDEVICEOBJECTINSTANCEA {
    DWORD dwSize;
    GUID  guidType;
    DWORD dwOfs;
    DWORD dwType;
    DWORD dwFlags;
    CHAR  tszName[260];
    DWORD dwFFMaxForce;
    DWORD dwFFForceResolution;
    WORD  wCollectionNumber;
    WORD  wDesignatorIndex;
    WORD  wUsagePage;
    WORD  wUsage;
    DWORD dwDimension;
    WORD  wExponent;
    WORD  wReportId;
} DIDEVICEOBJECTINSTANCEA, *LPDIDEVICEOBJECTINSTANCEA;
typedef const DIDEVICEOBJECTINSTANCEA* LPCDIDEVICEOBJECTINSTANCEA;

typedef struct DIDEVICEOBJECTINSTANCEW {
    DWORD dwSize;
    GUID  guidType;
    DWORD dwOfs;
    DWORD dwType;
    DWORD dwFlags;
    WCHAR tszName[260];
    DWORD dwFFMaxForce;
    DWORD dwFFForceResolution;
    WORD  wCollectionNumber;
    WORD  wDesignatorIndex;
    WORD  wUsagePage;
    WORD  wUsage;
    DWORD dwDimension;
    WORD  wExponent;
    WORD  wReportId;
} DIDEVICEOBJECTINSTANCEW, *LPDIDEVICEOBJECTINSTANCEW;
typedef const DIDEVICEOBJECTINSTANCEW* LPCDIDEVICEOBJECTINSTANCEW;

#ifdef UNICODE
typedef DIDEVICEOBJECTINSTANCEW DIDEVICEOBJECTINSTANCE;
typedef LPDIDEVICEOBJECTINSTANCEW LPDIDEVICEOBJECTINSTANCE;
typedef LPCDIDEVICEOBJECTINSTANCEW LPCDIDEVICEOBJECTINSTANCE;
#else
typedef DIDEVICEOBJECTINSTANCEA DIDEVICEOBJECTINSTANCE;
typedef LPDIDEVICEOBJECTINSTANCEA LPDIDEVICEOBJECTINSTANCE;
typedef LPCDIDEVICEOBJECTINSTANCEA LPCDIDEVICEOBJECTINSTANCE;
#endif

typedef struct DIOBJECTDATAFORMAT {
    const GUID* pguid;
    DWORD       dwOfs;
    DWORD       dwType;
    DWORD       dwFlags;
} DIOBJECTDATAFORMAT, *LPDIOBJECTDATAFORMAT;
typedef const DIOBJECTDATAFORMAT* LPCDIOBJECTDATAFORMAT;

typedef struct DIDATAFORMAT {
    DWORD               dwSize;
    DWORD               dwObjSize;
    DWORD               dwFlags;
    DWORD               dwDataSize;
    DWORD               dwNumObjs;
    LPDIOBJECTDATAFORMAT rgodf;
} DIDATAFORMAT, *LPDIDATAFORMAT;
typedef const DIDATAFORMAT* LPCDIDATAFORMAT;

typedef struct DIDEVCAPS {
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwDevType;
    DWORD dwAxes;
    DWORD dwButtons;
    DWORD dwPOVs;
    DWORD dwFFSamplePeriod;
    DWORD dwFFMinTimeResolution;
    DWORD dwFirmwareRevision;
    DWORD dwHardwareRevision;
    DWORD dwFFDriverVersion;
} DIDEVCAPS, *LPDIDEVCAPS;

typedef struct DIPROPHEADER {
    DWORD dwSize;
    DWORD dwHeaderSize;
    DWORD dwObj;
    DWORD dwHow;
} DIPROPHEADER, *LPDIPROPHEADER;
typedef const DIPROPHEADER* LPCDIPROPHEADER;

typedef struct DIPROPDWORD {
    DIPROPHEADER diph;
    DWORD        dwData;
} DIPROPDWORD, *LPDIPROPDWORD;
typedef const DIPROPDWORD* LPCDIPROPDWORD;

typedef struct DIPROPRANGE {
    DIPROPHEADER diph;
    LONG         lMin;
    LONG         lMax;
} DIPROPRANGE, *LPDIPROPRANGE;
typedef const DIPROPRANGE* LPCDIPROPRANGE;

typedef struct DIMOUSESTATE {
    LONG lX;
    LONG lY;
    LONG lZ;
    BYTE rgbButtons[4];
} DIMOUSESTATE, *LPDIMOUSESTATE;

typedef struct DIMOUSESTATE2 {
    LONG lX;
    LONG lY;
    LONG lZ;
    BYTE rgbButtons[8];
} DIMOUSESTATE2, *LPDIMOUSESTATE2;

typedef struct DIJOYSTATE {
    LONG    lX;
    LONG    lY;
    LONG    lZ;
    LONG    lRx;
    LONG    lRy;
    LONG    lRz;
    LONG    rglSlider[2];
    DWORD   rgdwPOV[4];
    BYTE    rgbButtons[32];
} DIJOYSTATE, *LPDIJOYSTATE;

typedef struct DIJOYSTATE2 {
    LONG    lX;
    LONG    lY;
    LONG    lZ;
    LONG    lRx;
    LONG    lRy;
    LONG    lRz;
    LONG    rglSlider[2];
    DWORD   rgdwPOV[4];
    BYTE    rgbButtons[128];
    LONG    lVX;
    LONG    lVY;
    LONG    lVZ;
    LONG    lVRx;
    LONG    lVRy;
    LONG    lVRz;
    LONG    rglVSlider[2];
    LONG    lAX;
    LONG    lAY;
    LONG    lAZ;
    LONG    lARx;
    LONG    lARy;
    LONG    lARz;
    LONG    rglASlider[2];
    LONG    lFX;
    LONG    lFY;
    LONG    lFZ;
    LONG    lFRx;
    LONG    lFRy;
    LONG    lFRz;
    LONG    rglFSlider[2];
} DIJOYSTATE2, *LPDIJOYSTATE2;

typedef struct DIDEVICEOBJECTDATA {
    DWORD dwOfs;
    DWORD dwData;
    DWORD dwTimeStamp;
    DWORD dwSequence;
} DIDEVICEOBJECTDATA, *LPDIDEVICEOBJECTDATA;
typedef const DIDEVICEOBJECTDATA* LPCDIDEVICEOBJECTDATA;

typedef struct DIACTIONFORMATA {
    DWORD dwSize;
    DWORD dwActionSize;
    DWORD dwDataSize;
    DWORD dwNumActions;
    /* ... simplified ... */
} DIACTIONFORMATA, *LPDIACTIONFORMATA;
typedef const DIACTIONFORMATA* LPCDIACTIONFORMATA;

#pragma pack(pop)

/* ------------------------------------------------------------------ */
/*  Callback typedefs                                                  */
/* ------------------------------------------------------------------ */

typedef BOOL (CALLBACK *LPDIENUMDEVICESCALLBACKA)(LPCDIDEVICEINSTANCEA, LPVOID);
typedef BOOL (CALLBACK *LPDIENUMDEVICESCALLBACKW)(LPCDIDEVICEINSTANCEW, LPVOID);
typedef BOOL (CALLBACK *LPDIENUMDEVICEOBJECTSCALLBACKA)(LPCDIDEVICEOBJECTINSTANCEA, LPVOID);
typedef BOOL (CALLBACK *LPDIENUMDEVICEOBJECTSCALLBACKW)(LPCDIDEVICEOBJECTINSTANCEW, LPVOID);

#ifdef UNICODE
typedef LPDIENUMDEVICESCALLBACKW LPDIENUMDEVICESCALLBACK;
typedef LPDIENUMDEVICEOBJECTSCALLBACKW LPDIENUMDEVICEOBJECTSCALLBACK;
#else
typedef LPDIENUMDEVICESCALLBACKA LPDIENUMDEVICESCALLBACK;
typedef LPDIENUMDEVICEOBJECTSCALLBACKA LPDIENUMDEVICEOBJECTSCALLBACK;
#endif

/* ------------------------------------------------------------------ */
/*  Property defines                                                   */
/* ------------------------------------------------------------------ */

#define DIPH_DEVICE   0
#define DIPH_BYOFFSET 1
#define DIPH_BYID     2
#define DIPH_BYUSAGE  3

#define DIPROP_BUFFERSIZE   MAKEDIPROP(1)
#define DIPROP_AXISMODE     MAKEDIPROP(2)
#define DIPROP_GRANULARITY  MAKEDIPROP(3)
#define DIPROP_RANGE        MAKEDIPROP(4)
#define DIPROP_DEADZONE     MAKEDIPROP(5)
#define DIPROP_SATURATION   MAKEDIPROP(6)
#define DIPROP_FFGAIN       MAKEDIPROP(7)
#define DIPROP_FFLOAD       MAKEDIPROP(8)
#define DIPROP_AUTOCENTER   MAKEDIPROP(9)
#define DIPROP_CALIBRATIONMODE MAKEDIPROP(10)

#ifdef _MSC_VER
#define MAKEDIPROP(prop) ((const GUID*)(UINT_PTR)(prop))
#else
#define MAKEDIPROP(prop) ((const GUID*)(prop))
#endif

#ifndef DIPROP_BUFFERSIZE
#define DIPROP_BUFFERSIZE   MAKEDIPROP(1)
#endif
#ifndef DIPROP_AXISMODE
#define DIPROP_AXISMODE     MAKEDIPROP(2)
#endif
#ifndef DIPROP_RANGE
#define DIPROP_RANGE        MAKEDIPROP(4)
#endif
#ifndef DIPROP_DEADZONE
#define DIPROP_DEADZONE     MAKEDIPROP(5)
#endif

#define DIPROPAXISMODE_ABS  0
#define DIPROPAXISMODE_REL  1

/* ------------------------------------------------------------------ */
/*  Device type / subtype macros                                       */
/* ------------------------------------------------------------------ */

#define DIDEVTYPE_HID           0x00010000

#define DI8DEVCLASS_ALL         0
#define DI8DEVCLASS_DEVICE      1
#define DI8DEVCLASS_POINTER     2
#define DI8DEVCLASS_KEYBOARD    3
#define DI8DEVCLASS_GAMECTRL    4

#define DI8DEVTYPE_DEVICE       0x11
#define DI8DEVTYPE_MOUSE        0x12
#define DI8DEVTYPE_KEYBOARD     0x13
#define DI8DEVTYPE_JOYSTICK     0x14
#define DI8DEVTYPE_GAMEPAD      0x15
#define DI8DEVTYPE_DRIVING      0x16
#define DI8DEVTYPE_FLIGHT       0x17
#define DI8DEVTYPE_1STPERSON    0x18
#define DI8DEVTYPE_DEVICECTRL   0x19
#define DI8DEVTYPE_SCREENPOINTER 0x1A
#define DI8DEVTYPE_REMOTE       0x1B
#define DI8DEVTYPE_SUPPLEMENTAL 0x1C

#define GET_DIDEVICE_TYPE(dwDevType)    LOBYTE(dwDevType)
#define GET_DIDEVICE_SUBTYPE(dwDevType) HIBYTE(dwDevType)

/* ------------------------------------------------------------------ */
/*  Enum flags                                                         */
/* ------------------------------------------------------------------ */

#define DIEDFL_ALLDEVICES       0x00000000
#define DIEDFL_ATTACHEDONLY     0x00000001
#define DIEDFL_FORCEFEEDBACK    0x00000100
#define DIEDFL_INCLUDEALIASES   0x00010000
#define DIEDFL_INCLUDEPHANTOMS  0x00020000
#define DIEDFL_INCLUDEHIDDEN    0x00040000

#define DIENUM_STOP             0
#define DIENUM_CONTINUE         1

/* ------------------------------------------------------------------ */
/*  Cooperative level flags                                            */
/* ------------------------------------------------------------------ */

#define DISCL_EXCLUSIVE    0x00000001
#define DISCL_NONEXCLUSIVE 0x00000002
#define DISCL_FOREGROUND   0x00000004
#define DISCL_BACKGROUND   0x00000008
#define DISCL_NOWINKEY     0x00000010

/* ------------------------------------------------------------------ */
/*  Object type flags (DIDFT)                                          */
/* ------------------------------------------------------------------ */

#define DIDFT_ALL           0x00000000
#define DIDFT_RELAXIS       0x00000001
#define DIDFT_ABSAXIS       0x00000002
#define DIDFT_AXIS          0x00000003
#define DIDFT_PSHBUTTON     0x00000004
#define DIDFT_TGLBUTTON     0x00000008
#define DIDFT_BUTTON        0x0000000C
#define DIDFT_POV           0x00000010
#define DIDFT_COLLECTION    0x00000040
#define DIDFT_NODATA        0x00000080
#define DIDFT_ANYINSTANCE   0x00FFFF00
#define DIDFT_INSTANCEMASK  DIDFT_ANYINSTANCE
#define DIDFT_FFACTUATOR    0x01000000
#define DIDFT_FFEFFECTTRIGGER 0x02000000
#define DIDFT_OUTPUT        0x10000000
#define DIDFT_VENDORDEFINED 0x04000000
#define DIDFT_ALIAS         0x08000000
#define DIDFT_OPTIONAL      0x80000000

#define DIDFT_MAKEINSTANCE(n)  ((DWORD)(n) << 8)
#define DIDFT_GETTYPE(n)       LOBYTE(n)
#define DIDFT_GETINSTANCE(n)   LOWORD((n) >> 8)
#define DIDFT_ENUMCOLLECTION(n) ((DWORD)(n) << 8)

#define DIDOI_FFACTUATOR    0x00000001
#define DIDOI_FFEFFECTTRIGGER 0x00000002
#define DIDOI_POLLED        0x00008000
#define DIDOI_ASPECTPOSITION 0x00000100
#define DIDOI_ASPECTVELOCITY 0x00000200
#define DIDOI_ASPECTACCEL   0x00000300
#define DIDOI_ASPECTFORCE   0x00000400
#define DIDOI_ASPECTMASK    0x00000F00
#define DIDOI_GUIDISUSAGE   0x00010000

/* ------------------------------------------------------------------ */
/*  Data format flags                                                  */
/* ------------------------------------------------------------------ */

#define DIDF_ABSAXIS 0x00000001
#define DIDF_RELAXIS 0x00000002

/* ------------------------------------------------------------------ */
/*  Sequence number                                                    */
/* ------------------------------------------------------------------ */

#define INFINITE_UNSPECIFIED 0xFFFFFFFF

/* ------------------------------------------------------------------ */
/*  Extern GUIDs and data formats                                      */
/* ------------------------------------------------------------------ */

/* These are defined as extern globals. In a real build they come from
   dinput8.lib. For compilation stubs we just declare them. */

#ifdef __cplusplus

extern const GUID GUID_SysMouse;
extern const GUID GUID_SysKeyboard;
extern const GUID GUID_SysKeyboardEm;
extern const GUID GUID_SysKeyboardEm2;
extern const GUID GUID_Joystick;

extern const GUID GUID_XAxis;
extern const GUID GUID_YAxis;
extern const GUID GUID_ZAxis;
extern const GUID GUID_RxAxis;
extern const GUID GUID_RyAxis;
extern const GUID GUID_RzAxis;
extern const GUID GUID_Slider;
extern const GUID GUID_Button;
extern const GUID GUID_Key;
extern const GUID GUID_POV;
extern const GUID GUID_Unknown;

extern const DIDATAFORMAT c_dfDIMouse;
extern const DIDATAFORMAT c_dfDIMouse2;
extern const DIDATAFORMAT c_dfDIKeyboard;
extern const DIDATAFORMAT c_dfDIJoystick;
extern const DIDATAFORMAT c_dfDIJoystick2;

#else

extern const GUID GUID_SysMouse;
extern const GUID GUID_SysKeyboard;
extern const GUID GUID_Joystick;
extern const GUID GUID_XAxis;
extern const GUID GUID_YAxis;
extern const GUID GUID_ZAxis;
extern const GUID GUID_RxAxis;
extern const GUID GUID_RyAxis;
extern const GUID GUID_RzAxis;
extern const GUID GUID_Slider;
extern const GUID GUID_Button;
extern const GUID GUID_Key;
extern const GUID GUID_POV;
extern const GUID GUID_Unknown;

extern const DIDATAFORMAT c_dfDIMouse;
extern const DIDATAFORMAT c_dfDIMouse2;
extern const DIDATAFORMAT c_dfDIKeyboard;
extern const DIDATAFORMAT c_dfDIJoystick;
extern const DIDATAFORMAT c_dfDIJoystick2;

#endif

/* ------------------------------------------------------------------ */
/*  Interfaces                                                         */
/* ------------------------------------------------------------------ */

#ifdef __cplusplus

struct IDirectInputDevice8A;
struct IDirectInputDevice8W;
struct IDirectInput8A;
struct IDirectInput8W;
struct IDirectInputEffect;

typedef IDirectInputDevice8A* LPDIRECTINPUTDEVICE8A;
typedef IDirectInputDevice8W* LPDIRECTINPUTDEVICE8W;
typedef IDirectInput8A*       LPDIRECTINPUT8A;
typedef IDirectInput8W*       LPDIRECTINPUT8W;
typedef IDirectInputEffect*   LPDIRECTINPUTEFFECT;

#ifdef UNICODE
typedef IDirectInputDevice8W  IDirectInputDevice8;
typedef IDirectInput8W        IDirectInput8;
typedef LPDIRECTINPUTDEVICE8W LPDIRECTINPUTDEVICE8;
typedef LPDIRECTINPUT8W       LPDIRECTINPUT8;
#else
typedef IDirectInputDevice8A  IDirectInputDevice8;
typedef IDirectInput8A        IDirectInput8;
typedef LPDIRECTINPUTDEVICE8A LPDIRECTINPUTDEVICE8;
typedef LPDIRECTINPUT8A       LPDIRECTINPUT8;
#endif

/* ------------------------------------------------------------------ */
/*  IDirectInputEffect                                                 */
/* ------------------------------------------------------------------ */

struct IDirectInputEffect : public IUnknown {
    virtual HRESULT __stdcall Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid) = 0;
    virtual HRESULT __stdcall GetEffectGuid(LPGUID pguid) = 0;
    virtual HRESULT __stdcall GetParameters(void* peff, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall SetParameters(const void* peff, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall Start(DWORD dwIterations, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall Stop() = 0;
    virtual HRESULT __stdcall GetEffectStatus(LPDWORD pdwFlags) = 0;
    virtual HRESULT __stdcall Download() = 0;
    virtual HRESULT __stdcall Unload() = 0;
    virtual HRESULT __stdcall Escape(void* pesc) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirectInputDevice8A                                               */
/* ------------------------------------------------------------------ */

struct IDirectInputDevice8A : public IUnknown {
    virtual HRESULT __stdcall GetCapabilities(LPDIDEVCAPS lpDIDevCaps) = 0;
    virtual HRESULT __stdcall EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall GetProperty(const GUID* rguidProp, LPDIPROPHEADER pdiph) = 0;
    virtual HRESULT __stdcall SetProperty(const GUID* rguidProp, LPCDIPROPHEADER pdiph) = 0;
    virtual HRESULT __stdcall Acquire() = 0;
    virtual HRESULT __stdcall Unacquire() = 0;
    virtual HRESULT __stdcall GetDeviceState(DWORD cbData, LPVOID lpvData) = 0;
    virtual HRESULT __stdcall GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall SetDataFormat(LPCDIDATAFORMAT lpdf) = 0;
    virtual HRESULT __stdcall SetEventNotification(HANDLE hEvent) = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND hwnd, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall GetObjectInfo(LPDIDEVICEOBJECTINSTANCEA pdidoi, DWORD dwObj, DWORD dwHow) = 0;
    virtual HRESULT __stdcall GetDeviceInfo(LPDIDEVICEINSTANCEA pdidi) = 0;
    virtual HRESULT __stdcall RunControlPanel(HWND hwndOwner, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid) = 0;
    virtual HRESULT __stdcall CreateEffect(REFGUID rguid, const void* lpeff, LPDIRECTINPUTEFFECT* ppdeff, LPUNKNOWN punkOuter) = 0;
    virtual HRESULT __stdcall EnumEffects(void* lpCallback, LPVOID pvRef, DWORD dwEffType) = 0;
    virtual HRESULT __stdcall GetEffectInfo(void* pdei, REFGUID rguid) = 0;
    virtual HRESULT __stdcall GetForceFeedbackState(LPDWORD pdwOut) = 0;
    virtual HRESULT __stdcall SendForceFeedbackCommand(DWORD dwFlags) = 0;
    virtual HRESULT __stdcall EnumCreatedEffectObjects(void* lpCallback, LPVOID pvRef, DWORD fl) = 0;
    virtual HRESULT __stdcall Escape(void* pesc) = 0;
    virtual HRESULT __stdcall Poll() = 0;
    virtual HRESULT __stdcall SendDeviceData(DWORD cbObjectData, const DIDEVICEOBJECTDATA* rgdod, LPDWORD pdwInOut, DWORD fl) = 0;
    virtual HRESULT __stdcall EnumEffectsInFile(LPCSTR lpszFileName, void* pec, LPVOID pvRef, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall WriteEffectToFile(LPCSTR lpszFileName, DWORD dwEntries, void* rgDiFileEft, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall BuildActionMap(LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall SetActionMap(LPDIACTIONFORMATA lpdiaf, LPCSTR lpszUserName, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall GetImageInfo(void* lpdiDevImageInfoHeader) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirectInputDevice8W                                               */
/* ------------------------------------------------------------------ */

struct IDirectInputDevice8W : public IUnknown {
    virtual HRESULT __stdcall GetCapabilities(LPDIDEVCAPS lpDIDevCaps) = 0;
    virtual HRESULT __stdcall EnumObjects(LPDIENUMDEVICEOBJECTSCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall GetProperty(const GUID* rguidProp, LPDIPROPHEADER pdiph) = 0;
    virtual HRESULT __stdcall SetProperty(const GUID* rguidProp, LPCDIPROPHEADER pdiph) = 0;
    virtual HRESULT __stdcall Acquire() = 0;
    virtual HRESULT __stdcall Unacquire() = 0;
    virtual HRESULT __stdcall GetDeviceState(DWORD cbData, LPVOID lpvData) = 0;
    virtual HRESULT __stdcall GetDeviceData(DWORD cbObjectData, LPDIDEVICEOBJECTDATA rgdod, LPDWORD pdwInOut, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall SetDataFormat(LPCDIDATAFORMAT lpdf) = 0;
    virtual HRESULT __stdcall SetEventNotification(HANDLE hEvent) = 0;
    virtual HRESULT __stdcall SetCooperativeLevel(HWND hwnd, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall GetObjectInfo(LPDIDEVICEOBJECTINSTANCEW pdidoi, DWORD dwObj, DWORD dwHow) = 0;
    virtual HRESULT __stdcall GetDeviceInfo(LPDIDEVICEINSTANCEW pdidi) = 0;
    virtual HRESULT __stdcall RunControlPanel(HWND hwndOwner, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall Initialize(HINSTANCE hinst, DWORD dwVersion, REFGUID rguid) = 0;
    virtual HRESULT __stdcall CreateEffect(REFGUID rguid, const void* lpeff, LPDIRECTINPUTEFFECT* ppdeff, LPUNKNOWN punkOuter) = 0;
    virtual HRESULT __stdcall EnumEffects(void* lpCallback, LPVOID pvRef, DWORD dwEffType) = 0;
    virtual HRESULT __stdcall GetEffectInfo(void* pdei, REFGUID rguid) = 0;
    virtual HRESULT __stdcall GetForceFeedbackState(LPDWORD pdwOut) = 0;
    virtual HRESULT __stdcall SendForceFeedbackCommand(DWORD dwFlags) = 0;
    virtual HRESULT __stdcall EnumCreatedEffectObjects(void* lpCallback, LPVOID pvRef, DWORD fl) = 0;
    virtual HRESULT __stdcall Escape(void* pesc) = 0;
    virtual HRESULT __stdcall Poll() = 0;
    virtual HRESULT __stdcall SendDeviceData(DWORD cbObjectData, const DIDEVICEOBJECTDATA* rgdod, LPDWORD pdwInOut, DWORD fl) = 0;
    virtual HRESULT __stdcall EnumEffectsInFile(LPCWSTR lpszFileName, void* pec, LPVOID pvRef, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall WriteEffectToFile(LPCWSTR lpszFileName, DWORD dwEntries, void* rgDiFileEft, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall BuildActionMap(void* lpdiaf, LPCWSTR lpszUserName, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall SetActionMap(void* lpdiaf, LPCWSTR lpszUserName, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall GetImageInfo(void* lpdiDevImageInfoHeader) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirectInput8A                                                     */
/* ------------------------------------------------------------------ */

struct IDirectInput8A : public IUnknown {
    virtual HRESULT __stdcall CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8A* lplpDirectInputDevice, LPUNKNOWN pUnkOuter) = 0;
    virtual HRESULT __stdcall EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKA lpCallback, LPVOID pvRef, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall GetDeviceStatus(REFGUID rguidInstance) = 0;
    virtual HRESULT __stdcall RunControlPanel(HWND hwndOwner, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall Initialize(HINSTANCE hinst, DWORD dwVersion) = 0;
    virtual HRESULT __stdcall FindDevice(REFGUID rguidClass, LPCSTR ptszName, LPGUID pguidInstance) = 0;
    virtual HRESULT __stdcall EnumDevicesBySemantics(LPCSTR ptszUserName, void* lpdiActionFormat, void* lpCallback, LPVOID pvRef, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall ConfigureDevices(void* lpdiCallback, void* lpdiCDParams, DWORD dwFlags, LPVOID pvRefData) = 0;
};

/* ------------------------------------------------------------------ */
/*  IDirectInput8W                                                     */
/* ------------------------------------------------------------------ */

struct IDirectInput8W : public IUnknown {
    virtual HRESULT __stdcall CreateDevice(REFGUID rguid, LPDIRECTINPUTDEVICE8W* lplpDirectInputDevice, LPUNKNOWN pUnkOuter) = 0;
    virtual HRESULT __stdcall EnumDevices(DWORD dwDevType, LPDIENUMDEVICESCALLBACKW lpCallback, LPVOID pvRef, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall GetDeviceStatus(REFGUID rguidInstance) = 0;
    virtual HRESULT __stdcall RunControlPanel(HWND hwndOwner, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall Initialize(HINSTANCE hinst, DWORD dwVersion) = 0;
    virtual HRESULT __stdcall FindDevice(REFGUID rguidClass, LPCWSTR ptszName, LPGUID pguidInstance) = 0;
    virtual HRESULT __stdcall EnumDevicesBySemantics(LPCWSTR ptszUserName, void* lpdiActionFormat, void* lpCallback, LPVOID pvRef, DWORD dwFlags) = 0;
    virtual HRESULT __stdcall ConfigureDevices(void* lpdiCallback, void* lpdiCDParams, DWORD dwFlags, LPVOID pvRefData) = 0;
};

/* ------------------------------------------------------------------ */
/*  DirectInput8Create function stub                                   */
/* ------------------------------------------------------------------ */

inline HRESULT WINAPI DirectInput8Create(
    HINSTANCE hinst, DWORD dwVersion, REFIID riidltf,
    LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    (void)hinst; (void)dwVersion; (void)riidltf;
    (void)ppvOut; (void)punkOuter;
    return E_NOTIMPL;
}

#else /* !__cplusplus */

typedef struct IDirectInput8A   IDirectInput8A;
typedef struct IDirectInput8W   IDirectInput8W;
typedef struct IDirectInputDevice8A  IDirectInputDevice8A;
typedef struct IDirectInputDevice8W  IDirectInputDevice8W;

typedef IDirectInput8A*          LPDIRECTINPUT8A;
typedef IDirectInput8W*          LPDIRECTINPUT8W;
typedef IDirectInputDevice8A*    LPDIRECTINPUTDEVICE8A;
typedef IDirectInputDevice8W*    LPDIRECTINPUTDEVICE8W;

#ifdef UNICODE
typedef IDirectInput8W           IDirectInput8;
typedef IDirectInputDevice8W     IDirectInputDevice8;
typedef LPDIRECTINPUT8W          LPDIRECTINPUT8;
typedef LPDIRECTINPUTDEVICE8W    LPDIRECTINPUTDEVICE8;
#else
typedef IDirectInput8A           IDirectInput8;
typedef IDirectInputDevice8A     IDirectInputDevice8;
typedef LPDIRECTINPUT8A          LPDIRECTINPUT8;
typedef LPDIRECTINPUTDEVICE8A    LPDIRECTINPUTDEVICE8;
#endif

#endif /* __cplusplus */

/* ------------------------------------------------------------------ */
/*  IID defines                                                        */
/* ------------------------------------------------------------------ */

/* These are normally in dinput8.lib; declare them extern for compilation */
extern const GUID IID_IDirectInput8A;
extern const GUID IID_IDirectInput8W;

#ifdef UNICODE
#define IID_IDirectInput8 IID_IDirectInput8W
#else
#define IID_IDirectInput8 IID_IDirectInput8A
#endif

/* ------------------------------------------------------------------ */
/*  DIK_ keyboard scan codes                                           */
/* ------------------------------------------------------------------ */

#define DIK_ESCAPE          0x01
#define DIK_1               0x02
#define DIK_2               0x03
#define DIK_3               0x04
#define DIK_4               0x05
#define DIK_5               0x06
#define DIK_6               0x07
#define DIK_7               0x08
#define DIK_8               0x09
#define DIK_9               0x0A
#define DIK_0               0x0B
#define DIK_MINUS           0x0C
#define DIK_EQUALS          0x0D
#define DIK_BACK            0x0E
#define DIK_BACKSPACE       DIK_BACK
#define DIK_TAB             0x0F
#define DIK_Q               0x10
#define DIK_W               0x11
#define DIK_E               0x12
#define DIK_R               0x13
#define DIK_T               0x14
#define DIK_Y               0x15
#define DIK_U               0x16
#define DIK_I               0x17
#define DIK_O               0x18
#define DIK_P               0x19
#define DIK_LBRACKET        0x1A
#define DIK_RBRACKET        0x1B
#define DIK_RETURN          0x1C
#define DIK_LCONTROL        0x1D
#define DIK_A               0x1E
#define DIK_S               0x1F
#define DIK_D               0x20
#define DIK_F               0x21
#define DIK_G               0x22
#define DIK_H               0x23
#define DIK_J               0x24
#define DIK_K               0x25
#define DIK_L               0x26
#define DIK_SEMICOLON       0x27
#define DIK_APOSTROPHE      0x28
#define DIK_GRAVE           0x29
#define DIK_LSHIFT          0x2A
#define DIK_BACKSLASH       0x2B
#define DIK_Z               0x2C
#define DIK_X               0x2D
#define DIK_C               0x2E
#define DIK_V               0x2F
#define DIK_B               0x30
#define DIK_N               0x31
#define DIK_M               0x32
#define DIK_COMMA           0x33
#define DIK_PERIOD          0x34
#define DIK_SLASH           0x35
#define DIK_RSHIFT          0x36
#define DIK_MULTIPLY        0x37
#define DIK_NUMPADSTAR      DIK_MULTIPLY
#define DIK_LMENU           0x38
#define DIK_LALT            DIK_LMENU
#define DIK_SPACE           0x39
#define DIK_CAPITAL         0x3A
#define DIK_CAPSLOCK        DIK_CAPITAL
#define DIK_F1              0x3B
#define DIK_F2              0x3C
#define DIK_F3              0x3D
#define DIK_F4              0x3E
#define DIK_F5              0x3F
#define DIK_F6              0x40
#define DIK_F7              0x41
#define DIK_F8              0x42
#define DIK_F9              0x43
#define DIK_F10             0x44
#define DIK_NUMLOCK         0x45
#define DIK_SCROLL          0x46
#define DIK_NUMPAD7         0x47
#define DIK_NUMPAD8         0x48
#define DIK_NUMPAD9         0x49
#define DIK_SUBTRACT        0x4A
#define DIK_NUMPADMINUS     DIK_SUBTRACT
#define DIK_NUMPAD4         0x4B
#define DIK_NUMPAD5         0x4C
#define DIK_NUMPAD6         0x4D
#define DIK_ADD             0x4E
#define DIK_NUMPADPLUS      DIK_ADD
#define DIK_NUMPAD1         0x4F
#define DIK_NUMPAD2         0x50
#define DIK_NUMPAD3         0x51
#define DIK_NUMPAD0         0x52
#define DIK_DECIMAL         0x53
#define DIK_NUMPADPERIOD    DIK_DECIMAL
#define DIK_OEM_102         0x56
#define DIK_F11             0x57
#define DIK_F12             0x58
#define DIK_F13             0x64
#define DIK_F14             0x65
#define DIK_F15             0x66
#define DIK_KANA            0x70
#define DIK_ABNT_C1         0x73
#define DIK_CONVERT         0x79
#define DIK_NOCONVERT       0x7B
#define DIK_YEN             0x7D
#define DIK_ABNT_C2         0x7E
#define DIK_NUMPADEQUALS    0x8D
#define DIK_PREVTRACK       0x90
#define DIK_CIRCUMFLEX      DIK_PREVTRACK
#define DIK_AT              0x91
#define DIK_COLON           0x92
#define DIK_UNDERLINE       0x93
#define DIK_KANJI           0x94
#define DIK_STOP            0x95
#define DIK_AX              0x96
#define DIK_UNLABELED       0x97
#define DIK_NEXTTRACK       0x99
#define DIK_NUMPADENTER     0x9C
#define DIK_RCONTROL        0x9D
#define DIK_MUTE            0xA0
#define DIK_CALCULATOR      0xA1
#define DIK_PLAYPAUSE       0xA2
#define DIK_MEDIASTOP       0xA4
#define DIK_VOLUMEDOWN      0xAE
#define DIK_VOLUMEUP        0xB0
#define DIK_WEBHOME         0xB2
#define DIK_NUMPADCOMMA     0xB3
#define DIK_DIVIDE          0xB5
#define DIK_NUMPADSLASH     DIK_DIVIDE
#define DIK_SYSRQ           0xB7
#define DIK_RMENU           0xB8
#define DIK_RALT            DIK_RMENU
#define DIK_PAUSE           0xC5
#define DIK_HOME            0xC7
#define DIK_UP              0xC8
#define DIK_PRIOR           0xC9
#define DIK_PGUP            DIK_PRIOR
#define DIK_LEFT            0xCB
#define DIK_RIGHT           0xCD
#define DIK_END             0xCF
#define DIK_DOWN            0xD0
#define DIK_NEXT            0xD1
#define DIK_PGDN            DIK_NEXT
#define DIK_INSERT          0xD2
#define DIK_DELETE          0xD3

// Aliases used by Generals code (DIK_UPARROW etc.)
#define DIK_UPARROW         DIK_UP
#define DIK_DOWNARROW       DIK_DOWN
#define DIK_LEFTARROW       DIK_LEFT
#define DIK_RIGHTARROW      DIK_RIGHT

#define DIK_LWIN            0xDB
#define DIK_RWIN            0xDC
#define DIK_APPS            0xDD
#define DIK_POWER           0xDE
#define DIK_SLEEP           0xDF
#define DIK_WAKE            0xE3
#define DIK_WEBSEARCH       0xE5
#define DIK_WEBFAVORITES    0xE6
#define DIK_WEBREFRESH      0xE7
#define DIK_WEBSTOP         0xE8
#define DIK_WEBFORWARD      0xE9
#define DIK_WEBBACK         0xEA
#define DIK_MYCOMPUTER      0xEB
#define DIK_MAIL            0xEC
#define DIK_MEDIASELECT     0xED

/* ------------------------------------------------------------------ */
/*  DIMOFS mouse offsets                                                */
/* ------------------------------------------------------------------ */

#define DIMOFS_X        0
#define DIMOFS_Y        4
#define DIMOFS_Z        8
#define DIMOFS_BUTTON0  12
#define DIMOFS_BUTTON1  13
#define DIMOFS_BUTTON2  14
#define DIMOFS_BUTTON3  15
#define DIMOFS_BUTTON4  16
#define DIMOFS_BUTTON5  17
#define DIMOFS_BUTTON6  18
#define DIMOFS_BUTTON7  19

/* ------------------------------------------------------------------ */
/*  DIJOFS joystick offsets                                             */
/* ------------------------------------------------------------------ */

#define DIJOFS_X            0
#define DIJOFS_Y            4
#define DIJOFS_Z            8
#define DIJOFS_RX           12
#define DIJOFS_RY           16
#define DIJOFS_RZ           20
#define DIJOFS_SLIDER(n)    (24 + (n) * 4)
#define DIJOFS_POV(n)       (32 + (n) * 4)
#define DIJOFS_BUTTON(n)    (48 + (n))

/* ------------------------------------------------------------------ */
/*  Data retrieval flags                                               */
/* ------------------------------------------------------------------ */

#define DIGDD_PEEK 0x00000001

#ifdef __cplusplus
}
#endif

#endif /* _DINPUT_H_ */
