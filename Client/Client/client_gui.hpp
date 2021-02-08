#pragma comment (lib, "d3d11.lib")

#define WIN32_LEAN_AND_MEAN

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/fonts.h"
#include "imgui/custom_widgets.h"

#include <d3d11.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#include <cstddef>
#include <array>
#include <wingdi.h>
#include <string>

extern IDXGIFactory* g_pFactory;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace client_gui
{
    static void HelpMarker(const char* desc)
    {
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered())
        {
            ImGui::BeginTooltip();
            ImGui::PushTextWrapPos(ImGui::GetFontSize() * 20.0f);
            ImGui::TextUnformatted(desc);
            ImGui::PopTextWrapPos();
            ImGui::EndTooltip();
        }
    }

    static void combo_box(const std::string& label, ImVec2 top_left, ImVec2 size)
    {
        auto previous_pos = ImGui::GetCursorPos();
        auto wnd_pos = ImGui::GetWindowPos();
        auto abs_top_left = top_left;
        auto abs_bottom_right = ImVec2{top_left.x + size.x, top_left.y + size.y};
        abs_top_left.offset(wnd_pos);
        abs_bottom_right.offset(wnd_pos);

        auto text_rect = ImGui::CalcTextSize(label.c_str());
        auto draw_list = ImGui::GetWindowDrawList();

        const int y_gap = 0;
        const ImVec2 points[] =
        {
            {abs_top_left.x + 20, abs_top_left.y + y_gap},				// top right + text start
            {abs_top_left.x, abs_top_left.y + y_gap},					// top right
            {abs_top_left.x, abs_bottom_right.y},					// bottom left
            abs_bottom_right,									// bottom right		
            {abs_bottom_right.x, abs_top_left.y + y_gap},				// top right
            {abs_top_left.x + 20 + text_rect.x, abs_top_left.y + y_gap} // top right + text end
        };

        const int in = 1;
        const ImVec2 points_inner[] =
        {
            {abs_top_left.x + 20, abs_top_left.y + y_gap + in},				 // top left + text start
            {abs_top_left.x + in, abs_top_left.y + y_gap + in},				 // top left
            {abs_top_left.x + in, abs_bottom_right.y - in},				 // bottom left
            {abs_bottom_right.x - in, abs_bottom_right.y - in},			 // bottom right		
            {abs_bottom_right.x - in, abs_top_left.y + y_gap + in},			 // top right
            {abs_top_left.x + 20 + text_rect.x, abs_top_left.y + y_gap + in} // top right + text end
        };

        draw_list->AddPolyline(&points_inner[0], IM_ARRAYSIZE(points_inner), IM_COL32(124, 124, 124, 255), false, 1.f);
        //draw_list->AddPolyline(&points[0], IM_ARRAYSIZE(points), IM_COL32(0, 0, 0, 255), false, 1.f); 
        //draw_list->AddRectFilledMultiColor({ abs_top_left.x, abs_top_left.y + 7 }, abs_bottom_right, combo_shader1, combo_shader1, combo_shader2, combo_shader2);

        ImGui::SetCursorPos({ top_left.x + 20, top_left.y - text_rect.y / 2 });
        ImGui::Text(label.c_str());

        ImGui::SetCursorPos(previous_pos);
    }

    // Data
    static ID3D11Device* g_pd3dDevice = NULL;
    static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
    static IDXGISwapChain* g_pSwapChain = NULL;
    static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

    // Forward declarations of helper functions
    bool CreateDeviceD3D(HWND hWnd);
    void CleanupDeviceD3D();
    void CreateRenderTarget();
    void CleanupRenderTarget();
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    // window size and location
    static bool show_login = true;
    static ImVec2 login_size{ 332, 163 };
    static ImVec2 menu_size{ 500, 500 };
    static int middle_x = ::GetSystemMetrics(SM_CXSCREEN) / 2;
    static int middle_y = ::GetSystemMetrics(SM_CYSCREEN) / 2;
    static int x_location = middle_x - login_size.x / 2;
    static int y_location = middle_y - login_size.y / 2;
    std::wstring class_name = L"class";
    std::wstring wnd_name = L"window";
    // text entry buffers
    char username_buffer[24];
    char password_buffer[24];
    char key_buffer[38];

    // macro to easily change mouse to hand cursor on hovered widgets
#define HAND_CURSOR if (ImGui::IsItemHovered()) ImGui::SetMouseCursor(ImGuiMouseCursor_Hand)

    // called to render before the user logs in
    void login_frame()
    {
        static bool show_register = false; // if user wants to register account

        // sizing variables
        const int x_gap = 8;                // gap from left edge
        const int y_gap = 25;               // gap from top edge
        const int entry_x_gap = 8;          // entry x gap from combo box border
        const int entry_y_gap = y_gap + 8;  // entry y gap from combo box border
        const int entry_height = 20;        // height of the text entry box
        const int entry_width = 300;        // width of the text entry box

        using namespace ImGui;

        combo_box(show_register ? "register account" : "login", { x_gap, y_gap }, { static_cast<float>(entry_width + entry_x_gap * 2), static_cast<float>(entry_y_gap - y_gap + entry_height * (show_register ? 3 : 2) + entry_x_gap) });

        // entry widgets
        SetCursorPos({ x_gap + entry_x_gap, entry_y_gap });
        SetNextItemWidth(entry_width);
        InputTextWithHint("##username_entry", "username", username_buffer, sizeof(username_buffer));

        SetCursorPos({ x_gap + entry_x_gap, entry_y_gap + entry_height });
        SetNextItemWidth(entry_width);
        InputTextWithHint("##password_entry", "password", password_buffer, sizeof(password_buffer), ImGuiInputTextFlags_Password);
        
        if (show_register)
        {
            SetCursorPos({ x_gap + entry_x_gap, entry_y_gap + entry_height * 2 });
            SetNextItemWidth(entry_width);
            InputTextWithHint("##key_entry", "product key", key_buffer, sizeof(key_buffer), ImGuiInputTextFlags_NoHorizontalScroll);
        }

        // buttons
        ImVec2 button_pos{ x_gap, static_cast<float>(entry_y_gap - y_gap + entry_height * 3 + entry_x_gap + entry_y_gap) };
        SetCursorPos(button_pos);
        if (Button(show_register ? "i have an account" : "i don't have an account", {200, 20}))
            show_register = !show_register;
        HAND_CURSOR;

        SetCursorPos({ button_pos.x + 200 + entry_x_gap, button_pos.y });
        if (Button("submit", { entry_width + entry_x_gap * 2 - 200 - entry_x_gap, 20 }))
        {

        }
        HAND_CURSOR;

        SetCursorPos({ button_pos.x, button_pos.y + entry_height + x_gap });
        Checkbox("remember info", &network::api::remember_login);
        HAND_CURSOR;
    }

    // called to render after the user logs in
    void products_frame()
    {
        using namespace ImGui;
    }

    // main render loop function
    void start()
    {
        ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_UNAWARE);

        // create application window class
        WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, class_name.c_str(), NULL };
        ::RegisterClassEx(&wc);

        // create application window
        HWND hwnd = ::CreateWindow(wc.lpszClassName, wnd_name.c_str(), WS_POPUP, x_location, y_location, (show_login ? login_size.x : menu_size.x), (show_login ? login_size.y : menu_size.y), NULL, NULL, wc.hInstance, NULL);
        
        ::SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

        // initialize Direct3D
        if (!CreateDeviceD3D(hwnd))
        {
            CleanupDeviceD3D();
            ::UnregisterClass(wc.lpszClassName, wc.hInstance);
            ::MessageBoxA(NULL, "window failed to create", NULL, MB_ICONERROR);
            return;
        }

        // show the window
        ::ShowWindow(hwnd, SW_SHOWDEFAULT);
        ::UpdateWindow(hwnd);

        // setup imgui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.IniFilename = NULL;
        ImGui::GetStyle().WindowRounding = 0.0f;
        ImGui::GetStyle().FrameRounding = 0.0f;

        // Setup theme
        ImVec4* colors = ImGui::GetStyle().Colors;
        {
            colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
            colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
            colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
            colors[ImGuiCol_ChildBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.00f);
            colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
            colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
            colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
            colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.21f, 0.22f, 0.54f);
            colors[ImGuiCol_FrameBgHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.40f);
            colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.18f, 0.67f);
            colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
            colors[ImGuiCol_TitleBgActive] = ImVec4(0.29f, 0.29f, 0.29f, 1.00f);
            colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
            colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
            colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
            colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
            colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
            colors[ImGuiCol_CheckMark] = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
            colors[ImGuiCol_SliderGrab] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
            colors[ImGuiCol_SliderGrabActive] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
            colors[ImGuiCol_Button] = ImVec4(0.44f, 0.44f, 0.44f, 0.40f);
            colors[ImGuiCol_ButtonHovered] = ImVec4(0.46f, 0.47f, 0.48f, 1.00f);
            colors[ImGuiCol_ButtonActive] = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
            colors[ImGuiCol_Header] = ImVec4(0.70f, 0.70f, 0.70f, 0.31f);
            colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.70f, 0.80f);
            colors[ImGuiCol_HeaderActive] = ImVec4(0.48f, 0.50f, 0.52f, 1.00f);
            colors[ImGuiCol_Separator] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
            colors[ImGuiCol_SeparatorHovered] = ImVec4(0.72f, 0.72f, 0.72f, 0.78f);
            colors[ImGuiCol_SeparatorActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
            colors[ImGuiCol_ResizeGrip] = ImVec4(0.91f, 0.91f, 0.91f, 0.25f);
            colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.81f, 0.81f, 0.81f, 0.67f);
            colors[ImGuiCol_ResizeGripActive] = ImVec4(0.46f, 0.46f, 0.46f, 0.95f);
            colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
            colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
            colors[ImGuiCol_PlotHistogram] = ImVec4(0.73f, 0.60f, 0.15f, 1.00f);
            colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
            colors[ImGuiCol_TextSelectedBg] = ImVec4(0.87f, 0.87f, 0.87f, 0.35f);
            colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
            colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
            colors[ImGuiCol_NavHighlight] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
            colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        }

        // load fonts we want to use
        ImFont* font_crux = io.Fonts->AddFontFromMemoryTTF(fonts::coders_crux.data(), fonts::coders_crux.size(), 12.0f);
        ImFont* font_crux_small = io.Fonts->AddFontFromMemoryTTF(fonts::coders_crux.data(), fonts::coders_crux.size(), 11.75f);
        ImFont* font_crux_small_small = io.Fonts->AddFontFromMemoryTTF(fonts::coders_crux.data(), fonts::coders_crux.size(), 10.0f);
        ImFont* font_crux_big = io.Fonts->AddFontFromMemoryTTF(fonts::coders_crux.data(), fonts::coders_crux.size(), 20.0f);
        ImFont* block_font = io.Fonts->AddFontFromMemoryTTF(fonts::blocks.data(), fonts::blocks.size(), 54.0f);
        widgets::font_normal = font_crux;
        widgets::font_small = font_crux_small;
        widgets::font_small_small = font_crux_small_small;

        // setup platform/renderer bindings
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

        // disable alt enter fullscreen toggle
        g_pFactory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

        // predefined colors
        const ImVec4 red = ImVec4(0.824f, 0.024f, 0.024f, 1.00f);
        const ImVec4 orange = ImVec4(0.824f, 0.824f, 0.024f, 1.00f);
        const ImVec4 green = ImVec4(0.134f, 0.814f, 0.092f, 1.00f);
        const ImVec4 grey = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        const ImVec4 lightGrey = ImVec4(.618f, .618f, .618, 1.0f);
        const ImVec4 purple = ImVec4(0.647f, 0.130f, 0.716f, 1.0f);
        const ImVec4 lightPurple = ImVec4(.731f, .000f, .828f, 1.0f);
        const ImVec4 darkPurple = ImVec4(.314f, .000f, .368f, 1.0f);
        const ImVec4 backgroundPurple = ImVec4(.090f, .090f, .118f, 1.000f);
        const ImVec4 dullWhite = ImVec4(.926f, .926f, .926f, .95f);
        const ImVec4 magenta = ImVec4(.698f, .000f, 1.00f, 1.0f);
        const ImVec4 moneygGreen = ImVec4(.0f, .699f, .217f, 1.0f);
        const ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // conditional rendering variables
        static bool login_page_show_register = false;
        static bool should_exit = false;

        // window flags for imgui windows
        ImGuiWindowFlags imgui_wnd_flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar;
       
        // main render loop
        MSG msg;
        ZeroMemory(&msg, sizeof(msg));
        while (msg.message != WM_QUIT)
        {
            if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
            {
                ::TranslateMessage(&msg);
                ::DispatchMessage(&msg);
                continue;
            }

            // if they just logged in, we need to resize window
            if (network::api::just_logged_in)
            {
                network::api::just_logged_in = false;

                ::RECT rect;
                if (GetWindowRect(hwnd, &rect))
                {
                    int width = rect.right - rect.left;
                    int height = rect.bottom - rect.top;
                    SetWindowPos(hwnd, NULL, rect.left - (menu_size.x - width) / 2, rect.top - (menu_size.y - height) / 2, menu_size.x, menu_size.y, NULL);
                }
            }

            // Start the imgui frame
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos({ 0, 0 });
            ImGui::SetNextWindowSize(login_size);
            ImGui::Begin("licensing system", &should_exit, imgui_wnd_flags);

            using namespace ImGui;
            {
                // render login window
                if (!network::api::has_logged_in)
                {
                    login_frame();
                }
                // render product window
                else if (network::api::has_logged_in)
                {
                    products_frame();
                }

                // if a message needs to be displayed, open the popup
                if (network::api::show_popup_message)
                    OpenPopup("##message_display");

                // popup window for displaying various messages to the user
                SetNextWindowSize({300, 50});
                if (BeginPopupModal("##message_display", 0, imgui_wnd_flags))
                {
                    if (network::api::show_popup_progression)
                    {
                        std::array<char, 4> spinner = { '/', '-', '\\', '|' };
                        auto progress = spinner[std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() / 100 % 4];
                        SetCursorPos({10, 10});
                        widgets::shadow_text(network::api::popup_message + " [" + progress + "]");
                    }
                    else if (network::api::allow_popup_close)
                    {
                        SetCursorPos({ 10, 10 });
                        widgets::shadow_text(network::api::popup_message);
                        if (widgets::shadow_button("close", { 250, 30 }, { 50, 20 }))
                            network::api::show_popup_message = false; 
                    }

                    if (!network::api::show_popup_message)
                        CloseCurrentPopup();

                    EndPopup();
                }
               
               
            }

            // rendering
            ImGui::End();
            ImGui::Render();

            g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
            g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

            // present with vsync (1, 0), present without vsync (0, 0)
            g_pSwapChain->Present(1, 0); 
        }

        network::api::disconnect();
        network::api::network_thread_should_run = false;

        // cleanup imgui
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupDeviceD3D();
        ::DestroyWindow(hwnd);
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
    }

    bool CreateDeviceD3D(HWND hWnd)
    {
        // Setup swap chain
        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags = 0;
        //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
        if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
            return false;

        CreateRenderTarget();
        return true;
    }

    void CleanupDeviceD3D()
    {
        CleanupRenderTarget();
        if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
        if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
        if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
    }

    void CreateRenderTarget()
    {
        ID3D11Texture2D* pBackBuffer;
        g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
        pBackBuffer->Release();
    }

    void CleanupRenderTarget()
    {
        if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
    }

    // win32 message handler
    LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_SIZE:
            if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
            {
                CleanupRenderTarget();
                g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
                CreateRenderTarget();
            }
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
        }
        return ::DefWindowProc(hWnd, msg, wParam, lParam);
    }
}