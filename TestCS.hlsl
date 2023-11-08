RWTexture2D<float4> output : register(u0);

cbuffer Constants : register(b0)
{
	float scale;
};

[numthreads(32, 32, 1)]
void main(uint3 gID : SV_GroupID, uint3 gtID : SV_GroupThreadID,
          uint3 dtID : SV_DispatchThreadID)
{
	
	if (gID.x % 2 == 0 ^ gID.y % 2 == 0)
	{
		output[dtID.xy] = float4(1.f, 1.f, 1.f, 1.f);
	}
	else
	{
		output[dtID.xy] = float4(0.f, 0.f + clamp(scale, 0.f, 1.f), 0.f, 1.f);
	}
	
	//output[gtID.xy] = float4(1.f, 0.f, 1.f, 1.f);
}

