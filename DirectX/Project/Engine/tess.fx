#ifndef _TESS
#define _TESS

#include "value.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Tess(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPos = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}


// Hull Shader
struct PatchLevel
{
    float arrEdge[3] : SV_TessFactor;
    float Inside : SV_InsideTessFactor;
};

PatchLevel PatchConstFunc(InputPatch<VS_OUT, 3> _in)
{
    PatchLevel output = (PatchLevel) 0.f;

    output.arrEdge[0] = 2;
    output.arrEdge[1] = 2;
    output.arrEdge[2] = 2;    
    output.Inside = 2;    
    
    return output;
}

struct HS_OUT
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

[patchconstantfunc("PatchConstFunc")]
[outputtopology("triangle_cw")]
[domain("tri")]
[maxtessfactor(64)]
[partitioning("integer")]
[outputcontrolpoints(3)]
HS_OUT HS_Tess(InputPatch<VS_OUT, 3> _in, uint _idx : SV_OutputControlPointID)
{
    HS_OUT output = (HS_OUT) 0.f;
    
    output.vPos = _in[_idx].vPos;
    output.vUV = _in[_idx].vUV;
    
    return output;
}

struct DS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV;
};


DS_OUT DS_Tess(HS_OUT _in)
{
    DS_OUT output = (DS_OUT) 0.f;
    
    return output;
}


float4 PS_Tess(DS_OUT _in) : SV_Target
{
    float4 vColor = float4(1.f, 0.f, 1.f, 1.f);    
    return vColor;
}



#endif