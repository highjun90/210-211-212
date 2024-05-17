#ifndef _DEBUG
#define _DEBUG

#include "value.fx"

struct VS_IN
{
    float3 vPos : POSITION;
    float2 vUV : TEXCOORD;    
    float3 vNormal : NORMAL;
};

struct VS_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;    
    
    float3 vViewPos : POSITION;
    float3 vViewNormal : NORMAL;    
};

VS_OUT VS_DebugShape(VS_IN _in)
{
    VS_OUT output = (VS_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);    
    output.vUV = _in.vUV;
    
    // g_int_0 가 0 이 아니면, 현재 렌더링하는 메시가 Sphere 메시이다.
    if(g_int_0)
    {
        // Sphere 의 View 공간상에서의 정점의 좌표와 노말값을 픽셀쉐이더로 전달
        output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV).xyz;
        output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV).xyz);
    }
    
    return output;
}

float4 PS_DebugShape(VS_OUT _in) : SV_Target
{
    float4 vOutColor = (float4) 0.f;
      
    vOutColor = g_vec4_0;
    vOutColor.a = 1.f;
    
    // 렌더링 중인 메시가 SphereMesh 인 경우
    if (g_int_0)
    {
        // View 공간에서 카메라(원점) 가 픽셀(대상) 을 바라보는 시선 방향벡터는 물체의 좌표이다.
        float3 vEye = normalize(_in.vViewPos);
        
        // 시선벡터랑 픽셀의 ViewNormal 의 내적을 통해서 구의 내부인지 외각영역인지 구별할 수 있다.
        // 내적의 결과를 토대로, 시선벡터와 물체의 노말의이 이루는 각도가 정면은 0 도에 가까울 수록, 
        // 뒤로 들어간 면은 180 도에 가까울 수록 중앙이기 때문에, 둘다 체크하기 위해서 내적의 결과를 
        // absolute 로 절대값을 씌운다        
        vOutColor.a = 1.f - pow(saturate(abs(dot(-vEye, _in.vViewNormal))), 0.1f);        
    }
    
    return vOutColor;
}






#endif