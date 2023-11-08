RWTexture2D<float4> densityField : register(u0);

[numthreads(32, 32, 1)]
void main(int3 gID : SV_GroupID, int3 gtID : SV_GroupThreadID,
          uint3 dtID : SV_DispatchThreadID)
{
    const float dissipation = 0.1f; // 시간을 곱해도 됩니다.

    // TODO: ???
	
	float3 color = densityField[dtID.xy].rgb - dissipation;
	
	color = max(float3(0.f, 0.f, 0.f), color);
	
	densityField[dtID.xy] = float4(color, 1.f);
}
