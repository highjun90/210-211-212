#ifndef _VALUE
#define _VALUE

#include "struct.fx"

#define PI 3.1415926535f

// 4096 크기제한
cbuffer TRANSFORM : register(b0)
{
    row_major Matrix g_matWorld;
    row_major Matrix g_matWorldInv;

    row_major Matrix g_matView;
    row_major Matrix g_matViewInv;

    row_major Matrix g_matProj;
    row_major Matrix g_matProjInv;

    row_major Matrix g_matWV;
    row_major Matrix g_matWVP;
}

cbuffer MATERIAL_CONST : register(b1)
{
    int g_int_0;
    int g_int_1;
    int g_int_2;
    int g_int_3;
    
    float g_float_0;
    float g_float_1;
    float g_float_2;
    float g_float_3;
    
    float2 g_vec2_0;
    float2 g_vec2_1;
    float2 g_vec2_2;
    float2 g_vec2_3;
    
    float4 g_vec4_0;
    float4 g_vec4_1;
    float4 g_vec4_2;
    float4 g_vec4_3;
    
    row_major matrix g_mat_0;
    row_major matrix g_mat_1;
    row_major matrix g_mat_2;
    row_major matrix g_mat_3;
    
    int g_btex_0;
    int g_btex_1;
    int g_btex_2;
    int g_btex_3;
    int g_btex_4;
    int g_btex_5;
    int g_btexcube_0;
    int g_btexcube_1;
    int g_btexarr_0;
    int g_btexarr_1;
}

cbuffer ANIM_DATA2D : register(b2)
{
    float2  g_vLeftTop;
    float2  g_vSlizeSize;
    float2  g_vBackground;
    float2  g_vOffset;    
    int     g_UseAnim2D;
    float3  padding;
}

cbuffer GLOBAL_DATA : register(b3)
{
    float2  g_RenderResolution; // 렌더링 해상도
    float2 g_NoiseTexResolution; // Noise 텍스쳐 해상도
    float   g_dt;               // Delta Time
    float   g_time;             // 누적 시간
    int     g_Light2DCount;     // 2D 광원 개수
    int     g_Light3DCount;     // 3D 광원 개수
    float2  globalpadding;
}


Texture2D g_tex_0 : register(t0);
Texture2D g_tex_1 : register(t1);
Texture2D g_tex_2 : register(t2);
Texture2D g_tex_3 : register(t3);
Texture2D g_tex_4 : register(t4);
Texture2D g_tex_5 : register(t5);

TextureCube g_texcube_0 : register(t6);
TextureCube g_texcube_1 : register(t7);

Texture2DArray g_texarr_0 : register(t8);
Texture2DArray g_texarr_1 : register(t9);

Texture2D g_anim2d_tex : register(t10);

StructuredBuffer<tLightInfo> g_Light2D : register(t11);
StructuredBuffer<tLightInfo> g_Light3D : register(t12);

Texture2D g_postprocess : register(t13);

Texture2D g_NoiseTex : register(t14);

SamplerState g_sam_0 : register(s0);
SamplerState g_sam_1 : register(s1);


#endif