#include "Common.hlsli"

struct GSInput
{
	float4 posModel : SV_POSITION;
	float3 normalModel : NORMAL;
};

struct PSInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR;
};

static const float lineScale = 2.f;

[maxvertexcount(6)]
void main(point GSInput input[1], inout LineStream<PSInput> outputStream)
{
	PSInput output;
    
	float4 posWorld = mul(input[0].posModel, world);
	float4 normalModel = float4(input[0].normalModel, 0.0);
	float4 normalWorld = mul(normalModel, worldIT);
	normalWorld = float4(normalize(normalWorld.xyz), 0.0);
	
	float4 center = float4(0.f, 0.f, 0.f, 1.f);
	float4 xAxis = float4(1.f, 0.f, 0.f, 1.f);
	float4 yAxis = float4(0.f, 1.f, 0.f, 1.f);
	float4 zAxis = float4(0.f, 0.f, 1.f, 1.f);
    
	output.pos = mul(center, viewProj);
	output.color = float3(1.0, 0.0, 0.0);
	outputStream.Append(output);
    
	output.pos = mul(center + lineScale * xAxis, viewProj);
	output.color = float3(1.0, 0.0, 0.0);
	outputStream.Append(output);
	
	output.pos = mul(center, viewProj);
	output.color = float3(0.0, 1.0, 0.0);
	outputStream.Append(output);
    
	output.pos = mul(center + lineScale * yAxis, viewProj);
	output.color = float3(0.0, 1.0, 0.0);
	outputStream.Append(output);
	
	output.pos = mul(center, viewProj);
	output.color = float3(0.0, 0.0, 1.0);
	outputStream.Append(output);
    
	output.pos = mul(center + lineScale * zAxis, viewProj);
	output.color = float3(0.0, 0.0, 1.0);
	outputStream.Append(output);
}
