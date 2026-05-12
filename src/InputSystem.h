#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <atomic>
#include <cmath>

# shared input state
struct InputState
{
    # head
    float headYaw   = 0.f;   
    float headPitch = 0.f;   
    float headX     = 0.f;   
    float headY     = 1.65f; 
    float headZ     = 0.f;

    # controller analogs
    float leftTrigger   = 0.f;
    float rightTrigger  = 0.f;
    float leftGrip      = 0.f;
    float rightGrip     = 0.f;
    float leftJoyX      = 0.f;
    float leftJoyY      = 0.f;
    float rightJoyX     = 0.f;
    float rightJoyY     = 0.f;

    # buttons
    bool  buttonA       = false;  
    bool  buttonB       = false;   
    bool  buttonX       = false;   
    bool  buttonY       = false;   
    bool  leftMenu      = false;
    bool  rightMenu     = false;
};

# inputthread
class InputThread
{
public:
    InputState  state;
    std::atomic<bool> running{ false };

    void Start()
    {
        running = true;
        
        RECT r{};
        GetWindowRect(GetDesktopWindow(), &r);
        m_cx = (r.left + r.right)  / 2;
        m_cy = (r.top  + r.bottom) / 2;
        SetCursorPos(m_cx, m_cy);

        m_thread = CreateThread(nullptr, 0, ThreadProc, this, 0, nullptr);
    }

    void Stop()
    {
        running = false;
        if (m_thread) { WaitForSingleObject(m_thread, 2000); CloseHandle(m_thread); }
    }

private:
    HANDLE m_thread = nullptr;
    int    m_cx = 0, m_cy = 0;

    static DWORD WINAPI ThreadProc(LPVOID param)
    {
        auto* self = reinterpret_cast<InputThread*>(param);
        self->Loop();
        return 0;
    }

    void Loop()
    {
        constexpr float MOVE_SPEED   = 0.03f;   
        constexpr float MOUSE_SENSE  = 0.003f; 
        constexpr float PITCH_LIMIT  = 1.48f;   

        while (running)
        {
            # mouse delta
            POINT p{};
            GetCursorPos(&p);
            int dx = p.x - m_cx;
            int dy = p.y - m_cy;
            SetCursorPos(m_cx, m_cy);

            state.headYaw   -= dx * MOUSE_SENSE;
            state.headPitch -= dy * MOUSE_SENSE;
            if (state.headPitch >  PITCH_LIMIT) state.headPitch =  PITCH_LIMIT;
            if (state.headPitch < -PITCH_LIMIT) state.headPitch = -PITCH_LIMIT;

            # movement
            float cy = cosf(state.headYaw);
            float sy = sinf(state.headYaw);

            if (GetAsyncKeyState('W') & 0x8000) { state.headX -= sy * MOVE_SPEED; state.headZ -= cy * MOVE_SPEED; }
            if (GetAsyncKeyState('S') & 0x8000) { state.headX += sy * MOVE_SPEED; state.headZ += cy * MOVE_SPEED; }
            if (GetAsyncKeyState('A') & 0x8000) { state.headX -= cy * MOVE_SPEED; state.headZ += sy * MOVE_SPEED; }
            if (GetAsyncKeyState('D') & 0x8000) { state.headX += cy * MOVE_SPEED; state.headZ -= sy * MOVE_SPEED; }
            if (GetAsyncKeyState(VK_SPACE) & 0x8000) state.headY += MOVE_SPEED;
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) state.headY -= MOVE_SPEED;

            
            state.leftTrigger  = (GetAsyncKeyState('Q') & 0x8000) ? 1.f : 0.f;
            state.rightTrigger = (GetAsyncKeyState('E') & 0x8000) ? 1.f : 0.f;
            state.leftGrip     = (GetAsyncKeyState('Z') & 0x8000) ? 1.f : 0.f;
            state.rightGrip    = (GetAsyncKeyState('C') & 0x8000) ? 1.f : 0.f;

            
            state.rightJoyX = ((GetAsyncKeyState(VK_RIGHT) & 0x8000) ? 1.f : 0.f)
                             - ((GetAsyncKeyState(VK_LEFT)  & 0x8000) ? 1.f : 0.f);
            state.rightJoyY = ((GetAsyncKeyState(VK_UP)    & 0x8000) ? 1.f : 0.f)
                             - ((GetAsyncKeyState(VK_DOWN)  & 0x8000) ? 1.f : 0.f);

            
            state.buttonA   = (GetAsyncKeyState('1') & 0x8000) != 0;
            state.buttonB   = (GetAsyncKeyState('2') & 0x8000) != 0;
            state.buttonX   = (GetAsyncKeyState('3') & 0x8000) != 0;
            state.buttonY   = (GetAsyncKeyState('4') & 0x8000) != 0;

            Sleep(11); 
        }
    }
};
