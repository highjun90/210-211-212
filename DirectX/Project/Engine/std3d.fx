#ifndef _STD3D
#define _STD3D

#include "value.fx"
#include "func.fx"

struct VTX_IN
{
    float3 vPos : POSITION;
    float3 vUV  : TEXCOORD; 
    
    float3 vTangent : TANGENT;
    float3 vNormal : NORMAL;
    float3 vBinormal : BINORMAL;
};


struct VTX_OUT
{
    float4 vPosition : SV_Position;
    float2 vUV : TEXCOORD;
    float LightPow : FOG;
    
    float3 vViewPos : POSITION;
    float3 vViewTangent : TANGENT;
    float3 vViewNormal : NORMAL;  
    float3 vViewBinormal : BINORMAL;
};


VTX_OUT VS_Std3D(VTX_IN _in)
{
    VTX_OUT output = (VTX_OUT) 0.f;
    
    output.vPosition = mul(float4(_in.vPos, 1.f), g_matWVP);
            
    // 정점 안에 들어있는 Normal 방향을 월드로 이동시킨다.
    // 방향벡터는 상태행렬의 4행(이동정보) 에 영향받지 않아야 하기 때문에 
    // 동차좌표를 0 으로 설정해서 행렬을 적용시킨다.
    
    // 광원 연산은 PixelShader 에서, ViewSpace 상에서 진행
    // 정점 쉐이더에서는 정점의 ViewSpace 상에서의 Normal 과 Position 값을 PixelShader 로 전달
    output.vViewTangent = normalize(mul(float4(_in.vTangent, 0.f), g_matWV));
    output.vViewNormal = normalize(mul(float4(_in.vNormal, 0.f), g_matWV));
    output.vViewBinormal = normalize(mul(float4(_in.vBinormal, 0.f), g_matWV));
        
    output.vViewPos = mul(float4(_in.vPos, 1.f), g_matWV);
    output.vUV = _in.vUV;
    
    return output;
}


float4 PS_Std3D(VTX_OUT _in) : SV_Target
{
    float4 vOutColor = float4(0.f, 0.f, 0.f, 1.f);
           
    
    // 물체 색상
    float4 ObjectColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    
    // 출력 텍스쳐가 바인딩 되어있다면, 텍스쳐의 색상을 사용한다.
    if (g_btex_0)
    {
        ObjectColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
   
    float3 vViewNormal = _in.vViewNormal;
    
    // 노말 텍스쳐가 바인딩 되어있다면, 노말맵핑을 진행한다.
    if (g_btex_1)
    {
        // 색상의 범위는 0~1 이지만, 저장된 값은 방향벡터를 뜻하기 때문에 원래 의도한 값으로 바꾸기 위해서
        // 값의 0 ~ 1 범위를 -1.f ~ 1.f 로 변경한다.
        float3 vNormal = g_tex_1.Sample(g_sam_0, _in.vUV).rgb;
        vNormal = vNormal * 2.f - 1.f;
                        
        float3x3 matRot =
        {
            _in.vViewTangent,           
            -_in.vViewBinormal,
            _in.vViewNormal,
        };        
        
        vViewNormal = normalize(mul(vNormal.xyz, matRot));        
    }
    
    tLightColor LightColor = (tLightColor) 0.f;
    
    for (int i = 0; i < g_Light3DCount; ++i)
    {
        CalculateLight3D(i, _in.vViewPos, vViewNormal, LightColor);
    }
        
    // 최종 색상 == 물체 색 x (광원의 색 x 표면의 광원 세기)
    //           + 물체 색 x (환경광 세기)
    //           + (빛의 색 x 빛의 반사광 감소비율 x 반사세기(카메라랑 반사벡터가 서로 마주보는 정도))
    vOutColor.xyz = ObjectColor.xyz * LightColor.vColor.rgb
                    + ObjectColor.xyz * LightColor.vAmbient.rgb
                    + LightColor.vSpecular.rgb;
    
    return vOutColor;
}


// 3D 로 넘어오면서 CULL_BACK 모드를 사용한다.
// 시계방향으로 인덱스 접근순서가 설정된 면을 정면이라고 보고
// 반대 방향에서 볼때는 Culling 을 한다.







#endif