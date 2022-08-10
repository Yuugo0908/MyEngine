#include "Mouse.h"
#include <cassert>

#pragma comment(lib, "dinput8.lib")

Mouse* Mouse::GetInstance()
{
	static Mouse instance;

	return &instance;
}

bool Mouse::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	HRESULT result = S_FALSE;

	assert(!dinput);

	// DirectInput�I�u�W�F�N�g�̐���	
	result = DirectInput8Create(hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �}�E�X�f�o�C�X�̐���	
	result = dinput->CreateDevice(GUID_SysMouse, &devMouse, NULL);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// ���̓f�[�^�`���̃Z�b�g
	result = devMouse->SetDataFormat(&c_dfDIMouse2); // �W���`��
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	// �r�����䃌�x���̃Z�b�g
	result = devMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	if (FAILED(result)) {
		assert(0);
		return false;
	}

	return true;
}

void Mouse::Update()
{
	HRESULT result;

	// �}�E�X
	{
		// �}�E�X����J�n
		result = devMouse->Acquire();

		// �O��̓��͂�ۑ�
		mouseStatePre = mouseState;

		// �}�E�X�̓���
		result = devMouse->GetDeviceState(sizeof(mouseState), &mouseState);
	}
}

bool Mouse::PushMouseLeft()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[0]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Mouse::PushMouseMiddle()
{
	// 0�łȂ���Ή����Ă���
	if (mouseState.rgbButtons[2]) {
		return true;
	}

	// �����Ă��Ȃ�
	return false;
}

bool Mouse::TriggerMouseLeft()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!mouseStatePre.rgbButtons[0] && mouseState.rgbButtons[0]) {
		return true;
	}

	// �g���K�[�łȂ�
	return false;
}

bool Mouse::TriggerMouseMiddle()
{
	// �O��0�ŁA����0�łȂ���΃g���K�[
	if (!mouseStatePre.rgbButtons[2] && mouseState.rgbButtons[2]) {
		return true;
	}

	// �g���K�[�łȂ�
	return false;
}

Mouse::MouseMove Mouse::GetMouseMove()
{
	MouseMove tmp;
	tmp.MouseX = mouseState.lX;
	tmp.MouseY = mouseState.lY;
	tmp.MouseZ = mouseState.lZ;
	return tmp;
}