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
            
    // ���� �ȿ� ����ִ� Normal ������ ����� �̵���Ų��.
    // ���⺤�ʹ� ��������� 4��(�̵�����) �� ������� �ʾƾ� �ϱ� ������ 
    // ������ǥ�� 0 ���� �����ؼ� ����� �����Ų��.
    
    // ���� ������ PixelShader ����, ViewSpace �󿡼� ����
    // ���� ���̴������� ������ ViewSpace �󿡼��� Normal �� Position ���� PixelShader �� ����
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
           
    
    // ��ü ����
    float4 ObjectColor = float4(0.7f, 0.7f, 0.7f, 1.f);
    
    // ��� �ؽ��İ� ���ε� �Ǿ��ִٸ�, �ؽ����� ������ ����Ѵ�.
    if (g_btex_0)
    {
        ObjectColor = g_tex_0.Sample(g_sam_0, _in.vUV);
    }
   
    float3 vViewNormal = _in.vViewNormal;
    
    // �븻 �ؽ��İ� ���ε� �Ǿ��ִٸ�, �븻������ �����Ѵ�.
    if (g_btex_1)
    {
        // ������ ������ 0~1 ������, ����� ���� ���⺤�͸� ���ϱ� ������ ���� �ǵ��� ������ �ٲٱ� ���ؼ�
        // ���� 0 ~ 1 ������ -1.f ~ 1.f �� �����Ѵ�.
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
        
    // ���� ���� == ��ü �� x (������ �� x ǥ���� ���� ����)
    //           + ��ü �� x (ȯ�汤 ����)
    //           + (���� �� x ���� �ݻ籤 ���Һ��� x �ݻ缼��(ī�޶�� �ݻ纤�Ͱ� ���� ���ֺ��� ����))
    vOutColor.xyz = ObjectColor.xyz * LightColor.vColor.rgb
                    + ObjectColor.xyz * LightColor.vAmbient.rgb
                    + LightColor.vSpecular.rgb;
    
    return vOutColor;
}


// 3D �� �Ѿ���鼭 CULL_BACK ��带 ����Ѵ�.
// �ð�������� �ε��� ���ټ����� ������ ���� �����̶�� ����
// �ݴ� ���⿡�� ������ Culling �� �Ѵ�.







#endif