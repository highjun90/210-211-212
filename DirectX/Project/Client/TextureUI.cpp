#include "pch.h"
#include "TextureUI.h"

#include <Engine/CTexture.h>
#include <Engine/CFontMgr.h>

TextureUI::TextureUI()
	: AssetUI("Texture", "##Texture", ASSET_TYPE::TEXTURE)
{
}

TextureUI::~TextureUI()
{
}

void TextureUI::render_update()
{
	AssetUI::render_update();

    // 해당 텍스쳐 이미지 출력
    Ptr<CTexture> pTex = (CTexture*)GetAsset().Get();
    
    string strPath = string(pTex->GetRelativePath().begin(), pTex->GetRelativePath().end());

    ImGui::Text("Texture");
    ImGui::SameLine();
    ImGui::InputText("##TexName", (char*)strPath.c_str(), strPath.length(), ImGuiInputTextFlags_ReadOnly);   
    
  
    // 해상도 정보 출력
    float Width = pTex->GetWidth();
    float Height = pTex->GetHeight();

    ImGui::Text("Width "); 
    ImGui::SameLine(); 
    ImGui::InputFloat("##TextureWidth", &Width, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

    ImGui::Text("Height");
    ImGui::SameLine();
    ImGui::InputFloat("##TextureHeight", &Height, 0.f, 0.f, "%.3f", ImGuiInputTextFlags_ReadOnly);

    static bool use_text_color_for_tint = false;
    ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
    ImVec2 uv_max = ImVec2(1.0f, 1.0f);                 // Lower-right
    ImVec4 tint_col = use_text_color_for_tint ? ImGui::GetStyleColorVec4(ImGuiCol_Text) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f); // No tint
    ImVec4 border_col = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    ImVec2 vScreenPos = ImGui::GetCursorScreenPos();
    ImGui::Image(pTex->GetSRV().Get(), ImVec2(150, 150), uv_min, uv_max, tint_col, border_col);
}