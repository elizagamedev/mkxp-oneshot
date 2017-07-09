
#include "FrameController.h"
#include <windows.h>

int g_dwCurrentTickCount = 0;

// CFrameController
CFrameController::CFrameController():
    m_FramesPerSec(-1),
    m_OneFrame(0)
{

}

CFrameController::CFrameController(long FramePerSec):
    m_FramesPerSec(FramePerSec),
    m_OneFrame(1000.0/m_FramesPerSec)
{
}

CFrameController::~CFrameController()
{

}

void CFrameController::Begin()
{
    g_dwCurrentTickCount = GetTickCount();
}

void CFrameController::End()
{
    static int dwSleep = 0;
    static int dwDiff = 0;

    dwDiff = GetTickCount() - g_dwCurrentTickCount;
    dwSleep = (int)(m_OneFrame - (double)dwDiff);

    if (dwDiff < m_OneFrame)
    {
        Sleep(dwSleep);
    }
}

void CFrameController::SetRate(long FramePerSec)
{
    m_FramesPerSec = FramePerSec;
    m_OneFrame = 1000.0 / m_FramesPerSec;
}

long CFrameController::GetRate(void)
{
    return m_FramesPerSec;
}
