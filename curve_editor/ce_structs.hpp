//----------------------------------------------------------------------------------
//		Curve Editor
//		ヘッダファイル (構造体)
//		Visual C++ 2022
//----------------------------------------------------------------------------------

#pragma once

#include "framework.hpp"



namespace ce {
	//---------------------------------------------------------------------
	//		Float Point (floatの点)
	//---------------------------------------------------------------------
	typedef struct tagFloat_Point {
		float x, y;
	} Float_Point;



	//---------------------------------------------------------------------
	//		Double Point (doubleの点)
	//---------------------------------------------------------------------
	typedef struct tagDouble_Point {
		double x, y;
	} Double_Point;



	//---------------------------------------------------------------------
	//		テーマ
	//---------------------------------------------------------------------
	typedef struct tagTheme {
		COLORREF
			bg,
			bg_graph,
			sepr,
			curve,
			handle;
	} Theme;



	//---------------------------------------------------------------------
	//		ウィンドウ
	//---------------------------------------------------------------------
	typedef struct tagWindow {
		HWND
			base,
			main,
			side,
			editor,
			graph,
			prev;
	} Window;



	//---------------------------------------------------------------------
	//		ウィンドウの位置
	//---------------------------------------------------------------------
	typedef struct tagWindow_Position {
		RECT
			rect_wnd,
			padding;
	} Window_Position;



	//---------------------------------------------------------------------
	//		ポイントの位置
	//---------------------------------------------------------------------
	typedef enum tagPoint_Position {
		CTPT_NULL,
		CTPT_CENTER,
		CTPT_LEFT,
		CTPT_RIGHT
	} Point_Position;



	//---------------------------------------------------------------------
	//		ポイントの場所
	//---------------------------------------------------------------------
	typedef struct tagPoint_Address {
		int index;
		Point_Position position;
	} Point_Address;



	//---------------------------------------------------------------------
	//		ポイント(IDモード)
	//---------------------------------------------------------------------
	typedef struct tagPoints_ID {
		POINT
			pt_center,
			pt_left,
			pt_right;
		int type;							//[タイプ]0:初期制御点の左; 1:初期制御点の右; 2:拡張制御点;
	} Points_ID;



	//---------------------------------------------------------------------
	//		設定
	//---------------------------------------------------------------------
	typedef struct tagConfig {
		BOOL
			alert,
			trace,
			auto_copy,
			mode,
			show_handle,
			align_handle;
		int
			theme,
			separator,
			current_id;
	} Config;
}
