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
    
    // g_int_0 �� 0 �� �ƴϸ�, ���� �������ϴ� �޽ð� Sphere �޽��̴�.
    if(g_int_0)
    {
        // Sphere �� View �����󿡼��� ������ ��ǥ�� �븻���� �ȼ����̴��� ����
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
    
    // ������ ���� �޽ð� SphereMesh �� ���
    if (g_int_0)
    {
        // View �������� ī�޶�(����) �� �ȼ�(���) �� �ٶ󺸴� �ü� ���⺤�ʹ� ��ü�� ��ǥ�̴�.
        float3 vEye = normalize(_in.vViewPos);
        
        // �ü����Ͷ� �ȼ��� ViewNormal �� ������ ���ؼ� ���� �������� �ܰ��������� ������ �� �ִ�.
        // ������ ����� ����, �ü����Ϳ� ��ü�� �븻���� �̷�� ������ ������ 0 ���� ����� ����, 
        // �ڷ� �� ���� 180 ���� ����� ���� �߾��̱� ������, �Ѵ� üũ�ϱ� ���ؼ� ������ ����� 
        // absolute �� ���밪�� �����        
        vOutColor.a = 1.f - pow(saturate(abs(dot(-vEye, _in.vViewNormal))), 0.1f);        
    }
    
    return vOutColor;
}






#endif