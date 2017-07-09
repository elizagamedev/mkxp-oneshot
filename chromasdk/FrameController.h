#ifndef _FRAMECONTROLLER_H_
#define _FRAMECONTROLLER_H_

#pragma once

class CFrameController
{
public:
    CFrameController();
    CFrameController(long FramePerSec);
    ~CFrameController();

    void Begin();
    void End();
    void SetRate(long FramePerSec);
    long GetRate();

private:
    long m_FramesPerSec;
    double m_OneFrame;
};

#endif
