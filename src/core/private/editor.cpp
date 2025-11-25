#include "cameraComponent.hpp"
#include "folderPath.hpp"//Camera Compoenent가 아래로 올 때 빌드 오류
#include "glad/glad.h"
#include "editor.hpp"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include "context.hpp"
#include "world.hpp"
#include "renderer.hpp"
#include "material.hpp"
#include "texture.hpp"
#include "resourceManager.hpp"
#include "meshComponent.hpp"
#include "lightComponent.hpp"

Editor::~Editor()
{
    ImGui_ImplOpenGL3_DestroyDeviceObjects();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void Editor::Init()
{
    GLFWwindow* window = glfwGetCurrentContext();
    IMGUI_CHECKVERSION();
    auto imguiContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(imguiContext);
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    #ifdef _WIN32
        ImGui_ImplOpenGL3_Init("#version 430");
    #elif __APPLE__
        ImGui_ImplOpenGL3_Init("#version 410");
    #endif
    ImGui_ImplOpenGL3_CreateDeviceObjects();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard |
                     ImGuiConfigFlags_DockingEnable |
                     ImGuiConfigFlags_ViewportsEnable;
    //nanumSquare font
    const std::string fontPath_EB = GetFullPath("/fonts/NanumSquareEB.ttf");
    const std::string fontPath_B = GetFullPath("/fonts/NanumSquareB.ttf");
    const std::string fontPath_L = GetFullPath("/fonts/NanumSquareL.ttf");
    const std::string fontPath_R = GetFullPath("/fonts/NanumSquareR.ttf");
    const std::string fontPath_acEB = GetFullPath("/fonts/NanumSquare_acEB.ttf");
    const std::string fontPath_acB = GetFullPath("/fonts/NanumSquare_acB.ttf");
    const std::string fontPath_acL = GetFullPath("/fonts/NanumSquare_acL.ttf");
    const std::string fontPath_acR = GetFullPath("/fonts/NanumSquare_acR.ttf");
    nanumSquare.size = 16.0f;
    nanumSquare.extraBold = io.Fonts->AddFontFromFileTTF(fontPath_EB.c_str(), nanumSquare.size, NULL, io.Fonts->GetGlyphRangesKorean());
    nanumSquare.bold = io.Fonts->AddFontFromFileTTF(fontPath_B.c_str(), nanumSquare.size, NULL, io.Fonts->GetGlyphRangesKorean());
    nanumSquare.light = io.Fonts->AddFontFromFileTTF(fontPath_L.c_str(), nanumSquare.size, NULL, io.Fonts->GetGlyphRangesKorean());
    nanumSquare.reqular = io.Fonts->AddFontFromFileTTF(fontPath_R.c_str(), nanumSquare.size, NULL, io.Fonts->GetGlyphRangesKorean());
    nanumSquare.extraBold_ac = io.Fonts->AddFontFromFileTTF(fontPath_acEB.c_str(), nanumSquare.size, NULL, io.Fonts->GetGlyphRangesKorean());
    nanumSquare.bold_ac = io.Fonts->AddFontFromFileTTF(fontPath_acB.c_str(), nanumSquare.size, NULL, io.Fonts->GetGlyphRangesKorean());
    nanumSquare.light_ac = io.Fonts->AddFontFromFileTTF(fontPath_acL.c_str(), nanumSquare.size, NULL, io.Fonts->GetGlyphRangesKorean());
    nanumSquare.reqular_ac = io.Fonts->AddFontFromFileTTF(fontPath_acR.c_str(), nanumSquare.size, NULL, io.Fonts->GetGlyphRangesKorean());
    io.Fonts->Build();
    if(nanumSquare.bold == NULL)
        io.Fonts->AddFontDefault();
    else
        io.FontDefault = nanumSquare.bold;
    //style
    SetupStyle();
}

void Editor::Update()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    CreateLayout();
    //ImGui::ShowDemoWindow();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* window = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(window);
    }
}

void Editor::SetupStyle()
{
	// Unreal style by dev0-1 from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();
	
	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.6f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 9.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 4.0f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
	
	style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.49803922f, 0.49803922f, 0.49803922f, 1.0f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.05882353f, 0.05882353f, 0.05882353f, 0.94f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(1.0f, 1.0f, 1.0f, 0.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.078431375f, 0.078431375f, 0.078431375f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.42745098f, 0.42745098f, 0.49803922f, 0.5f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.20784314f, 0.21960784f, 0.54f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.4f, 0.4f, 0.4f, 0.4f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.1764706f, 0.1764706f, 0.1764706f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.039215688f, 0.039215688f, 0.039215688f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.28627452f, 0.28627452f, 0.28627452f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.0f, 0.0f, 0.0f, 0.51f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.13725491f, 0.13725491f, 0.13725491f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.019607844f, 0.019607844f, 0.019607844f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.30980393f, 0.30980393f, 0.30980393f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40784314f, 0.40784314f, 0.40784314f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.50980395f, 0.50980395f, 0.50980395f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.9372549f, 0.9372549f, 0.9372549f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.50980395f, 0.50980395f, 0.50980395f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.85882354f, 0.85882354f, 0.85882354f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.4392157f, 0.4392157f, 0.4392157f, 0.4f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.45882353f, 0.46666667f, 0.47843137f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.41960785f, 0.41960785f, 0.41960785f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.69803923f, 0.69803923f, 0.69803923f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.69803923f, 0.69803923f, 0.69803923f, 0.8f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.47843137f, 0.49803922f, 0.5176471f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.42745098f, 0.42745098f, 0.49803922f, 0.5f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.7176471f, 0.7176471f, 0.7176471f, 0.78f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.50980395f, 0.50980395f, 0.50980395f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.9098039f, 0.9098039f, 0.9098039f, 0.25f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.80784315f, 0.80784315f, 0.80784315f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.45882353f, 0.45882353f, 0.45882353f, 0.95f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.1764706f, 0.34901962f, 0.5764706f, 0.862f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.25882354f, 0.5882353f, 0.9764706f, 0.8f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.19607843f, 0.40784314f, 0.6784314f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.06666667f, 0.101960786f, 0.14509805f, 0.9724f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13333334f, 0.25882354f, 0.42352942f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.60784316f, 0.60784316f, 0.60784316f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.42745098f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.7294118f, 0.6f, 0.14901961f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.1882353f, 0.1882353f, 0.2f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.30980393f, 0.30980393f, 0.34901962f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.22745098f, 0.22745098f, 0.24705882f, 1.0f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.06f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.8666667f, 0.8666667f, 0.8666667f, 0.35f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.9f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.6f, 0.6f, 0.6f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.7f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.2f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.8f, 0.8f, 0.8f, 0.35f);
}

void Editor::CreateLayout()
{
    //CreateMainBar
    CreateMainMenuBar();
    CreateMainToolBar();
    //CreateDockSpace
    CreateDockSpace();
    //Create Panel
    CreateSceneLayout();
    CreateRightPanel();
}

void Editor::CreateMainMenuBar()
{
    if(ImGui::BeginMainMenuBar())
    {
        ImGui::Separator();
        ImGui::SameLine();
        if(ImGui::BeginMenu("파일"))
        {
            if(ImGui::MenuItem("종료", "Alt + F4"))
            {

            }
            ImGui::EndMenu();
        }
        ImGui::SameLine(.0f,20.0f);
        ImGui::SetNextWindowSize(ImVec2(200, 0));
        if(ImGui::BeginMenu("보기"))
        {
            /*
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::PushFont(nanumSquare.reqular, 13.0f);
            ImGui::SeparatorText("Display");
            ImGui::PopFont();
            ImGui::PopStyleColor();

            ImGui::SetCursorPosX(20.0f);
            ImGui::RadioButton("라이팅포함", &selectedDisplay, 0);
            ImGui::SameLine(160.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::Text("1");
            ImGui::PopStyleColor();

            ImGui::SetCursorPosX(20.0f);
            ImGui::RadioButton("언릿", &selectedDisplay, 1);
            ImGui::SameLine(160.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::Text("2");
            ImGui::PopStyleColor();

            ImGui::SetCursorPosX(20.0f);
            ImGui::RadioButton("와이어프레임", &selectedDisplay, 2);
            ImGui::SameLine(160.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::Text("3");
            ImGui::PopStyleColor();

            ImGui::SetCursorPosX(20.0f);
            ImGui::RadioButton("라이팅", &selectedDisplay, 3);
            ImGui::SameLine(160.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::Text("4");
            ImGui::PopStyleColor();
            */
            //Shape
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::PushFont(nanumSquare.reqular, 13.0f);
            ImGui::SeparatorText("Shape");
            ImGui::PopFont();
            ImGui::PopStyleColor();

            ImGui::SetCursorPosX(20.0f);
            if(ImGui::RadioButton("Sphere", &selectedShape, 0))
            {
                MeshComponent* meshComponent = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>();
                IMesh* mesh = Context::GetContext()->resourceManager->FindMesh("sphere");
                meshComponent->SetMesh(mesh);
                meshComponent->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
                meshComponent->SetRotation(glm::vec3(90.0f, -90.0f, -90.0f));
                meshComponent->SetRotation(glm::vec3(0.0f));
                meshComponent->SetScale(glm::vec3(2.5f));
            }
            ImGui::SameLine(120.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::Text("Ctrl + 1");
            ImGui::PopStyleColor();
            
            ImGui::SetCursorPosX(20.0f);
            if(ImGui::RadioButton("Cube", &selectedShape, 1))
            {
                MeshComponent* meshComponent = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>();
                IMesh* mesh = Context::GetContext()->resourceManager->FindMesh("cube");
                meshComponent->SetMesh(mesh);
                meshComponent->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
                meshComponent->SetRotation(glm::vec3(0.0f));
                meshComponent->SetScale(glm::vec3(2.0f));
            }
            ImGui::SameLine(120.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::Text("Ctrl + 2");
            ImGui::PopStyleColor();

            ImGui::SetCursorPosX(20.0f);
            if(ImGui::RadioButton("Cylinder", &selectedShape, 2))
            {
                MeshComponent* meshComponent = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>();
                IMesh* mesh = Context::GetContext()->resourceManager->FindMesh("cylinder");
                meshComponent->SetMesh(mesh);
                meshComponent->SetPosition(glm::vec3(0.0f, 2.0f, 0.0f));
                meshComponent->SetRotation(glm::vec3(0.0f));
                meshComponent->SetScale(glm::vec3(1.0f, 2.0f, 1.0f));
            }
            ImGui::SameLine(120.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::Text("Ctrl + 3");
            ImGui::PopStyleColor();

            ImGui::SetCursorPosX(20.0f);
            if(ImGui::RadioButton("Plane", &selectedShape, 3))
            {
                MeshComponent* meshComponent = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>();
                IMesh* mesh = Context::GetContext()->resourceManager->FindMesh("plane");
                meshComponent->SetMesh(mesh);
                meshComponent->SetPosition(glm::vec3(0.0f, 2.5f, 0.0f));
                meshComponent->SetRotation(glm::vec3(90.0f, 0.0f, 0.0f));
                meshComponent->SetScale(glm::vec3(0.5f));
            }
            ImGui::SameLine(120.0f, 0.0f);
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
            ImGui::Text("Ctrl + 4");
            ImGui::PopStyleColor();

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void Editor::CreateMainToolBar()
{

}

void Editor::CreateDockSpace()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGuiWindowFlags winFlags = ImGuiWindowFlags_NoTitleBar |
                                ImGuiWindowFlags_NoDecoration |
                                ImGuiWindowFlags_NoCollapse |
                                ImGuiWindowFlags_NoMove |
                                ImGuiWindowFlags_NoBringToFrontOnFocus |
                                ImGuiWindowFlags_NoNavFocus |
                                ImGuiWindowFlags_NoDocking;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("MainDockSpace", nullptr, winFlags);
    ImGui::PopStyleVar();
    ImGuiID dockspaceID = ImGui::GetID("MainDockSpace");
    ImGui::DockSpace(dockspaceID, ImVec2(0.0f,0.0f), ImGuiDockNodeFlags_None);
    if(!bInit)
    {
        bInit = true;
        ImGui::DockBuilderRemoveNode(dockspaceID);
        ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace);
        ImGui::DockBuilderSetNodeSize(dockspaceID, viewport->WorkSize);
        ImGuiID dockSceneID = dockspaceID;
        ImGuiID dockInspectorID;
        ImGui::DockBuilderSplitNode(dockSceneID, ImGuiDir_Right, 0.2f, &dockInspectorID, &dockSceneID);
        ImGui::DockBuilderDockWindow("Scene", dockSceneID);
        ImGui::DockBuilderDockWindow("Inspector", dockInspectorID);
        ImGuiDockNode* sceneNode = ImGui::DockBuilderGetNode(dockSceneID);
        ImGuiDockNode* inspectorNode = ImGui::DockBuilderGetNode(dockInspectorID);
        if(sceneNode != nullptr)
        {
            sceneNode->LocalFlags |= ImGuiDockNodeFlags_NoDocking |
                                     ImGuiDockNodeFlags_NoTabBar |
                                     ImGuiDockNodeFlags_NoSplit;
        }
        if(inspectorNode != nullptr)
        {
            inspectorNode->LocalFlags |= ImGuiDockNodeFlags_NoDocking |
                                         ImGuiDockNodeFlags_NoTabBar |
                                         ImGuiDockNodeFlags_NoSplit;
        }
        ImGui::DockBuilderFinish(dockspaceID);
    }
    ImGui::End();
}

void Editor::CreateSceneLayout()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Scene");
    ImGui::PopStyleVar();
    ImVec2 sceneSize = ImGui::GetContentRegionAvail();
    if (sceneSize.x>0 && sceneSize.y>0)
        Context::GetContext()->renderer->ResizeBuffer((int)sceneSize.x, (int)sceneSize.y);
    ImGui::Image((void*)(intptr_t) Context::GetContext()->renderer->GetColorBuffer(), sceneSize, ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    //Overay Gbuffer
    if(bG_buffer)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 scenePos = viewport->WorkPos;
        scenePos.x += 5.0f;
        scenePos.y += 5.0f;

        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::SetNextWindowPos(scenePos);
        ImGui::SetNextWindowSize(ImVec2(sceneSize.x-20.f, sceneSize.y-20.f));
        ImGui::Begin("Overay", nullptr,
                ImGuiWindowFlags_NoTitleBar  |
                ImGuiWindowFlags_NoInputs    |
                ImGuiWindowFlags_NoMove      |
                ImGuiWindowFlags_AlwaysAutoResize|
                ImGuiWindowFlags_NoSavedSettings|
                ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoDecoration);
        if(ImGui::BeginTable("framebufferTable", 6, ImGuiTableFlags_RowBg|ImGuiTableFlags_Resizable|ImGuiTableFlags_Borders, ImVec2(1180, 0)))
        {
            ImGui::TableSetupColumn("Shadow Map", ImGuiTableColumnFlags_WidthFixed, 130.0f);
            ImGui::TableSetupColumn("Position", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Diffuse", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Roughness", ImGuiTableColumnFlags_WidthFixed, 200.0f);
            ImGui::TableSetupColumn("Occlusion", ImGuiTableColumnFlags_WidthFixed, 200.0f);

            ImGui::TableHeadersRow();
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0);
            ImGui::Image(Context::GetContext()->renderer->GetShadowBuffer(), ImVec2(128, 128), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TableSetColumnIndex(1);
            ImGui::Image(Context::GetContext()->renderer->GetGBufferPosition(), ImVec2(200, 128), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TableSetColumnIndex(2);
            ImGui::Image(Context::GetContext()->renderer->GetGBufferNormal(), ImVec2(200, 128), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TableSetColumnIndex(3);
            ImGui::Image(Context::GetContext()->renderer->GetGBufferDiffuse(), ImVec2(200, 128), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TableSetColumnIndex(4);
            ImGui::Image(Context::GetContext()->renderer->GetGBufferRoughness(), ImVec2(200, 128), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::TableSetColumnIndex(5);
            ImGui::Image(Context::GetContext()->renderer->GetGBufferOcclusion(), ImVec2(200, 128), ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndTable();
        }
        //ImGui::GetForegroundDrawList()->AddImage(Context::GetContext()->renderer->GetShadowBuffer(), scenePos , ImVec2(256+scenePos.x, 256+scenePos.y), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::End();
    }
}

void Editor::CreateRightPanel()
{
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(30, 30, 30, 255));
    ImGui::Begin("Inspector", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoCollapse);
    ImGui::PopStyleColor();
    if(ImGui::BeginTabBar("RightTabs"))
    {
        if(ImGui::BeginTabItem("디테일"))
        {
            //ImGuiViewport* viewport = ImGui::GetMainViewport();
            //ImVec2 pos = viewport->WorkPos;
            //pos.y += 5.0f;
            //ImGui::SetCursorPosY(pos.y);
            if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                ImGui::PushFont(nanumSquare.reqular, 13.0f);
                ImGui::SeparatorText("transform");
                ImGui::PopFont();
                ImGui::PopStyleColor();

                LightComponent* lightComponent = Context::GetContext()->world->GetCurrentLight()->GetComponent<LightComponent>();
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Rotation");
                ImGui::SameLine(120.0f, 0.0f);
                ImGui::DragFloat2("##rotationfoat3", lightComponent->GetRotation(), 1.0f, -360.0f, 360.0f);
                ImGui::Separator();

                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                ImGui::PushFont(nanumSquare.reqular, 13.0f);
                ImGui::SeparatorText("propertices");
                ImGui::PopFont();
                ImGui::PopStyleColor();

                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Intensity");
                ImGui::SameLine(120.0f, 0.0f);
                ImGui::DragFloat("##intensityfloat", lightComponent->GetIntensity(), 0.01f, 0.0f, 150.0f);
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Color");
                ImGui::SameLine(120.0f, 0.0f);
                ImGui::ColorEdit3("##colorpick", lightComponent->GetColor());
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Ambient");
                ImGui::SameLine(120.0f, 0.0f);
                ImGui::ColorEdit3("##ambientcolorpick", lightComponent->GetAmbient());
                ImGui::Separator();
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Skybox");
                ImGui::SameLine(120.0f, 0.0f);
                ImGui::ImageButton("hdriTexButton", lightComponent->GetSkyboxTexture()->GetID(), ImVec2(64*3,64*2));
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("Renderer", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                ImGui::PushFont(nanumSquare.reqular, 13.0f);
                ImGui::SeparatorText("propertices");
                ImGui::PopFont();
                ImGui::PopStyleColor();

                OpenGLRenderer* renderer = static_cast<OpenGLRenderer*>(Context::GetContext()->renderer);
                IMaterial* material = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>()->GetMaterial();
                MaterialParameter* parameter = static_cast<OpenGLMaterial*>(material)->GetParameter();
                ImGui::Checkbox("##Stencil", renderer->GetStencil());
                ImGui::SameLine();
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Stencil");
                ImGui::SameLine(120.0f, 0.0f);
                if(*renderer->GetStencil())
                {
                    ImGui::SliderFloat("##stencilOutline", &parameter->stencilOutline, 0.01f, 1.0f);
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Color");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::ColorEdit3("##stencilColor", glm::value_ptr(parameter->stencilColor));
                }
                else
                {
                    ImGui::BeginDisabled();
                    ImGui::SliderFloat("##stencilOutline", &parameter->stencilOutline, 0.01f, 1.0f);
                    ImGui::EndDisabled();
                }
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                ImGui::PushFont(nanumSquare.reqular, 13.0f);
                ImGui::SeparatorText("post-processing");
                ImGui::PopFont();
                ImGui::PopStyleColor();

                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Effect");
                ImGui::SameLine(120.0f, 0.0f);
                const char* postItems[] = { "None", "OnMinus", "Gray", "Sharpen", "Blur", "Edge"};
                static int post_item_selected_idx = 0;
                const char* post_combo_preview_value = postItems[post_item_selected_idx];
                if(ImGui::BeginCombo("##postCombo", post_combo_preview_value))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(postItems); n++)
                    {
                        const bool is_selected = (post_item_selected_idx == n);
                        if (ImGui::Selectable(postItems[n], is_selected))
                        {
                            renderer->SetEffectType((PostEffectType)n);
                            post_item_selected_idx = n;
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                //
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                ImGui::PushFont(nanumSquare.reqular, 13.0f);
                ImGui::SeparatorText("ToneMapping");
                ImGui::PopFont();
                ImGui::PopStyleColor();
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Mode");
                ImGui::SameLine(120.0f, 0.0f);
                const char* toneItems[] = { "Linear", "Reinhard", "Filmic", "ACES", "Agx"};
                static int tone_item_selected_idx = 0;
                const char* tone_combo_preview_value = toneItems[tone_item_selected_idx];
                if(ImGui::BeginCombo("##toneCombo", tone_combo_preview_value))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(toneItems); n++)
                    {
                        const bool is_selected = (tone_item_selected_idx == n);
                        if (ImGui::Selectable(toneItems[n], is_selected))
                        {
                            tone_item_selected_idx = n;
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Exposure");
                ImGui::SameLine(120.0f, 0.0f);
                ImGui::DragFloat("##exposure", renderer->GetExposure(), 0.01f, 0.0, 10.0f);
                ImGui::Separator();
                /*
                ImGui::SetCursorPosX(40.0f);
                if (ImGui::CollapsingHeader("Shadow Map"))
                {
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                    ImGui::PushFont(nanumSquare.reqular, 13.0f);
                    ImGui::SeparatorText("shadowMap");
                    ImGui::PopFont();
                    ImGui::PopStyleColor();

                    ImGui::SetCursorPosX(80.0f);
                    ImGui::Image(renderer->GetShadowBuffer(), ImVec2(256, 256), ImVec2(0, 1), ImVec2(1, 0));
                }*/
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Rendering");
                ImGui::SameLine(120.0f, 0.0f);
                ImGui::RadioButton("Forward", renderer->GetDeferred(), 0);
                ImGui::SameLine();
                ImGui::RadioButton("Deferred", renderer->GetDeferred(), 1);
                if (*renderer->GetDeferred()>0)
                {
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Frame\nBuffer");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::Checkbox("##bufferCheck", &bG_buffer);
                }
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                ImGui::PushFont(nanumSquare.reqular, 13.0f);
                ImGui::SeparatorText("propertices");
                ImGui::PopFont();
                ImGui::PopStyleColor();

                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Shader");
                ImGui::SameLine(120.0f, 0.0f);
                const char* shaderItems[] = { "Blinn", "Phong", "PBR"};
                static int shader_item_selected_idx = 0;
                const char* shader_combo_preview_value = shaderItems[shader_item_selected_idx];
                if(ImGui::BeginCombo("##shaderCombo", shader_combo_preview_value))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(shaderItems); n++)
                    {
                        const bool is_selected = (shader_item_selected_idx == n);
                        if (ImGui::Selectable(shaderItems[n], is_selected))
                        {
                            IMaterial* material = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>()->GetMaterial();
                            MaterialParameter* parameter = static_cast<OpenGLMaterial*>(material)->GetParameter();
                            parameter->shader = n;
                            shader_item_selected_idx = n;
                        }
                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Type");
                ImGui::SameLine(120.0f, 0.0f);
                const char* typeItems[] = { "Plastic", "Transparent"};
                static int type_item_selected_idx = 0;
                const char* type_combo_preview_value = typeItems[type_item_selected_idx];
                if(ImGui::BeginCombo("##typeCombo", type_combo_preview_value))
                {
                    for (int n = 0; n < IM_ARRAYSIZE(typeItems); n++)
                    {
                        const bool is_selected = (type_item_selected_idx == n);
                        if (ImGui::Selectable(typeItems[n], is_selected))
                        {
                            IMaterial* material = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>()->GetMaterial();
                            MaterialParameter* parameter = static_cast<OpenGLMaterial*>(material)->GetParameter();
                            parameter->type = (MaterialType)n;
                            type_item_selected_idx = n;
                        }

                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
                ImGui::Separator();
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                ImGui::PushFont(nanumSquare.reqular, 13.0f);
                ImGui::SeparatorText("textures");
                ImGui::PopFont();
                ImGui::PopStyleColor();
                //textures
                IMaterial* material = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>()->GetMaterial();
                MaterialParameter* parameter = static_cast<OpenGLMaterial*>(material)->GetParameter();
                ImGui::SetCursorPosX(40.0f);
                if(ImGui::BeginTable("textureTable", 4, ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders))
                {
                    ImGui::TableSetupColumn("Diffuse", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Roughness", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Metallic", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableSetupColumn("Normal", ImGuiTableColumnFlags_WidthFixed);
                    ImGui::TableHeadersRow();
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (parameter->diffuseTexture != nullptr)
                        ImGui::ImageButton("diffTexButton", parameter->diffuseTexture->GetID(), ImVec2(64,64));
                    else
                        ImGui::ImageButton("diffTexButton", nullptr, ImVec2(64,64));
                    ImGui::TableSetColumnIndex(1);
                    if (parameter->roughnessTextrue != nullptr)
                        ImGui::ImageButton("rougTexButton", parameter->roughnessTextrue->GetID(), ImVec2(64,64));
                    else
                        ImGui::ImageButton("rougTexButton", nullptr, ImVec2(64,64));
                    ImGui::TableSetColumnIndex(2);
                    if (parameter->metallicTexture != nullptr)
                        ImGui::ImageButton("metalTexButton", parameter->metallicTexture->GetID(), ImVec2(64,64));
                    else
                        ImGui::ImageButton("metalTexButton", nullptr, ImVec2(64,64));
                    ImGui::TableSetColumnIndex(3);
                    if (parameter->normalTexture != nullptr)
                        ImGui::ImageButton("nolTexButton", parameter->normalTexture->GetID(), ImVec2(64,64));
                    else
                        ImGui::ImageButton("nolTexButton", nullptr, ImVec2(64,64));
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    if (parameter->aoTexture != nullptr)
                        ImGui::ImageButton("aoTexButton", parameter->aoTexture->GetID(), ImVec2(64,64));
                    else
                        ImGui::ImageButton("aoTexButton", nullptr, ImVec2(64,64));
                    ImGui::Text("Occulsion");
                    ImGui::TableSetColumnIndex(1);
                    if (parameter->dispTexture != nullptr)
                        ImGui::ImageButton("dispTexButton", parameter->dispTexture->GetID(), ImVec2(64,64));
                    else
                        ImGui::ImageButton("dispTexButton", nullptr, ImVec2(64,64));
                    ImGui::Text("Displacement");
                    ImGui::EndTable();
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("HeightScale");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::DragFloat("##heightScale", &parameter->heightScale, 0.001f, -1.0f, 1.0f);
                }
                //parameter
                if(shader_item_selected_idx != 2) //blinn phong
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                    ImGui::PushFont(nanumSquare.reqular, 13.0f);
                    ImGui::SeparatorText("parameters");
                    ImGui::PopFont();
                    ImGui::PopStyleColor();

                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Color");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::ColorEdit4("##diffusecolorpick", glm::value_ptr(parameter->diffuseColor));
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Specular");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::ColorEdit3("##specularcolorpick", glm::value_ptr(parameter->specularColor));
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Shininess");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::DragFloat("##specularfloat", &parameter->specularShininess, 1.0f, 1.0f, 128.0f);
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Refraction");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::DragFloat("##refractionfloat", &parameter->refraction, 0.01f, 0.0f, 1.0f);
                }else if(shader_item_selected_idx == 2)//PBR parameter
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                    ImGui::PushFont(nanumSquare.reqular, 13.0f);
                    ImGui::SeparatorText("PBR");
                    ImGui::PopFont();
                    ImGui::PopStyleColor();                
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Roughness");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::DragFloat("##roughnessfloat", &parameter->roughnessFactor, 0.01f, 0.0f, 1.0f);
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Specular");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::DragFloat("##specualrfloat", &parameter->shininess, 0.01f, 0.0f, 1.0f);
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Metallic");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::DragFloat("##metallicfloat", &parameter->metallicFactor, 0.01f, 0.0f, 1.0f);
                    ImGui::SetCursorPosX(40.0f);
                    ImGui::Text("Normal");
                    ImGui::SameLine(120.0f, 0.0f);
                    ImGui::DragFloat("##normalfloat", &parameter->normalFactor, 0.01f, 0.0f, 1.0f);
                }

                MeshComponent* meshComponent = Context::GetContext()->world->GetSelectedActor()->GetComponent<MeshComponent>();
                ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 120));
                ImGui::PushFont(nanumSquare.reqular, 13.0f);
                ImGui::SeparatorText("transform");
                ImGui::PopFont();
                ImGui::PopStyleColor();

                ImGui::SetCursorPosX(40.0f);
                ImGui::Text("Rotation");
                ImGui::SameLine(120.0f, 0.0f);
                
                ImGui::DragFloat3("##meshrotationfoat3", meshComponent->GetRotation(), 1.0f, -360.0f, 360.0f);
                ImGui::Separator();
            }
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();
}

void Editor::CreateStatus()
{
}