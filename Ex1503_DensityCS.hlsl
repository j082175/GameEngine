RWTexture2D<float4> densityField : register(u0);

struct Particle
{
	float3 position = float3(0.0f);
	float3 velocity = float3(0.0f);
	float3 force = float3(0.0f);
	float density = 0.0f;
	float pressure = 0.0f;

	float3 color = float3(1.0f);
	float life = 0.0f;
	float size = 1.0f;
};

[numthreads(1024, 1, 1)]
void main(int3 gID : SV_GroupID, int3 gtID : SV_GroupThreadID,
          uint3 dtID : SV_DispatchThreadID)
{
	
	//int i = dtID.x;
	

	//if (m_particlesCpu[i].life < 0.0f)
	//	continue;

	//m_particlesCpu[i].density = 0.0f;

 //       // the summation over j includes all particles
 //       // i와 j가 같을 경우에도 고려한다는 의미
 //       // https://en.wikipedia.org/wiki/Smoothed-particle_hydrodynamics

	//for (size_t j = 0; j < m_particlesCpu.size(); j++)
	//{

	//	if (m_particlesCpu[j].life < 0.0f)
	//		continue;

	//	const float dist =
 //               (m_particlesCpu[i].position - m_particlesCpu[j].position)
 //                   .Length();

	//	if (dist >= m_radius)
	//		continue;

	//	m_particlesCpu[i].density +=
 //               m_mass * SphKernels::CubicSpline(dist * 2.0f / m_radius);
	//}

	//m_particlesCpu[i].pressure =
 //           m_pressureCoeff *
 //           (pow(m_particlesCpu[i].density / m_density0, 7.0f) - 1.0f);
	
}
