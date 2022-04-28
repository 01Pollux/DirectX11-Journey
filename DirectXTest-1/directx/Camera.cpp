
#include "utils/pch.hpp"
#include "utils/StepTimer.hpp"
#include "Camera.hpp"

namespace DX
{
    void Camera::update(uint64_t ticks)
    {
        constexpr float move_scale = 10.f, rotation_scale = 0.8f, strafe_scale = 10.f;
        float delta_time = static_cast<float>(Pleiades::StepTimer::TicksToSeconds(ticks));

        if (GetAsyncKeyState('W') & 0x8000)
            this->walk<MoveDir::Y>(move_scale * delta_time);

        if (GetAsyncKeyState('S') & 0x8000)
            this->walk<MoveDir::Y>(-move_scale * delta_time);


        if (GetAsyncKeyState('A') & 0x8000)
            this->walk<MoveDir::X>(-strafe_scale * delta_time);

        if (GetAsyncKeyState('D') & 0x8000)
            this->walk<MoveDir::X>(strafe_scale * delta_time);


        if (GetAsyncKeyState(VK_ADD) & 0x8000)
            this->walk<MoveDir::Z>(strafe_scale * delta_time);

        if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
            this->walk<MoveDir::Z>(-strafe_scale * delta_time);



        if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
            this->rotate<MoveDir::Yaw>(-rotation_scale * delta_time);

        if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
            this->rotate<MoveDir::Yaw>(rotation_scale * delta_time);


        if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000)
            this->rotate<MoveDir::X>(rotation_scale * delta_time);

        if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
            this->rotate<MoveDir::X>(-rotation_scale * delta_time);

        update_viewmatrix();
    }

	
    void Camera::update_viewmatrix()
	{
		XMVector position = XMLoadFloat3(&m_Position),
			right = XMLoadFloat3(&m_Right),
			look = XMLoadFloat3(&m_Look),
			up = XMLoadFloat3(&m_Up);

        m_View = XMMatrixLookToLH(position, look, up);

        look = XMVector3Normalize(look);
        XMStoreFloat3(
            &m_Look,
            look
        );

        up = XMVector3Normalize(XMVector3Cross(look, right));
        XMStoreFloat3(
            &m_Up,
            up
        );

        XMStoreFloat3(
            &m_Right,
            XMVector3Cross(up, look)
        );
	}
}