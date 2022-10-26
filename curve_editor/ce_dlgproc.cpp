//----------------------------------------------------------------------------------
//		Curve Editor
//		ソースファイル (ダイアログプロシージャ)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#include "ce_header.hpp"

#define CE_REGEX_VALUE				R"(^((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *, *)|(-?\d*\.\d* *, *))((\d+ *, *)|(\d*\.\d* *, *))((-?\d+ *)|(-?\d*\.\d* *))$)"
#define CE_REGEX_FLOW_1				R"()"
#define CE_REGEX_FLOW_2				R"(^\s*\[\s*(\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\},)+\s*\{\s*"name"\s*:\s*".*"\s*,\s*"curve"\s*:\s*\[\s*(\s*-?\d\.?\d+\s*,){3}\s*-?\d\.?\d+\s*\]\s*\}\s*\]\s*$)"
#define CE_REGEX_CEP				R"(^(\s*\{\s*".*"(\s*\[\s*-?\d?\.?\d+\s*,\s*-?\d?\.?\d+\s*\]\s*)+\s*\}\s*)+$)"



//---------------------------------------------------------------------
//		ダイアログプロシージャ（設定ダイアログ）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_config(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static HWND combo;

	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_INITDIALOG:
		if (g_config.trace)
			::SendMessage(
				::GetDlgItem(hwnd, IDC_PREVIOUSCURVE),
				BM_SETCHECK,
				BST_CHECKED, 0
			);

		if (g_config.alert)
			::SendMessage(
				::GetDlgItem(hwnd, IDC_ALERT),
				BM_SETCHECK,
				BST_CHECKED, 0
			);

		if (g_config.auto_copy)
			::SendMessage(
				::GetDlgItem(hwnd, IDC_AUTOCOPY),
				BM_SETCHECK,
				BST_CHECKED, 0
			);

		combo = ::GetDlgItem(hwnd, IDC_THEME);
		::SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"ダーク");
		::SendMessage(combo, CB_ADDSTRING, 0, (LPARAM)"ライト");
		::SendMessage(combo, CB_SETCURSEL, g_config.theme, 0);

		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			g_config.trace = ::SendMessage(
				::GetDlgItem(hwnd, IDC_PREVIOUSCURVE),
				BM_GETCHECK, 0, 0
			);
			g_config.alert = ::SendMessage(
				::GetDlgItem(hwnd, IDC_ALERT),
				BM_GETCHECK, 0, 0
			);
			g_config.auto_copy = ::SendMessage(
				::GetDlgItem(hwnd, IDC_AUTOCOPY),
				BM_GETCHECK, 0, 0
			);
			g_config.theme = ::SendMessage(
				combo,
				CB_GETCURSEL, 0, 0
			);

			g_fp->exfunc->ini_save_int(g_fp, "theme", g_config.theme);
			g_fp->exfunc->ini_save_int(g_fp, "show_previous_curve", g_config.trace);
			g_fp->exfunc->ini_save_int(g_fp, "show_alerts", g_config.alert);
			g_fp->exfunc->ini_save_int(g_fp, "auto_copy", g_config.auto_copy);

			::EndDialog(hwnd, 1);

			return 0;

		case IDCANCEL:
			::EndDialog(hwnd, 1);

			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（カーブ値の設定）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_value(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	TCHAR buffer[30];
	std::regex re(CE_REGEX_VALUE);

	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			::GetDlgItemText(hwnd, IDC_EDIT_VALUE, buffer, 30);
			if (std::regex_match(buffer, re)) {
				std::string str = buffer;
				std::vector<std::string> vec = ce::split(buffer, ',');

				float values[4];

				values[0] = MINMAXLIM(std::stof(vec[0]), 0, 1);
				values[1] = MINMAXLIM(
					std::stof(vec[1]),
					CE_CURVE_VALUE_MIN_Y,
					CE_CURVE_VALUE_MAX_Y
				);
				values[2] = MINMAXLIM(std::stof(vec[2]), 0, 1);
				values[3] = MINMAXLIM(
					std::stof(vec[3]),
					CE_CURVE_VALUE_MIN_Y,
					CE_CURVE_VALUE_MAX_Y
				);

				g_curve_value.ctpts[0].pt_right.x = (int)(values[0] * CE_GR_RESOLUTION);
				g_curve_value.ctpts[0].pt_right.y = (int)(values[1] * CE_GR_RESOLUTION);
				g_curve_value.ctpts[1].pt_left.x = (int)(values[2] * CE_GR_RESOLUTION);
				g_curve_value.ctpts[1].pt_left.y = (int)(values[3] * CE_GR_RESOLUTION);

				::EndDialog(hwnd, 1);
			}
			else if (g_config.alert)
				::MessageBox(hwnd, CE_STR_INVALIDINPUT, CE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);

			return 0;

		case IDCANCEL:
			::EndDialog(hwnd, 1);
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ(カーブ読み取り)
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_read(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// エディットコントロール
	HWND edit;
	TCHAR buffer[12];
	int value;
	// 値の正規表現
	std::regex re(R"(^-?\d+$)");
	std::string str;

	switch (msg) {
	case WM_INITDIALOG:
		edit = ::GetDlgItem(hwnd, IDC_EDIT_READ);
		::SendMessage(edit, EM_SETLIMITTEXT, 11, 0);
		return 0;

	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_KEYDOWN:
		if (wparam == VK_RETURN)
			::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			::GetDlgItemText(hwnd, IDC_EDIT_READ, buffer, 11);
			if (std::regex_match(buffer, re)) {
				str = buffer;
				try {
					value = std::stoi(str);
				}
				catch (std::out_of_range& e) {
					if (g_config.alert)
						MessageBox(hwnd, CE_STR_OUTOFRANGE, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

					return 0;
				}
				if (!ISINRANGEEQ(value, -2147483647, 2122746761)) {
					if (g_config.alert)
						::MessageBox(hwnd, CE_STR_OUTOFRANGE, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

					return 0;
				}
				g_curve_value.read_value_1d(value);
				::EndDialog(hwnd, 1);
			}
			else if (g_config.alert)
				::MessageBox(hwnd, CE_STR_INVALIDINPUT, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

			return 0;
		case IDCANCEL:
			::EndDialog(hwnd, 1);
			return 0;
		}
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（カーブ保存ダイアログ）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_save(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	HWND edit;
	//TCHAR buffer[64];

	switch (msg) {
	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;
	case WM_INITDIALOG:
		edit = ::GetDlgItem(hwnd, IDC_EDIT_SAVE);
		::SendMessage(edit, EM_SETLIMITTEXT, 128, 0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			/*ce::Preset additem;
			::GetDlgItemText(hwnd, IDC_EDIT_SAVE, buffer, 64);
			if (strlen(buffer) < 64 && strlen(buffer) != 0) {
				additem = { buffer, g_curve_value.ctpt[0].x, g_curve_value.ctpt[0].y, g_curve_value.ctpt[1].x, g_curve_value.ctpt[1].y };
				g_presets.emplace_back(additem);
				::EndDialog(hwnd, 1);
			}
			else if (strlen(buffer) == 0 && g_config.alert) MessageBox(hwnd, CE_STR_INPUTANAME, CE_PLUGIN_NAME, MB_OK | MB_ICONINFORMATION);*/
			return 0;
		case IDCANCEL:
			::EndDialog(hwnd, 1);
			return 0;
		}
		return 0;
	}
	return 0;
}



//---------------------------------------------------------------------
//		ダイアログプロシージャ（指定したIDに移動）
//---------------------------------------------------------------------
BOOL CALLBACK dialogproc_id(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	// エディットコントロール
	HWND edit;
	TCHAR buffer[5];
	int value;
	std::string str;

	switch (msg) {
	case WM_INITDIALOG:
		edit = ::GetDlgItem(hwnd, IDC_EDIT_ID);
		::SendMessage(edit, EM_SETLIMITTEXT, 4, 0);
		return 0;

	case WM_CLOSE:
		::EndDialog(hwnd, 1);
		return 0;

	case WM_KEYDOWN:
		if (wparam == VK_RETURN)
			::SendMessage(hwnd, WM_COMMAND, MAKEWPARAM(IDOK, 0), 0);
		return 0;

	case WM_COMMAND:
		switch (LOWORD(wparam)) {
		case IDOK:
			::GetDlgItemText(hwnd, IDC_EDIT_ID, buffer, 5);

			if (strlen(buffer) == 0) {
				if (g_config.alert)
					::MessageBox(hwnd, CE_STR_INVALIDINPUT, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

				return 0;
			}

			str = buffer;
			value = std::stoi(str);

			if (!ISINRANGEEQ(value, 0, CE_CURVE_MAX - 1)) {
				if (g_config.alert)
					::MessageBox(hwnd, CE_STR_OUTOFRANGE, CE_PLUGIN_NAME, MB_OK | MB_ICONERROR);

				return 0;
			}

			::SendMessage(g_window_header.hwnd, WM_COMMAND, CE_CM_CHANGE_ID, (LPARAM)value);
			::EndDialog(hwnd, 1);
			return 0;
		case IDCANCEL:
			::EndDialog(hwnd, 1);
			return 0;
		}
	}
	return 0;
}