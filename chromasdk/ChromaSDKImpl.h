
#ifndef _CHROMASDKIMPL2_H_
#define _CHROMASDKIMPL2_H_

#pragma once

#include "RzChromaSDKDefines.h"
#include "RzChromaSDKTypes.h"
#include "RzErrors.h"

const COLORREF BLACK = RGB(0,0,0);
const COLORREF WHITE = RGB(255,255,255);
const COLORREF RED = RGB(255,0,0);
const COLORREF GREEN = RGB(0,255,0);
const COLORREF BLUE = RGB(0,0,255);
const COLORREF YELLOW = RGB(255,255,0);
const COLORREF PURPLE = RGB(128,0,128);
const COLORREF CYAN = RGB(00,255,255);
const COLORREF ORANGE = RGB(255,165,00);
const COLORREF PINK = RGB(255,192,203);
const COLORREF GREY = RGB(125, 125, 125);

class CChromaSDKImpl
{
public:
    CChromaSDKImpl();
    ~CChromaSDKImpl();

public:
    void Initialize();
    void UnInitialize();

    void CreateEffectImpl(RZDEVICEID DeviceId, ChromaSDK::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId=NULL);
    void CreateKeyboardEffectImpl(ChromaSDK::Keyboard::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId=NULL);
    void CreateMouseEffectImpl(ChromaSDK::Mouse::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId=NULL);
    void CreateMousematEffectImpl(ChromaSDK::Mousepad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId=NULL);
    void CreateKeypadEffectImpl(ChromaSDK::Keypad::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId=NULL);
    void CreateHeadsetEffectImpl(ChromaSDK::Headset::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId=NULL);
    void CreateChromaLinkEffectImpl(ChromaSDK::ChromaLink::EFFECT_TYPE Effect, PRZPARAM pParam, RZEFFECTID *pEffectId=NULL);
    void SetEffectImpl(RZEFFECTID EffectId);
    void DeleteEffectImpl(RZEFFECTID EffectId);
    void StopEffectImpl(RZEFFECTID EffectId);

    void CreateEffectGroup(RZEFFECTID *pGroupEffectId, BOOL Repeat=FALSE);
    void AddToGroup(RZEFFECTID GroupEffectId, RZEFFECTID EffectId, LONG DelayMS=100);

private:
    HMODULE m_hModule;
    HANDLE m_hEvent;
};

extern CChromaSDKImpl g_ChromaSDKImpl;

#endif


