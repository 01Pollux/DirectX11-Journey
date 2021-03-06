
#include "utils/pch.hpp"
#include "DeviceResources.hpp"
#include "utils/StepTimer.hpp"
#include "Camera.hpp"

namespace DX
{
    Camera::Camera(DeviceResources* d3dres, float fovy, float near_z, float far_z) :
        m_d3dRes(d3dres)
    {
        set_lens(fovy, m_d3dRes->GetAspectRatio(), near_z, far_z);
        m_d3dRes->RegisterDeviceNotify(this);
    }

    Camera::~Camera()
    {
        m_d3dRes->UnregisterDeviceNotify(this);
    }

    void Camera::OnDeviceWindowSizeChanged()
    {
        m_AspectRatio = m_d3dRes->GetAspectRatio();
        
        m_Projection = XMMatrixPerspectiveLH(
            m_FovY, m_AspectRatio, m_NearZ, m_FarZ
        );

        m_TViewProjection = XMMatrixTranspose(XMMatrixMultiply(get_view(), get_projection()));
        BoundingFrustum::CreateFromMatrix(m_Frustum, get_projection());
    }


    void Camera::update(uint64_t ticks)
    {
        float delta_time = static_cast<float>(Pleiades::StepTimer::TicksToSeconds(ticks));

        if (GetAsyncKeyState(VK_ADD) & 0x8000)
            this->walk<MoveDir::Y>(m_MoveSpeed * delta_time);

        if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
            this->walk<MoveDir::Y>(-m_MoveSpeed * delta_time);


        if (GetAsyncKeyState('A') & 0x8000)
            this->walk<MoveDir::X>(-m_MoveSpeed * delta_time);

        if (GetAsyncKeyState('D') & 0x8000)
            this->walk<MoveDir::X>(m_MoveSpeed * delta_time);


        if (GetAsyncKeyState('W') & 0x8000)
            this->walk<MoveDir::Z>(m_MoveSpeed * delta_time);

        if (GetAsyncKeyState('S') & 0x8000)
            this->walk<MoveDir::Z>(-m_MoveSpeed * delta_time);



        if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000)
            this->rotate<MoveDir::Yaw>(-m_RotationSpeed * delta_time);

        if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000)
            this->rotate<MoveDir::Yaw>(m_RotationSpeed * delta_time);


        if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000)
            this->rotate<MoveDir::X>(m_RotationSpeed * delta_time);

        if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000)
            this->rotate<MoveDir::X>(-m_RotationSpeed * delta_time);

        if (m_ViewDirty)
            update_viewmatrix();
    }

	
    void Camera::update_viewmatrix()
	{
        m_ViewDirty = false;
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

        m_TViewProjection = XMMatrixTranspose(XMMatrixMultiply(get_view(), get_projection()));
        BoundingFrustum::CreateFromMatrix(m_Frustum, get_projection());
    }
}