#pragma once
#include <Windows.h>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "WinApp.h"
#include "Operator.h"

class Controller
{
protected: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public: // �T�u�N���X
	//�Q�[���p�b�h�i�{�^���j
	enum ButtonKind
	{
		Button_A,
		Button_B,
		Button_X,
		Button_Y,
		Button_LB,
		Button_RB,
		Select,
		Start,
		Button_LS,
		Button_RS,
		Cross_Up,
		Cross_Down,
		Cross_Right,
		Cross_Left,
		ButtonMax
	};

	//�}�E�X
	enum MouseKind
	{
		M_Left, M_Right, M_Middle
	};

	//�X�e�B�b�N
	enum StickKind
	{
		S_Up, S_Down, S_Right, S_Left
	};
public: //�����o�֐�
	//������
	void Initialize(WinApp* win_app);
	//�X�V
	void Update();
	//�Q�[���p�b�h�X�e�B�b�N
	bool TiltLeftStick(int stick);
	//�Q�[���p�b�h�X�e�B�b�N�i�������s�j
	bool TriggerLeftStick(int stick);
	//�Q�[���p�b�h�X�e�B�b�N��|�����䗦
	XMFLOAT2 LeftStickAngle();
	//�Q�[���p�b�h�{�^��
	bool PushButton(int Button);
	//�Q�[���p�b�h�{�^���i�������s�j
	bool TriggerButton(int Button);
	//�Q�[���p�b�h�\���L�[
	bool PushCrossKey(int CrossKey);
	//�Q�[���p�b�h�\���L�[�i�������s�j
	bool TriggerCrossKey(int CrossKey);
	// �C���X�^���X
	static Controller* GetInstance();

private: //�����o�ϐ�
	//DirectInput�̃C���X�^���X
	ComPtr<IDirectInput8> dinput;
	//�Q�[���R���g���[���[
	ComPtr<IDirectInputDevice8> devController;
	//�Q�[���p�b�h�̔���
	DIJOYSTATE gamePadState = {};
	//�O�t���[���̃Q�[���p�b�h�̔���
	DIJOYSTATE oldGamePadState = {};
	//�{�^���f�[�^
	bool is_push[32] = {};
	//�X�e�B�b�N�̔����͈�
	LONG responsive_range = 100;
	//�X�e�B�b�N�̖������͈�
	LONG unresponsive_range = 20;
};

