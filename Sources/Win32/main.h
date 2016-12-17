
#ifndef MAIN_H
#define MAIN_H


bool OpenGL_Init(HWND hWnd);
void OpenGL_Release(HWND hWnd);

LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);


#endif /* MAIN_H */