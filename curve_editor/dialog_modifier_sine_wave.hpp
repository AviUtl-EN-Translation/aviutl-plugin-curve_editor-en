#pragma once

#include "dialog.hpp"
#include "modifier_sine_wave.hpp"



namespace curve_editor {
	class SineWaveModifierDialog : public Dialog {
		HWND hwnd_slider_amplitude_ = NULL;
		HWND hwnd_slider_frequency_ = NULL;
		HWND hwnd_slider_phase_ = NULL;
		HWND hwnd_static_amplitude_ = NULL;
		HWND hwnd_static_frequency_ = NULL;
		HWND hwnd_static_phase_ = NULL;

		int resource_id() const noexcept override;
		INT_PTR dialog_proc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) override;
		void init_controls(HWND hwnd, const SineWaveModifier* p_mod_sinewave) noexcept;
	};
} // namespace curve_editor