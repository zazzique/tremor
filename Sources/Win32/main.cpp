
#include <windows.h>
#include "glee.h"
#include "Common.h"
#include "Vector.h"
#include "Core.h"
#include "CoreDefines.h"
#include "Game.h"
#include "main.h"

#define TC_USE_CONSOLE

float input_x = 0.0f;
float input_y = 0.0f;
bool  input_button_down = false;

const int V_SX = 1024;
const int V_SY = 600;
const float PIXEL_SCALE = 1.0f;


Core* core = nullptr;

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	static POINT cursor_pos;
	static bool show_crosshair = true;
	static Vector2D locked_input;

	switch (msg)
	{
		case WM_RBUTTONDOWN:
			input_x = LOWORD(lParam);
			input_y = HIWORD(lParam);

			input_x /= PIXEL_SCALE;
			input_y /= PIXEL_SCALE;

			show_crosshair = !show_crosshair;

			if (show_crosshair)
			{
				ShowCursor(true);

				if (core)
					core->InputTouchEnded(locked_input.x, (V_SY * PIXEL_SCALE) - locked_input.y);
			}
			else
			{
				locked_input.x = 0;
				locked_input.y = 0;

				ShowCursor(false);
				GetCursorPos(&cursor_pos);
				
				if (core)
					core->InputTouchBegan(locked_input.x, (V_SY * PIXEL_SCALE) - locked_input.y);
			}
			break;

		case WM_LBUTTONDOWN:
			if (!show_crosshair)
				break;

			input_x = LOWORD(lParam);
			input_y = HIWORD(lParam);

			input_x /= PIXEL_SCALE;
			input_y /= PIXEL_SCALE;

			if (!input_button_down)
			{
				input_button_down = true;
				
				if (core)
					core->InputTouchBegan(input_x, (V_SY * PIXEL_SCALE) - input_y);
			}
			break;

		case WM_LBUTTONUP:
			if (!show_crosshair)
				break;

			input_x = LOWORD(lParam);
			input_y = HIWORD(lParam);

			input_x /= PIXEL_SCALE;
			input_y /= PIXEL_SCALE;

			if (input_button_down)
			{
				input_button_down = false;
				
				if (core)
					core->InputTouchEnded(input_x, (V_SY * PIXEL_SCALE) - input_y);
			}
			break;

		case WM_MOUSEMOVE:
			if (input_button_down)
			{
				input_x = LOWORD(lParam);
				input_y = HIWORD(lParam);

				input_x /= PIXEL_SCALE;
				input_y /= PIXEL_SCALE;
								
				if (core)
					core->InputTouchMoved(input_x, (V_SY * PIXEL_SCALE) - input_y);
			}

			if (!show_crosshair)
			{
				POINT current_cursor_pos;
				GetCursorPos(&current_cursor_pos);

				float x = (float)(current_cursor_pos.x - cursor_pos.x);
				float y = (float)(current_cursor_pos.y - cursor_pos.y);

				x /= PIXEL_SCALE;
				y /= PIXEL_SCALE;

				locked_input.x += x;
				locked_input.y += y;
				SetCursorPos(cursor_pos.x, cursor_pos.y);

				if (core)
					core->InputTouchMoved(locked_input.x, (V_SY * PIXEL_SCALE) - locked_input.y);
			}
			break;

		case WM_KEYDOWN:
			switch (wParam)
			{
				/*case 'W':
					gameControls.movement.y = 1.0f;
					break;
				case 'S':
					gameControls.movement.y = -1.0f;
					break;*/
			}
			break;

		case WM_KEYUP:
			switch (wParam)
			{
				/*case 'W':
					gameControls.movement.y = 0.0f;
					break;
				case 'S':
					gameControls.movement.y = 0.0f;
					break;*/
			}
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		default:
			result = DefWindowProc( hWnd, msg, wParam, lParam );
			break;
	}

	return result;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifdef TC_USE_CONSOLE
	AllocConsole();

	freopen( "CON", "w", stdout );
	freopen( "CON", "w", stderr );
#endif

	WNDCLASS wc;
	memset( &wc, 0, sizeof( wc ) );

	wc.style         = 0;
	wc.lpfnWndProc   = (WNDPROC) MsgProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = nullptr;
	wc.hCursor       = LoadCursor (nullptr,IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName  = nullptr;
	wc.lpszClassName = "tremor_core";

	if (!RegisterClass (&wc))
		return 0;

	HWND hWnd = CreateWindowEx(0, "tremor_core", "CSM", WS_OVERLAPPED | WS_EX_TOPMOST | WS_CAPTION | WS_BORDER | WS_SYSMENU,
							   100, 100, V_SX, V_SY, nullptr, nullptr, hInstance, nullptr);
    
	RECT window_rect;
	GetWindowRect(hWnd, &window_rect);
	RECT client_rect;
	GetClientRect( hWnd, &client_rect );
	INT w_sx = V_SX + (window_rect.right - window_rect.left) - (client_rect.right - client_rect.left);
	INT w_sy = V_SY + (window_rect.bottom - window_rect.top) - (client_rect.bottom - client_rect.top);
	INT s_sx = GetSystemMetrics(SM_CXSCREEN);
	INT s_sy = GetSystemMetrics(SM_CYSCREEN);
	INT w_x = (s_sx - w_sx) / 2;
	INT w_y = (s_sy - w_sy) / 2;
	if (w_x < 0) w_x = 0;
	if (w_y < 0) w_y = 0;
	MoveWindow( hWnd, w_x, w_y, w_sx, w_sy, true );


	ApplicationSettings settings;
	settings.hWnd = hWnd;
	settings.screen_width = V_SX;
	settings.screen_height = V_SY;

	Core* core = new Core(settings);

	if (core == nullptr)
		return 0;

	ShowWindow( hWnd, SW_SHOWDEFAULT );
	UpdateWindow( hWnd );
		
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg,nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		core->Process();
		core->Render();
		
		Sleep(5);
	}
	
	if (core != nullptr)
		delete core;

	DestroyWindow( hWnd );

	UnregisterClass( "tremor_core", hInstance );
	
	return 0;
}

