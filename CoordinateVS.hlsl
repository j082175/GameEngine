#include "Common.hlsli"

struct GSInput
{
	float4 posModel : SV_POSITION;
	float3 normalWorld : NORMAL;
};

GSInput main(VertexShaderInput input)
{
	GSInput output;

	output.posModel = float4(input.posModel, 1.0);
	output.normalWorld = input.normalModel;

	return output;
}
