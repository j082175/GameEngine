#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

float4 main(VertexInstanceShaderInput input) : SV_POSITION
{
    
#ifdef SKINNED
    
    // 참고 자료: Luna DX 12 교재
    
    float weights[8];
    weights[0] = input.boneWeights0.x;
    weights[1] = input.boneWeights0.y;
    weights[2] = input.boneWeights0.z;
    weights[3] = input.boneWeights0.w;
    weights[4] = input.boneWeights1.x;
    weights[5] = input.boneWeights1.y;
    weights[6] = input.boneWeights1.z;
    weights[7] = input.boneWeights1.w;
    
    uint indices[8];
    indices[0] = input.boneIndices0.x;
    indices[1] = input.boneIndices0.y;
    indices[2] = input.boneIndices0.z;
    indices[3] = input.boneIndices0.w;
    indices[4] = input.boneIndices1.x;
    indices[5] = input.boneIndices1.y;
    indices[6] = input.boneIndices1.z;
    indices[7] = input.boneIndices1.w;

    float3 posModel = float3(0.0f, 0.0f, 0.0f);
    
    // Uniform Scaling 가정 (worldIT 불필요)
    // (float3x3) 캐스팅으로 Translation 제외
    for(int i = 0; i < 8; ++i)
    {
        // TODO: 
		posModel += weights[i] * mul(float4(input.posModel, 1.f), boneTransforms[indices[i]]);
    }

     input.posModel = posModel;

#endif
	
	    //참고: windTrunk, windLeaves 옵션도 skinnedMesh처럼 매크로 사용 가능
	if (windTrunk != 0.0)
	{
		float2 rotCenter = float2(0.0f, -0.5f);
		float2 temp = (input.posModel.xy - rotCenter);
		float coeff = windTrunk * pow(max(0, temp.y), 2.0) * sin(globalTime);
        
		float2x2 rot = float2x2(cos(coeff), sin(coeff), -sin(coeff), cos(coeff));
        
		input.posModel.xy = mul(temp, rot);
		input.posModel.xy += rotCenter;
	}
    
	if (windLeaves != 0.0)
	{
		float3 windVel = float3(sin(input.posModel.x * 100.0 + globalTime * 0.1)
                                * cos(input.posModel.y * 100 + globalTime * 0.1), 0, 0)
                                * sin(globalTime * 10.0);

        // float3 coeff = windLeaves * ... ;
        
        // input.posModel.xyz += coeff;
	}
    
	//float4 pos = mul(float4(input.posModel, 1.0f), world);
	

	float4 pos = mul(float4(input.posModel, 1.f), input.insWorld);
	pos = mul(pos, world);
	
	
	return mul(pos, viewProj);
}
