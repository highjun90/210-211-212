#ifndef _LIGHT
#define _LIGHT

#include "value.fx"
#include "func.fx"


// ========================
// Directional Light Shader
// MRT      : LIGHT
// Mesh     : RectMesh
// DS_TYPE  : NO_TEST_NO_WIRTE
// BS_TYPE  : ONE_ONE , 여러개의 빛이 누적될 수 있게

// Parameter
// g_mat_1 : 광원시점의 View * Proj Matrix

// g_int_0 : Light Idex
// g_tex_0 : PositionTargetTex
// g_tex_1 : NormalTargetTex
// g_tex_2 : ShadowDepthTargetTex
// ========================
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

VS_OUT VS_DirLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = float4(_in.vPos * 2.f, 1.f);
    output.vUV = _in.vUV;
    
    return output;
}

struct PS_OUT
{
    float4 vDiffuse : SV_Target0;
    float4 vSpecular : SV_Target1;
};

PS_OUT PS_DirLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
        
    // PositionTarget 에서 현재 호출된 픽셀쉐이더랑 동일한 지점에 접근해서 좌표값을 확인
    float4 vViewPos = g_tex_0.Sample(g_sam_0, _in.vUV);
    
    // Deferred 단계에서 그려진게 없다면 빛을 줄 수 없다.
    if (-1.f == vViewPos.w)
        discard;
    
    // 해당 지점의 Normal 값을 가져온다.
    float3 vViewNormal = normalize(g_tex_1.Sample(g_sam_0, _in.vUV).xyz);
       
    // 해당 지점이 받을 빛의 세기를 구한다.
    tLightColor LightColor = (tLightColor)0.f;
    CalculateLight3D(g_int_0, vViewPos.xyz, vViewNormal, LightColor);
        
    // 그림자 확인
    float fShadowPow = 0.f;
    // ViewPos 를 ViewInv 행렬(메인카메라)을 적용, WorldPos 를 알아낸다.
    float3 vWorld = mul(float4(vViewPos.xyz, 1.f), g_matViewInv).xyz;
    
    // 광원 시야 기준 Projection 좌표계로 변환한다.
    float4 vLightProjPos = mul(float4(vWorld, 1.f), g_mat_1);
    
    float2 vShadowMapUV = float2(vLightProjPos.x * 0.5f + 0.5f, 1.f - (vLightProjPos.y * 0.5f + 0.5f));    
    
    if (0.f <= vShadowMapUV.x && vShadowMapUV.x <= 1.f
        || 0.f <= vShadowMapUV.y && vShadowMapUV.y <= 1.f)
    {
        float fDepth = g_tex_2.Sample(g_sam_0, vShadowMapUV).x;
        
        // Magic Number
        if (fDepth + 0.001f < vLightProjPos.z / vLightProjPos.w)
        {
            fShadowPow = 0.8f;
        }
    }
        
    output.vDiffuse = LightColor.vColor * (1.f - fShadowPow) + LightColor.vAmbient;
    output.vSpecular = LightColor.vSpecular * (1.f - fShadowPow);
    
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    return output;
}



// ========================
// Point Light Shader
// MRT      : LIGHT
// Mesh     : SphereMesh
// DS_TYPE  : NO_TEST_NO_WIRTE
// BS_TYPE  : ONE_ONE , 여러개의 빛이 누적될 수 있게

// Parameter
// g_int_0 : Light Idex
// g_mat_0 : ViewInv * WorldInv
// g_tex_0 : PositionTargetTex
// g_tex_1 : NormalTargetTex
// ========================
VS_OUT VS_PointLight(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
    output.vUV = _in.vUV;
    
    return output;
}

PS_OUT PS_PointLight(VS_OUT _in)
{
    PS_OUT output = (PS_OUT) 0.f;
    
    // 호출된 픽셀의 위치를 UV 값으로 환산
    float2 vScreenUV = _in.vPosition.xy / g_RenderResolution;
        
    // PositionTarget 에서 현재 호출된 픽셀쉐이더랑 동일한 지점에 접근해서 좌표값을 확인
    float4 vViewPos = g_tex_0.Sample(g_sam_0, vScreenUV);        
    
    // Deferred 단계에서 그려진게 없다면 빛을 줄 수 없다.
    if (-1.f == vViewPos.w)
    {       
        discard;
    }
                
    // Sphere 볼륨메쉬의 로컬 공간으로 데려간다.
    float3 vLocal = mul(float4(vViewPos.xyz, 1.f), g_mat_0).xyz;
    
    // 로컬공간에서 구(Sphere) 내부에 있는지 체크한다.
    if (0.5f < length(vLocal))
    {      
        discard;
    }
    
    // 해당 지점의 Normal 값을 가져온다.
    float3 vViewNormal = normalize(g_tex_1.Sample(g_sam_0, vScreenUV).xyz);
       
    // 해당 지점이 받을 빛의 세기를 구한다.
    tLightColor LightColor = (tLightColor) 0.f;
    CalculateLight3D(g_int_0, vViewPos.xyz, vViewNormal, LightColor);
        
    output.vDiffuse = LightColor.vColor + LightColor.vAmbient;
    output.vSpecular = LightColor.vSpecular;
    output.vDiffuse.a = 1.f;
    output.vSpecular.a = 1.f;
    
    
    return output;
}





#endif