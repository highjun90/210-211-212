#ifndef _FUNC
#define _FUNC

#include "struct.fx"
#include "value.fx"


void CalLight2D(float3 _WorldPos, int _LightIdx, inout tLightColor _output)
{
    // 빛을 적용시킬 광원의 정보
    tLightInfo info = g_Light2D[_LightIdx];    
    
    // Directional Light
    if (0 == info.LightType)
    {
        _output.vAmbient += info.Color.vAmbient;
    }
    
    // Point Light
    else if (1 == info.LightType)
    {
        float fAttenu = 1.f;
        
        float fDist = distance(info.vWorldPos.xy, _WorldPos.xy);
        if (fDist < info.fRadius)
        {
            if (g_int_0)
            {
                float fTheta = (fDist / info.fRadius) * (PI / 2.f);
                fAttenu = saturate(cos(fTheta));
            }
            else
            {
                fAttenu = saturate(1.f - fDist / g_Light2D[0].fRadius);
            }
            
            _output.vColor += info.Color.vColor * fAttenu;
        }        
    }
    
    // Spot Light
    else
    {
        // Point Light 거의 유사
        // 내적을 활용, 각도 체크
        // 간단한 영상 찍어서 올리기
        // 광원을 회전시키기
    }    
}


void CalculateLight3D(int _LightIdx, float3 _vViewPos, float3 _vViewNormal, inout tLightColor _LightColor)
{
    // 광원의 정보를 확인
    tLightInfo Light = g_Light3D[_LightIdx];
           
    // 광원이 물체를 향하는 방향벡터
    float3 vViewLightDir = (float3) 0.f; 
    
    float fDistanceRatio = 1.f;
    
    // Directional Light
    if(0 == Light.LightType)
    {
        // 광원 연산이 ViewSpace 에서 진행되기로 했기 때문에,
        // 광원이 진입하는 방향도 View 공간 기준으로 변경함
        vViewLightDir = normalize(mul(float4(Light.vWorldDir, 0.f), g_matView).xyz);
    }
    
    // Point Light
    else if( 1 == Light.LightType)
    {
        float3 vLightViewPos = mul(float4(Light.vWorldPos, 1.f), g_matView).xyz;
        vViewLightDir = _vViewPos - vLightViewPos;
        
        // 광원과 물체 사이의 거리
        float fDistance = length(vViewLightDir);
        vViewLightDir = normalize(vViewLightDir);
                
        // 광원 반경과 물체까지의 거리에 따른 빛의 세기
        //fDistanceRatio = saturate(1.f - (fDistance / Light.fRadius));
        fDistanceRatio = saturate(cos(fDistance / Light.fRadius * (PI / 2.f)));                
    }
    
    // Spot Light
    else
    {
        
    }   
   
    // ViewSpace 에서 광원의 방향과, 물체 표면의 법선를 이용해서 광원의 진입 세기(Diffuse) 를 구한다.
    float LightPow = saturate(dot(_vViewNormal, -vViewLightDir));
            
    // 빛이 표면에 진입해서 반사되는 방향을 구한다.
    float3 vReflect = vViewLightDir + 2 * dot(-vViewLightDir, _vViewNormal) * _vViewNormal;
    vReflect = normalize(vReflect);
    
    // 카메라가 물체를 향하는 방향
    float3 vEye = normalize(_vViewPos);
    
    // 시선벡터와 반사벡터 내적, 반사광의 세기
    float ReflectPow = saturate(dot(-vEye, vReflect));
    ReflectPow = pow(ReflectPow, 20.f);
    
    _LightColor.vColor += Light.Color.vColor * LightPow * fDistanceRatio;
    _LightColor.vAmbient += Light.Color.vAmbient;
    _LightColor.vSpecular += Light.Color.vColor * Light.Color.vSpecular * ReflectPow * fDistanceRatio;
}









// ======
// Random
// ======
static float GaussianFilter[5][5] =
{
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.0219f, 0.0983f, 0.1621f, 0.0983f, 0.0219f,
    0.0133f, 0.0596f, 0.0983f, 0.0596f, 0.0133f,
    0.003f, 0.0133f, 0.0219f, 0.0133f, 0.003f,
};

void GaussianSample(in Texture2D _NoiseTex, float2 _vResolution, float _NomalizedThreadID, out float3 _vOut)
{
    float2 vUV = float2(_NomalizedThreadID, 0.5f);
    
    vUV.x += g_time * 0.5f;
    
    // sin 그래프로 텍스쳐의 샘플링 위치 UV 를 계산
    vUV.y -= (sin((_NomalizedThreadID - (g_time /*그래프 우측 이동 속도*/)) * 2.f * 3.1415926535f * 10.f /*반복주기*/) / 2.f);
    
    if (1.f < vUV.x)
        vUV.x = frac(vUV.x);
    else if (vUV.x < 0.f)
        vUV.x = 1.f + frac(vUV.x);
    
    if (1.f < vUV.y)
        vUV.y = frac(vUV.y);
    else if (vUV.y < 0.f)
        vUV.y = 1.f + frac(vUV.y);
        
    int2 pixel = vUV * _vResolution;
    int2 offset = int2(-2, -2);
    float3 vOut = (float3) 0.f;
    
    for (int i = 0; i < 5; ++i)
    {
        for (int j = 0; j < 5; ++j)
        {
            vOut += _NoiseTex[pixel + offset + int2(j, i)].xyz * GaussianFilter[i][j];
        }
    }
    
    _vOut = vOut;
}




#endif