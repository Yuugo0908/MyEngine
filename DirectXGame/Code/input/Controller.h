#pragma once
#pragma comment(lib,"XInput.lib")
#include <windows.h>
#include <Xinput.h>


class Controller
{
private:
	Controller();

	~Controller();
public:
	enum class State
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
	enum class Mode
	{
		BUTTON,
		STICK,
	};

	enum class Type
	{
		NONE,
		PUSH,
		TRIGGER,
	};

public:
	static Controller* GetInstance();
	bool Update();
	bool GetPadState(State p_state, Type p_type);
	void Vibration();

private:
	bool GetButtonState(XINPUT_STATE state, State p_state, Type p_type);
	bool GetStickState(XINPUT_STATE state, State p_state);
	bool PushButton(State& p_state);
	bool TriggerButton(XINPUT_STATE state, State& p_state);
	void CheckMode(State p_state);

	Mode mode{};
	XINPUT_STATE state{};
	XINPUT_STATE statePre{};
	State stateNum{};
	WORD buttonNum{};
	XINPUT_VIBRATION vibration{};
	const int controllerNum = 0;
	int vCount = 0;
};