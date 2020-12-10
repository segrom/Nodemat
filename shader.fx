cbuffer ConstantBuffer {
	matrix MVP;
};

struct VS_OUTPUT {
	float4 pos: SV_POSITION;
};

struct VS_INPUT {
	float4 pos: POSITION;
};

VS_OUTPUT vert(VS_INPUT input) {
	VS_OUTPUT output = (VS_OUTPUT)0;
	output.pos = mul(MVP, input.pos);
	return output;
}

float4 frag(VS_OUTPUT input) :SV_Target{
	return float4(1.0f,0.0f,1.0f,1.0f);
}
