cbuffer TransformOffsetBuffer : register (b0)
{
    float4 offset;
	matrix matrixModel;
	matrix matrixView;
    matrix matrixProjection;
	float4 padding[3];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR, uint instanceId : SV_InstanceID)
{
    PSInput result;

    result.position = mul(position, matrixModel);
	result.position = mul(result.position, matrixView);
	result.position = mul(result.position, matrixProjection);
	result.position.x += instanceId * 0.6;
	
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}