#pragma once

#include <DirectXMath.h>
#include <DirectXCollision.h>

namespace DX
{
	class DeviceResources;
	using namespace DirectX;

	class Camera : public IDeviceNotify
	{
		using XMMatrix = XMMATRIX;
		using XMVector = XMVECTOR;
		using XMFloat3 = XMFLOAT3;

	public:
		Camera(DeviceResources* d3dres, float fovy = DX::XM_PIDIV4, float near_z = 1.f, float far_z = 1'000.f);
		Camera(const Camera&) = delete; Camera& operator=(const Camera&) = delete;
		Camera(Camera&&) = delete;		Camera& operator=(Camera&&) = delete;
		~Camera();

		void OnDeviceWindowSizeChanged() override;

	public:
		enum class MoveDir
		{
			Yaw,
			Pitch,
			Roll,

			X = Pitch,
			Y = Yaw,
			Z = Roll
		};

		void set_lens(float fovy, float aspect_ratio, float near_z, float far_z)
		{
			m_FovY = fovy;
			m_AspectRatio = aspect_ratio;
			m_NearZ = near_z;
			m_FarZ = far_z;

			m_NearZWindowHeight = 2.f * m_NearZ * tanf(.5f * m_FovY);
			m_FarZWindowHeight = 2.f * m_FarZ * tanf(.5f * m_FovY);

			m_Projection = XMMatrixPerspectiveLH(
				m_FovY, m_AspectRatio, m_NearZ, m_FarZ
			);

			m_TViewProjection = XMMatrixTranspose(XMMatrixMultiply(get_view(), get_projection()));
			BoundingFrustum::CreateFromMatrix(m_Frustum, get_projection());
		}

		template<MoveDir direction>
		void walk(float dist)
		{
			m_ViewDirty = true;
			XMVector dir;
			if constexpr (direction == MoveDir::X)
				dir = XMLoadFloat3(&m_Right);
			else if constexpr (direction == MoveDir::Y)
				dir = XMLoadFloat3(&m_Up);
			else
				dir = XMLoadFloat3(&m_Look);

			// m_Position += dist * m_Look;
			XMStoreFloat3(
				&m_Position,
				XMVectorMultiplyAdd(
					XMVectorReplicate(dist),
					dir,
					XMLoadFloat3(&m_Position)
				)
			);
		}

		template<MoveDir rotation>
		void rotate(float angle)
		{
			m_ViewDirty = true;

			XMVector rotvec;
			if constexpr (rotation == MoveDir::X)
				rotvec = XMVectorSet(angle, 0.f, 0.f, 0.f);
			else if constexpr (rotation == MoveDir::Y)
				rotvec = XMVectorSet(0.f, angle, 0.f, 0.f);
			else
				rotvec = XMVectorSet(0.f, 0.f, angle, 0.f);

			XMMatrix rot = XMMatrixRotationRollPitchYawFromVector(rotvec);

			if constexpr (rotation != MoveDir::X)
			{
				XMStoreFloat3(
					&m_Right,
					XMVector3Transform(XMLoadFloat3(&m_Right), rot)
				);
			}
			
			XMStoreFloat3(
				&m_Look,
				XMVector3Transform(XMLoadFloat3(&m_Look), rot)
			);
			
			XMStoreFloat3(
				&m_Up,
				XMVector3Transform(XMLoadFloat3(&m_Up), rot)
			);
		}
		
		void update(uint64_t ticks);

	public:
		[[nodiscard]]
		const XMMatrix& get_view() const noexcept
		{
			return m_View;
		}
		
		[[nodiscard]]
		const XMMatrix& get_projection() const noexcept
		{
			return m_Projection;
		}
		
		[[nodiscard]]
		const XMMatrix& get_tviewprojection() const noexcept
		{
			return m_TViewProjection;
		}

		[[nodiscard]]
		const BoundingFrustum& get_frustum() const noexcept
		{
			return m_Frustum;
		}

	public:
		[[nodiscard]]
		const XMFloat3& position() const noexcept
		{
			return m_Position;
		}
		
		void set_position(const XMFloat3& position) noexcept
		{
			m_Position = position;
			m_ViewDirty = true;
		}
		
		void set_position(float x, float y, float z) noexcept
		{
			set_position({ x, y, z });
		}

		[[nodiscard]]
		const XMFloat3& right() const noexcept
		{
			return m_Right;
		}
		[[nodiscard]]
		const XMFloat3& look() const noexcept
		{
			return m_Look;
		}

		[[nodiscard]]
		const XMFloat3& up() const noexcept
		{
			return m_Up;
		}

	public:
		[[nodiscard]]
		float near_z() const noexcept
		{
			return m_NearZ;
		}
		[[nodiscard]]
		float near_zw_height() const noexcept
		{
			return m_NearZWindowHeight;
		}
		[[nodiscard]]
		float near_zw_width() const noexcept
		{
			return near_zw_height() * aspect_ratio();
		}

		[[nodiscard]]
		float far_z() const noexcept
		{
			return m_FarZ;
		}
		[[nodiscard]]
		float far_zw_height() const noexcept
		{
			return m_FarZWindowHeight;
		}
		[[nodiscard]]
		float far_zw_width() const noexcept
		{
			return far_zw_height() * aspect_ratio();
		}

		[[nodiscard]]
		float aspect_ratio() const noexcept
		{
			return m_AspectRatio;
		}
		[[nodiscard]]
		float fov_y() const noexcept
		{
			return m_FovY;
		}
		[[nodiscard]]
		float fov_x() const noexcept
		{
			float width = 0.5f * near_zw_width();
			return 2.f * atanf(tanf(width / near_z()));
		}

		void set_move_speed(float speed) noexcept
		{
			m_MoveSpeed = speed;
		}

		void set_rotation_speed(float speed) noexcept
		{
			m_RotationSpeed = speed;
		}

	private:
		void update_viewmatrix();

	private:
		XMMatrix m_View = XMMatrixIdentity(),
			m_Projection = XMMatrixIdentity(),
			m_TViewProjection = XMMatrixIdentity();

		XMFloat3 m_Position{},
			m_Right{ 1.f, 0.f, 0.f }, m_Look{ 0.f, 0.f, 1.f }, m_Up{ 0.f, 1.f, 0.f };

		BoundingFrustum m_Frustum;

		float m_NearZ{}, m_NearZWindowHeight{};
		float m_FarZ{}, m_FarZWindowHeight{};
		float m_AspectRatio{};
		float m_FovY{};
		float m_MoveSpeed = 10.f, m_RotationSpeed = 0.75f;

		DeviceResources* m_d3dRes;
		bool m_ViewDirty{};
	};
}