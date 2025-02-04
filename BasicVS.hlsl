#include "Common.hlsli" // 쉐이더에서도 include 사용 가능

// Vertex Shader에서도 텍스춰 사용
Texture2D g_heightTexture : register(t0);

PixelShaderInput main(VertexShaderInput input)
{
    // 뷰 좌표계는 NDC이기 때문에 월드 좌표를 이용해서 조명 계산
    
	PixelShaderInput output;
    
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
    
    uint indices[8]; // 힌트: 꼭 사용!
    indices[0] = input.boneIndices0.x;
    indices[1] = input.boneIndices0.y;
    indices[2] = input.boneIndices0.z;
    indices[3] = input.boneIndices0.w;
    indices[4] = input.boneIndices1.x;
    indices[5] = input.boneIndices1.y;
    indices[6] = input.boneIndices1.z;
    indices[7] = input.boneIndices1.w;

    float3 posModel = float3(0.0f, 0.0f, 0.0f);
    float3 normalModel = float3(0.0f, 0.0f, 0.0f);
    float3 tangentModel = float3(0.0f, 0.0f, 0.0f);
    
    // Uniform Scaling 가정
    // (float3x3)boneTransforms 캐스팅으로 Translation 제외
    for(int i = 0; i < 8; ++i)
    {
        // TODO:
		posModel += weights[i] * mul(float4(input.posModel, 1.f), boneTransforms[indices[i]]);
		normalModel += weights[i] * mul(float4(input.normalModel, 1.f), boneTransforms[indices[i]]);
		tangentModel += weights[i] * mul(float4(input.tangentModel, 1.f), boneTransforms[indices[i]]);
    }

    input.posModel = posModel;
    input.normalModel = normalModel;
    input.tangentModel = tangentModel;

#endif

	output.normalWorld = mul(float4(input.normalModel, 0.0f), worldIT).xyz;
	output.normalWorld = normalize(output.normalWorld);
	
	output.posWorld = mul(float4(input.posModel, 1.0f), world).xyz;

	if (useHeightMap)
	{
		float height = g_heightTexture.SampleLevel(linearClampSampler, input.texcoord, 0).r;
		height = height * 2.0 - 1.0;
		output.posWorld += output.normalWorld * height * heightScale;
	}

	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = input.texcoord;
	output.tangentWorld = mul(float4(input.tangentModel, 0.0f), world).xyz;

	return output;
}
