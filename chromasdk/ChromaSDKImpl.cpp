//! \example ChromaSDKImpl.cpp

#include <map>
#include <vector>
#include <windows.h>
#include "ChromaSDKImpl.h"
#include "FrameController.h"

CChromaSDKImpl g_ChromaSDKImpl;

#ifdef _WIN64
#define CHROMASDKDLL        L"RzChromaSDK64.dll"
#else
#define CHROMASDKDLL        L"RzChromaSDK.dll"
#endif

using namespace std;
using namespace ChromaSDK;
using namespace ChromaSDK::Keyboard;
using namespace ChromaSDK::Keypad;
using namespace ChromaSDK::Mouse;
using namespace ChromaSDK::Mousepad;
using namespace ChromaSDK::Headset;

typedef RZRESULT (*INIT)(void);
typedef RZRESULT (*UNINIT)(void);
typedef RZRESULT (*CREATEEFFECT)(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEKEYBOARDEFFECT)(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEHEADSETEFFECT)(ChromaSDK::Headset::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEMOUSEPADEFFECT)(ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEMOUSEEFFECT)(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATEKEYPADEFFECT)(ChromaSDK::Keypad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*CREATECHROMALINKEFFECT)(ChromaSDK::ChromaLink::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId);
typedef RZRESULT (*SETEFFECT)(RZEFFECTID EffectId);
typedef RZRESULT (*DELETEEFFECT)(RZEFFECTID EffectId);
typedef RZRESULT (*REGISTEREVENTNOTIFICATION)(HWND hWnd);
typedef RZRESULT (*UNREGISTEREVENTNOTIFICATION)(void);
typedef RZRESULT (*QUERYDEVICE)(RZDEVICEID DeviceId, ChromaSDK::DEVICE_INFO_TYPE &DeviceInfo);

INIT Init = NULL;
UNINIT UnInit = NULL;
CREATEEFFECT CreateEffect = NULL;
CREATEKEYBOARDEFFECT CreateKeyboardEffect = NULL;
CREATEMOUSEEFFECT CreateMouseEffect = NULL;
CREATEHEADSETEFFECT CreateHeadsetEffect = NULL;
CREATEMOUSEPADEFFECT CreateMousematEffect = NULL;
CREATEKEYPADEFFECT CreateKeypadEffect = NULL;
CREATECHROMALINKEFFECT CreateChromaLinkEffect = NULL;
SETEFFECT SetEffect = NULL;
DELETEEFFECT DeleteEffect = NULL;
QUERYDEVICE QueryDevice = NULL;

#define EVENT_NAME  L"{4784D90A-1179-4F7D-8558-52511D809190}"

#define MAX_EFFECTS     100

typedef struct _EFFECTDATATYPE
{
    long numEffects;
    BOOL repeat;
    HANDLE thread;
    struct _EFFECT
    {
        RZEFFECTID id;
        long delay;
    } Effect[MAX_EFFECTS];
} EFFECTDATATYPE;

struct GUIDCompare
{
    BOOL operator()(const GUID & Left, const GUID & Right) const
    {
        return memcmp(&Left , &Right,sizeof(Right)) < 0;
    }
};

std::map<RZEFFECTID, EFFECTDATATYPE, GUIDCompare> g_Effects;

DWORD WINAPI Thread_RenderEffects(LPVOID lpParameter)
{
    RZEFFECTID *pEffectId = (RZEFFECTID*)lpParameter;

    auto iterator = g_Effects.find(*pEffectId);
    if(iterator != g_Effects.end())
    {
        EFFECTDATATYPE *pEffectData = &iterator->second;

        CFrameController FrameControl(30);

        if(pEffectData->repeat == FALSE)
        {
            for(int i=0; i<pEffectData->numEffects; i++)
            {
                FrameControl.Begin();

                SetEffect(pEffectData->Effect[i].id);

                Sleep(pEffectData->Effect[i].delay);

                FrameControl.End();
            }
        }
        else
        {
            while(pEffectData->repeat)
            {
                for(int i=0; i<pEffectData->numEffects; i++)
                {
                    FrameControl.Begin();

                    SetEffect(pEffectData->Effect[i].id);

                    Sleep(pEffectData->Effect[i].delay);

                    FrameControl.End();
                }
            };
        }
    }

    return 0;
}

// CChromaSDKImpl

CChromaSDKImpl::CChromaSDKImpl():
    m_hModule(NULL),
    m_hEvent(NULL)
{
}

CChromaSDKImpl::~CChromaSDKImpl()
{
}

void CChromaSDKImpl::Initialize()
{
    if(m_hModule == NULL)
    {
        m_hModule =::LoadLibrary(CHROMASDKDLL);
        if(m_hModule != NULL)
        {
            INIT Init = (INIT)::GetProcAddress(m_hModule, "Init");
            if(Init != NULL)
            {
                RZRESULT rzResult = Init();
                if(rzResult == RZRESULT_SUCCESS)
                {
                    CreateEffect = (CREATEEFFECT)::GetProcAddress(m_hModule, "CreateEffect");
                    CreateKeyboardEffect = (CREATEKEYBOARDEFFECT)::GetProcAddress(m_hModule, "CreateKeyboardEffect");
                    CreateMouseEffect = (CREATEMOUSEEFFECT)::GetProcAddress(m_hModule, "CreateMouseEffect");
                    CreateMousematEffect = (CREATEMOUSEPADEFFECT)::GetProcAddress(m_hModule, "CreateMousepadEffect");
                    CreateKeypadEffect = (CREATEKEYPADEFFECT)::GetProcAddress(m_hModule, "CreateKeypadEffect");
                    CreateHeadsetEffect = (CREATEHEADSETEFFECT)::GetProcAddress(m_hModule, "CreateHeadsetEffect");
                    CreateChromaLinkEffect = (CREATECHROMALINKEFFECT)::GetProcAddress(m_hModule, "CreateChromaLinkEffect");
                    SetEffect = (SETEFFECT)GetProcAddress(m_hModule, "SetEffect");
                    DeleteEffect = (DELETEEFFECT)GetProcAddress(m_hModule, "DeleteEffect");
                }
            }
        }
    }

    if(m_hEvent == NULL)
    {
        m_hEvent = ::CreateEvent(NULL, TRUE, FALSE, EVENT_NAME);
    }
}

void CChromaSDKImpl::UnInitialize()
{
    // Free memeory
    while(!g_Effects.empty())
    {
        auto iterator = g_Effects.begin();
        for(int i=0; i<iterator->second.numEffects; i++)
        {
            DeleteEffect(iterator->second.Effect[i].id);
        }

        g_Effects.erase(iterator);
    };

    if(m_hEvent != NULL)
    {
        ::CloseHandle(m_hEvent);
        m_hEvent = NULL;
    }

    if(m_hModule != NULL)
    {
        UNINIT UnInit = (UNINIT)::GetProcAddress(m_hModule, "UnInit");
        if(UnInit != NULL)
        {
            RZRESULT rzResult = UnInit();
            if(rzResult != RZRESULT_SUCCESS)
            {
                // Some error here
            }
        }

        ::FreeLibrary(m_hModule);
        m_hModule = NULL;
    }
}

void CChromaSDKImpl::CreateEffectImpl(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId)
{
    if(CreateEffect == NULL) return;

    CreateEffect(DeviceId, Effect, pParam, pEffectId);
}

void CChromaSDKImpl::CreateKeyboardEffectImpl(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId)
{
    if(CreateKeyboardEffect == NULL) return;

    CreateKeyboardEffect(Effect, pParam, pEffectId);
}

void CChromaSDKImpl::CreateMouseEffectImpl(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId)
{
    if(CreateMouseEffect == NULL) return;

    CreateMouseEffect(Effect, pParam, pEffectId);
}

void CChromaSDKImpl::CreateMousematEffectImpl(ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId)
{
    if(CreateMousematEffect == NULL) return;

    CreateMousematEffect(Effect, pParam, pEffectId);
}

void CChromaSDKImpl::CreateKeypadEffectImpl(ChromaSDK::Keypad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId)
{
    if(CreateKeypadEffect == NULL) return;

    CreateKeypadEffect(Effect, pParam, pEffectId);
}

void CChromaSDKImpl::CreateHeadsetEffectImpl(ChromaSDK::Headset::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId)
{
    if(CreateHeadsetEffect == NULL) return;

    CreateHeadsetEffect(Effect, pParam, pEffectId);
}

void CChromaSDKImpl::CreateChromaLinkEffectImpl(ChromaSDK::ChromaLink::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId)
{
    if(CreateChromaLinkEffect == NULL) return;

    CreateChromaLinkEffect(Effect, pParam, pEffectId);
}

void CChromaSDKImpl::SetEffectImpl(RZEFFECTID EffectId)
{
    auto iterator = g_Effects.find(EffectId);
    if(iterator != g_Effects.end())
    {
        if(iterator->second.repeat == FALSE)
        {
            HANDLE hThread = CreateThread(NULL, 0, Thread_RenderEffects, (LPVOID)&iterator->first, 0, NULL);
            if(hThread != NULL)
            {
                CloseHandle(hThread);
            }
        }
        else
        {
            HANDLE hThread = CreateThread(NULL, 0, Thread_RenderEffects, (LPVOID)&iterator->first, 0, NULL);
            if(hThread != NULL)
            {
                iterator->second.thread = hThread;
            }
        }
    }
    else
    {
        if(SetEffect == NULL) return;

        SetEffect(EffectId);
    }
}

void CChromaSDKImpl::DeleteEffectImpl(RZEFFECTID EffectId)
{
    auto iterator = g_Effects.find(EffectId);
    if(iterator != g_Effects.end())
    {
        EFFECTDATATYPE EffectData = iterator->second;
        for(int i=0; i<EffectData.numEffects; i++)
        {
            DeleteEffect(EffectData.Effect[i].id);
        }

        g_Effects.erase(iterator);
    }
    else
    {
        if(DeleteEffect == NULL) return;

        DeleteEffect(EffectId);
    }
}

void CChromaSDKImpl::StopEffectImpl(RZEFFECTID EffectId)
{
    auto iterator = g_Effects.find(EffectId);
    if(iterator != g_Effects.end())
    {
        if((iterator->second.repeat == TRUE) &&
            (iterator->second.thread != NULL))
        {
            iterator->second.repeat = FALSE;

            CloseHandle(iterator->second.thread);

            iterator->second.thread = NULL;
        }
    }
}

void CChromaSDKImpl::CreateEffectGroup(RZEFFECTID *pGroupEffectId, BOOL Repeat)
{
    RZEFFECTID EffectId = GUID_NULL;
    if(SUCCEEDED(::CoCreateGuid(&EffectId)))
    {
        EFFECTDATATYPE EffectData = {};

        EffectData.numEffects = 0;
        EffectData.repeat = Repeat;

        g_Effects.insert(make_pair(EffectId, EffectData));

        *pGroupEffectId = EffectId;
    }
}

void CChromaSDKImpl::AddToGroup(RZEFFECTID GroupEffectId, RZEFFECTID EffectId, long DelayMS)
{
    auto iterator = g_Effects.find(GroupEffectId);
    if(iterator != g_Effects.end())
    {
        long lIndex = iterator->second.numEffects;

        iterator->second.Effect[lIndex].id = EffectId;
        iterator->second.Effect[lIndex].delay = DelayMS;

        iterator->second.numEffects++;
    }
}
