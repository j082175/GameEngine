//RWTexture2D<float4> densityField : register(u0);

[numthreads(1024, 1, 1)]
void main(int3 gID : SV_GroupID, int3 gtID : SV_GroupThreadID,
          uint3 dtID : SV_DispatchThreadID)
{
	for (int i = 0; i < m_particlesCpu.size(); i++)
	{

		if (m_particlesCpu[i].life < 0.0f)
			continue;

		Vector3 pressureForce
		(0.0f);
		Vector3 viscosityForce
		(0.0f);

		const float& rho_i = m_particlesCpu[i].density;
		const float& p_i = m_particlesCpu[i].pressure;
		const Vector3& x_i = m_particlesCpu[i].position;
		const Vector3& v_i = m_particlesCpu[i].velocity;

		for (size_t j = 0; j < m_particlesCpu.size(); j++)
		{

			if (m_particlesCpu[j].life < 0.0f)
				continue;

			if (i == j)
				continue;

			const float& rho_j = m_particlesCpu[j].density;
			const float& p_j = m_particlesCpu[j].pressure;
			const Vector3& x_j = m_particlesCpu[j].position;
			const Vector3 x_ij = x_i - x_j;
			const Vector3& v_j = m_particlesCpu[j].velocity;

			const float dist = (x_i - x_j).Length();

			if (dist >= m_radius)
				continue;

			if (dist < 1e-3f) // ��ġ ���� ����
				continue;

            // ��Ʈ: SphKernels::CubicSplineGrad() ���
            // const Vector3 gradPressure = Vector3(0.0f);

            // const Vector3 laplacianVelocity = Vector3(0.0f);

			Vector3 x_ijNorm = x_ij;
			x_ijNorm.Normalize();

			const Vector3 gradPressure =
                rho_i * m_mass *
                (p_i / pow(rho_i, 2.f) + p_j / pow(rho_j, 2.f)) *
                SphKernels::CubicSplineGrad(dist * 2.f / m_radius) * x_ijNorm;

			const Vector3 laplacianVelocity =
                2.f * m_mass / rho_j * (v_i - v_j) *
                (x_ij * SphKernels::CubicSplineGrad(dist * 2.f / m_radius) *
                 x_ijNorm /
                 (x_ij.LengthSquared() + 0.01f * pow(m_radius, 2.f)));

			pressureForce -= m_mass / rho_i * gradPressure;
			viscosityForce += m_mass * m_viscosity * laplacianVelocity;
		}

		m_particlesCpu[i].force = pressureForce + viscosityForce;
	}
}
