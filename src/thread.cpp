#include "pch.h"

#include "detours/detours.h"
#include "include/capi/cef_browser_capi.h"
#include "include/internal/cef_types_win.h"
#include "include/capi/cef_client_capi.h"
#include "include/internal/cef_win.h"

PVOID g_cef_browser_host_create_browser = nullptr;
PVOID g_cef_get_keyboard_handler = nullptr;
PVOID g_cef_get_load_handler = nullptr;
PVOID g_cef_on_key_event = nullptr;
PVOID g_cef_on_load_end = nullptr;
PVOID g_set_window_display_affinity = nullptr;

typedef int(CEF_EXPORT* cef_string_from_ptr_t)(const TCHAR*, size_t, cef_string_utf16_t*);

void SetAsPopup(cef_window_info_t* window_info) {
	window_info->style =
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE;
	window_info->parent_window = NULL;
	window_info->x = CW_USEDEFAULT;
	window_info->y = CW_USEDEFAULT;
	window_info->width = CW_USEDEFAULT;
	window_info->height = CW_USEDEFAULT;
}

cef_string_from_ptr_t func_cef_string_from_ptr = nullptr;

int CEF_CALLBACK hook_cef_on_key_event(
	struct _cef_keyboard_handler_t* self,
	struct _cef_browser_t* browser,
	const struct _cef_key_event_t* event,
	cef_event_handle_t os_event) {

	auto cef_browser_host = browser->get_host(browser);

	if (event->type == KEYEVENT_RAWKEYDOWN) {
		OutputDebugString((TEXT("[CefHook] Pressed: ") + (IntToString(event->windows_key_code)) + TEXT("\n")).c_str());
		if (event->windows_key_code == 18) {
			OutputDebugString(TEXT("[CefHook] Show dev tools\n"));
			cef_window_info_t windowInfo{};
			cef_browser_settings_t settings{};
			cef_point_t point{};
			SetAsPopup(&windowInfo);
			cef_browser_host->show_dev_tools(cef_browser_host, &windowInfo, 0, &settings, &point);
		}
		else {
			_cef_frame_t* frame = browser->get_main_frame(browser);
			string_t eval_str = TEXT("alert(\"You pressed: ") + IntToString(event->windows_key_code) + TEXT("\");");
			cef_string_t eval{};
			func_cef_string_from_ptr(eval_str.c_str(), eval_str.length(), &eval);
			cef_string_t url{};
			string_t url_str = TEXT("");
			func_cef_string_from_ptr(url_str.c_str(), url_str.length(), &url);
			frame->execute_java_script(frame, &eval, &url, 0);
		}
	}

	return reinterpret_cast<decltype(&hook_cef_on_key_event)>
		(g_cef_on_key_event)(self, browser, event, os_event);
}

struct _cef_keyboard_handler_t* CEF_CALLBACK hook_cef_get_keyboard_handler(
	struct _cef_client_t* self) {
	OutputDebugString(TEXT("[CefHook] hook_cef_get_keyboard_handler\n"));

	auto keyboard_handler = reinterpret_cast<decltype(&hook_cef_get_keyboard_handler)>
		(g_cef_get_keyboard_handler)(self);
	if (keyboard_handler) {
		g_cef_on_key_event = (PVOID)(keyboard_handler->on_key_event);
		keyboard_handler->on_key_event = hook_cef_on_key_event;
	}
	return keyboard_handler;
}

void CEF_CALLBACK hook_cef_on_load_end(
	struct _cef_load_handler_t* self,
	struct _cef_browser_t* browser,
	struct _cef_frame_t* frame,
	int httpStatusCode
) {
	(void)browser;
	(void)self;
	OutputDebugString((TEXT("Load end event, statusCode = ") + IntToString(httpStatusCode) + TEXT("\n")).c_str());
	string_t eval_str = TEXT("alert(\"Load end event, statusCode = ") + IntToString(httpStatusCode) + TEXT("\");");
	cef_string_t eval{};
	func_cef_string_from_ptr(eval_str.c_str(), eval_str.length(), &eval);
	cef_string_t url{};
	string_t url_str = TEXT("");
	func_cef_string_from_ptr(url_str.c_str(), url_str.length(), &url);
	frame->execute_java_script(frame, &eval, &url, 0);
};

struct _cef_load_handler_t* CEF_CALLBACK hook_cef_get_load_handler(
	struct _cef_client_t* self) {
	OutputDebugString(TEXT("[CefHook] hook_cef_on_load_end\n"));

	auto load_handler = reinterpret_cast<decltype(&hook_cef_get_load_handler)>
		(g_cef_get_load_handler)(self);
	if (load_handler) {
		g_cef_on_load_end = (PVOID)(load_handler->on_load_end);
		load_handler->on_load_end = hook_cef_on_load_end;
	}
	return load_handler;
}

int hook_cef_browser_host_create_browser(
	const cef_window_info_t* windowInfo,
	struct _cef_client_t* client,
	const cef_string_t* url,
	const struct _cef_browser_settings_t* settings,
	struct _cef_dictionary_value_t* extra_info,
	struct _cef_request_context_t* request_context) {
	OutputDebugString(TEXT("[CefHook] hook_cef_browser_host_create_browser\n"));

	g_cef_get_keyboard_handler = client->get_keyboard_handler;
	client->get_keyboard_handler = hook_cef_get_keyboard_handler;

	g_cef_get_load_handler = (PVOID)(client->get_load_handler);
	client->get_load_handler = hook_cef_get_load_handler;

	return reinterpret_cast<decltype(&hook_cef_browser_host_create_browser)>
		(g_cef_browser_host_create_browser)(
			windowInfo, client, url, settings, extra_info, request_context);
}

BOOL WINAPI hook_set_window_display_affinity(HWND  hWnd, DWORD dwAffinity) {
	OutputDebugString(L"[CefHook] hook_set_window_display_affinity\n");
	if (dwAffinity == WDA_NONE) {
		return true;
	}
	return reinterpret_cast<decltype(&hook_set_window_display_affinity)>
		(g_set_window_display_affinity)(hWnd, dwAffinity);
}

BOOL APIENTRY InstallHook()
{
	OutputDebugString(TEXT("[CefHook] Installing hook...\n"));
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	g_cef_browser_host_create_browser =
		DetourFindFunction("libcef.dll", "cef_browser_host_create_browser");
#ifdef UNICODE
	func_cef_string_from_ptr = reinterpret_cast<cef_string_from_ptr_t>(DetourFindFunction("libcef.dll", "cef_string_wide_to_utf16"));
#elif
	func_cef_string_from_ptr = reinterpret_cast<cef_string_from_ptr_t>(DetourFindFunction("libcef.dll", "cef_string_ascii_to_utf16"));
#endif
	DetourAttach(&g_cef_browser_host_create_browser, (PVOID)hook_cef_browser_host_create_browser);
	LONG ret1 = DetourTransactionCommit();

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	g_set_window_display_affinity =
		DetourFindFunction("user32.dll", "SetWindowDisplayAffinity");
	DetourAttach(&g_set_window_display_affinity, (PVOID)hook_set_window_display_affinity);
	LONG ret2 = DetourTransactionCommit();

	return ret1 == NO_ERROR && ret2 == NO_ERROR;
}


DWORD WINAPI ThreadProc(LPVOID lpThreadParameter)
{
	(void)lpThreadParameter;
	HANDLE hProcess;

	PVOID addr1 = reinterpret_cast<PVOID>(0x00401000);
	BYTE data1[] = { 0x90, 0x90, 0x90, 0x90 };

	//
	// 绕过VMP3.x 的内存保护
	//
	hProcess = OpenProcess(PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE, FALSE, GetCurrentProcessId());
	if (hProcess)
	{
		WriteProcessMemory(hProcess, addr1, data1, sizeof(data1), NULL);

		CloseHandle(hProcess);
	}

	OutputDebugString(InstallHook() ? TEXT("Hooked Successfully") : TEXT("Hooking failed"));
	return 0;
}
