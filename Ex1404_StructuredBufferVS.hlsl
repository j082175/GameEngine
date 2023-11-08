struct PSInput // GS가 있다면 GSInput으로 사용됨
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

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

StructuredBuffer<Particle> particles : register(t0);

// VSInput이 없이 vertexID만 사용
PSInput main(uint vertexID : SV_VertexID)
{
    Particle p = particles[vertexID];
    
    PSInput output;
    
    //output.pos = float4(p.pos.xyz, 1.0);
	
	float3 velocity = float3(-p.pos.y, p.pos.x, 0.f);
	p.pos += velocity;
	
    output.color = p.color;
	output.position = float4(p.pos, 1.f);

    return output;
}
