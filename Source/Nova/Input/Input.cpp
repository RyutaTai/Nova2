#include "../Input/Input.h"

#include <windows.h>

Input* Input::instance_ = nullptr;

// コンストラクタ
Input::Input(HWND hWnd)
	: mouse_(hWnd)
{
	instance_ = this;
}

// 更新処理
void Input::Update()
{
	gamePad_.Update();
	mouse_.Update();
}

//	入力キー設定
void Input::SetInputKey(const InputKey& key)
{
	//	古い入力キーを一つずつ隣にずらす
	for (int i = 1; i < MaxInputKey; ++i)
	{
		inputKeys_[i] = inputKeys_[i - 1];
	}

	//	新しい入力キーを設定
	inputKeys_[0].key_ = key;
}

//	猶予フレーム内にコマンドが押されているか
bool Input::CommandConfirm(const Command& command, const float& frame)
{
	int currentCommand = command.at(0);	//	コマンドの最初から確認する
	for (int i = 0; i < MaxInputKey; ++i)
	{

	}

	return false;
}

//	入)力情報更新処理
void Input::UpdateKeyData()
{
	//	キー入力状態を取得
	bool up		= (gamePad_.GetButton() & GamePad::BTN_UP) != 0;
	bool right	= (gamePad_.GetButton() & GamePad::BTN_RIGHT) != 0;
	bool down	= (gamePad_.GetButton() & GamePad::BTN_DOWN) != 0;
	bool left	= (gamePad_.GetButtonDown() & GamePad::BTN_LEFT) != 0;
	bool punch	= (gamePad_.GetButton() & GamePad::BTN_Y) != 0;
	bool kick	= (gamePad_.GetButton() & GamePad::BTN_B) != 0;
	//	スティック入力値をキー入力に割り当て
	if (gamePad_.GetAxisLX() < -0.5f)	left = true;
	if (gamePad_.GetAxisLX() > 0.5f)	right = true;
	if (gamePad_.GetAxisLY() < -0.5f)	down = true;
	if (gamePad_.GetAxisLY() > 0.5f)	up	= true;

	//	キー入力設定
	InputKey key = 0;
	key |= (down && left) ? Key1 : Not1;
	key |= (down) ? Key2 : Not2;
	key |= (down && right) ? Key3 : Not3;
	key |= (left) ? Key4 : Not4;
	key |= (!up && !down && !left && !right) ? Key5 : Not5;
	key |= (right) ? Key6 : Not6;
	key |= (up && left) ? Key7 : Not7;
	key |= (up) ? Key8 : Not8;
	key |= (up && right) ? Key9 : Not9;
	key |= (punch) ? KeyP : NotP;
	key |= (kick) ? KeyK : NotK;

	//	斜めキー入力がある場合は重複する上下左右キーの入力を外す
	if (key & Key1)key &= ~(Key2 | Key4);
	if (key & Key3)key &= ~(Key2 | Key6);
	if (key & Key7)key &= ~(Key8 | Key4);
	if (key & Key9)key &= ~(Key8 | Key6);

	SetInputKey(key);

	//	スタックされた入力情報の経過時間を更新
	for (int i = 0; i < MaxInputKey; ++i)
	{
		inputKeys_[i].frame_++;
		if (inputKeys_[i].frame_ > SaveFrame)	//	入力保持時間を超えたら無効にする
		{
			inputKeys_[i].key_ = 0;
		}
	}

}

//	入力情報リセット
void Input::ResetInputData()
{
	for (int i = 0; i < MaxInputKey; ++i)
	{
		inputKeys_[i].frame_ = 0;
		inputKeys_[i].key_ = 0;
	}
}

//	デバッグ描画
void Input::DrawDebug()
{

}
