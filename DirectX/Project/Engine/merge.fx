#ifndef _MERGE
#define _MERGE

#include "value.fx"


// ===============
// Merge Shader
// MRT : SwapChain
// Mesh : RectMesh
// g_tex_0 : ColorTargetTex
// g_tex_1 : DiffuseTargetTex
// g_tex_2 : SpecularTargetTex
// g_tex_3 : EmissiveTargetTex
// ===============

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
};

VS_OUT VS_Merge(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

float4 PS_Merge(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
        
    float4 vColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    float4 vDiffuse = g_tex_1.Sample(g_sam_0, _in.vUV);
    float4 Specular = g_tex_2.Sample(g_sam_0, _in.vUV);
    float4 Emissive = g_tex_3.Sample(g_sam_0, _in.vUV);
        
    vOutColor = (vColor * (vDiffuse + Emissive)) + Specular;
    vOutColor.a = 1.f;   
    
    return vOutColor;
}





#endif