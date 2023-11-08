struct Particle
{
	float3 pos;
	float3 color;
};

cbuffer Consts : register(b0)
{
	float dt;
	float velocity;
}

//static float dt = 1 / 60.0; // ConstBuffer로 받아올 수 있음

//StructuredBuffer<Particle> inputParticles : register(t0); // SRV로 사용 가능
RWStructuredBuffer<Particle> outputParticles : register(u0);

[numthreads(1024, 1, 1)]
void main(int3 gID : SV_GroupID, int3 gtID : SV_GroupThreadID,
          uint3 dtID : SV_DispatchThreadID)
{
	Particle p = outputParticles[dtID.x]; // Read
    
    // float3 velocity = ...;
    // p.pos += ...;
	
	float3 velocity = float3(-p.pos.y, p.pos.x, 0.f);
	p.pos += velocity * dt;
    
	outputParticles[dtID.x].pos = p.pos; // Write
	
	//
	//p.color = float3(0.5f, 0.5f, 0.5f);
	//outputParticles[dtID.x].color = p.color;
}
