#pragma once

#include "../Nova/AI/State.h"
#include "../Nova/Scenes/SceneTitle.h"

namespace TitleState
{
	class MainState : public State<SceneTitle>
	{
	public:
		MainState(SceneTitle* owner) : State(owner) {}
		~MainState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

namespace TitleState
{
	class SettingState : public State<SceneTitle>
	{
	public:
		SettingState(SceneTitle* owner) : State(owner) {}
		~SettingState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	};
}

namespace TitleState
{
	class FadeState : public State<SceneTitle>
	{
	public:
		FadeState(SceneTitle* owner) : State(owner){}
		~FadeState() {}

		void Initialize()override;
		void Update(const float& elapsedTime)override;
		void Finalize()override;

	private:
		static constexpr float FADE_TIME = 1.5f;	//	フェードする時間
		float fadeTimer_ = 0.0f;

	};

}
