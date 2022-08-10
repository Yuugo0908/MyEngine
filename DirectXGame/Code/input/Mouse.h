#pragma once
#include <Windows.h>
#include <wrl.h>

#define DIRECTINPUT_VERSION     0x0800          // DirectInput�̃o�[�W�����w��
#include <dinput.h>
class Mouse
{
public:
	// �}�E�X�̍��W
	struct MouseMove {
		long MouseX;
		long MouseY;
		long MouseZ;
	};

private: // �G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // �ÓI�����o�ϐ�
	static Mouse* GetInstance();

public: // �����o�֐�

	// ������
	bool Initialize(HINSTANCE hInstance, HWND hwnd);

	// �X�V
	void Update();

	// �L�[�̍��{�^���������`�F�b�N
	bool PushMouseLeft();

	// �L�[�̒��{�^���������`�F�b�N
	bool PushMouseMiddle();

	// �L�[�̍��{�^���g���K�[���`�F�b�N
	bool TriggerMouseLeft();

	// �L�[�̒��{�^���g���K�[���`�F�b�N
	bool TriggerMouseMiddle();

	// �}�E�X�ړ��ʂ��擾
	MouseMove GetMouseMove();

private: // �����o�ϐ�

	// �f�o�C�X�֘A
	ComPtr<IDirectInput8> dinput;
	ComPtr<IDirectInputDevice8> devMouse;
	DIMOUSESTATE2 mouseState = {};
	DIMOUSESTATE2 mouseStatePre = {};
};

