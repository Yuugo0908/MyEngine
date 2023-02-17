#pragma once
#pragma comment(lib,"XInput.lib")
#include <windows.h>
#include <Xinput.h>

class Controller
{
public:
	enum State
	{
		// �f�W�^��
		A = 1,	//	A�{�^��
		B,	//	B�{�^��
		X,	//	X�{�^��
		Y,	//	Y�{�^��
		L_SHOULDER,  //	LB
		R_SHOULDER,	//	RB
		BACK,	//	Back
		START,	//	Start
		UP,		//	�\���L�[�̏�
		DOWN,	//	�\���L�[�̉�
		LEFT,	//	�\���L�[�̍�
		RIGHT,	//	�\���L�[�̉E
		LEFT_THUMB,		//	���X�e�B�b�N
		RIGHT_THUMB,	//	�E�X�e�B�b�N
		// �A�i���O
		L_TRIGGER,	//	���g���K�[
		R_TRIGGER,	//	�E�g���K�[
		LEFT_U_STICK,	//	���X�e�B�b�N�̏�
		LEFT_D_STICK,	//	���X�e�B�b�N�̉�
		LEFT_L_STICK,	//	���X�e�B�b�N�̍�
		LEFT_R_STICK,	//	���X�e�B�b�N�̉E
		RIGHT_U_STICK,	//	�E�X�e�B�b�N�̏�
		RIGHT_D_STICK,	//	�E�X�e�B�b�N�̉�
		RIGHT_L_STICK,	//	�E�X�e�B�b�N�̍�
		RIGHT_R_STICK,	//	�E�X�e�B�b�N�̉E
	};
	enum Mode
	{
		BUTTON,
		STICK,
	};

	enum Type
	{
		NONE,
		PUSH,
		TRIGGER,
	};

public:
	static Controller* GetInstance();
	bool IsConnected();
	bool GetPadState(State p_state, Type p_type);

private:
	Mode mode;
	bool GetButtonState(XINPUT_STATE state, State p_state, Type p_type);
	bool GetStickState(XINPUT_STATE state, State p_state);
	bool PushButton(State& p_state);
	bool TriggerButton(XINPUT_STATE state, State& p_state);
	void CheckMode(State p_state);

	XINPUT_STATE state;
	XINPUT_STATE statePre;
	State stateNum;
};