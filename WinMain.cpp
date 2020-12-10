#include "stdafx.h"
#include "ViewportDX11Renderer.h"

namespace ed = ax::NodeEditor;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    PSTR lpCmdLine, INT nCmdShow)
{
    ImguiDX11WinRenderer contextRenderer((wchar_t*)L"Nodemat", hInstance);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable | ImGuiConfigFlags_ViewportsEnable | ImGuiConfigFlags_DpiEnableScaleViewports; //ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: THIS CURRENTLY DOESN'T WORK AS EXPECTED. DON'T USE IN USER APP!


    
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        style.FrameRounding = 0;
        style.WindowMenuButtonPosition = ImGuiDir_Right;
        style.ScrollbarRounding = 0;
        style.FrameBorderSize = 0;
    }
    
    contextRenderer.ImguiInit();
    
    float dpiScale = GetDpiForSystem()/96.0f;
    ViewportDX11Renderer vpRenderer(&contextRenderer, ImVec2(1024, 1024));
    
    io.Fonts->AddFontFromFileTTF("H:\\Cpp\\Nodemat\\resources\\Roboto-Medium.ttf",16.0f* dpiScale);

    ed::EditorContext* g_Context = ed::CreateEditor();

    bool show_demo_window = true;
    bool show_another_window = false;
    bool show_editor = true;
    bool show_viewport = true;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    MSG msg ;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {    
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }
        
        contextRenderer.ImguiNewframe();

        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport();


        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
                if (ImGui::MenuItem("Redo", "CTRL+Y", false, false)) {}  // Disabled item
                ImGui::Separator();
                if (ImGui::MenuItem("Cut", "CTRL+X")) {}
                if (ImGui::MenuItem("Copy", "CTRL+C")) {}
                if (ImGui::MenuItem("Paste", "CTRL+V")) {}
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world! dpiscale:");                          // Create a window called "Hello, world!" and append into it.
            ImGui::Text("GetWindowDpiScale = %f", dpiScale);               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        if (show_editor) {
            ImGui::Begin("Node editor", &show_editor);
            ed::SetCurrentEditor(g_Context);

            ed::Begin("My Editor");

            int uniqueId = 1;

            // Start drawing nodes.
            ed::BeginNode(uniqueId++);
            ed::PushStyleVar(ed::StyleVar_NodeRounding, 0);
            ImGui::Text("Node A");
            ed::BeginPin(uniqueId++, ed::PinKind::Input);
            ImGui::Text("-> In");
            ed::EndPin();
            ImGui::SameLine();
            ed::BeginPin(uniqueId++, ed::PinKind::Output);
            ImGui::Text("Out ->");
            ed::EndPin();


            ed::EndNode();
            
            ed::End();
            ImGui::End();
        }

        if(show_viewport)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding , ImVec2(0, 0));
            struct CustomConstraints
            {
                // Helper functions to demonstrate programmatic constraints
                static void Square(ImGuiSizeCallbackData* data) { data->DesiredSize.x = data->DesiredSize.y = IM_MAX(data->DesiredSize.x, data->DesiredSize.y); }
                static void Step(ImGuiSizeCallbackData* data) { float step = (float)(int)(intptr_t)data->UserData; data->DesiredSize = ImVec2((int)(data->DesiredSize.x / step + 0.5f) * step, (int)(data->DesiredSize.y / step + 0.5f) * step); }
            };
            ImGui::SetNextWindowSizeConstraints(ImVec2(64, 64), ImVec2(2048, 2048), CustomConstraints::Square);
            ImGui::Begin("View port",&show_viewport, ImGuiWindowFlags_NoScrollbar| ImGuiWindowFlags_NoScrollWithMouse );
            vpRenderer.Frame();
            ImTextureID texId = vpRenderer.GetShaderResourceView();
            ImVec2 uv_min = ImVec2(0.0f, 0.0f);                 // Top-left
            ImVec2 uv_max = ImVec2(1, 1);                       // Lower-right
            float size = IM_MIN(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
            ImGui::SetCursorPosX((ImGui::GetWindowWidth() - size) / 2);
            ImGui::SetCursorPosY((ImGui::GetWindowHeight() - size) / 2);
            ImGui::Image(texId, ImVec2(size, size), uv_min, uv_max);
            ImGui::End();
            ImGui::PopStyleVar(1);
        }

        ImGui::Render();
        contextRenderer.RenderDrawData(ImGui::GetDrawData(), (float*)&clear_color, &io);
       
    }
   // vpRenderer->~ViewportDX11Renderer();
    ed::DestroyEditor(g_Context);
    contextRenderer.ImguiShutdown();
    ImGui::DestroyContext();

    return 0;
}
