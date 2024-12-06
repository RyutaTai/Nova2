#pragma once

#pragma comment(lib,"xapobase.lib")

#include <xapobase.h>

//	フェードエフェクトXAPO
class __declspec(uuid("{FD16C679-5181-428C-8180-F42268EDA607}"/*GUID*/))
	FadeXAPO :public CXAPOBase
{
public:
	FadeXAPO();
	~FadeXAPO() {}

	//	LOckForProcess関数をオーバーライド
	STDMETHOD(LockForProcess)(UINT32 InputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters, UINT32 OutputLockedParameterCount, const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters);

	//	Process関数をオーバーライド
	STDMETHOD_(void, Process)(UINT32 InputProcessParameterCount, const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters, UINT32 OutputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters, BOOL IsEnabled);

private:
	static XAPO_REGISTRATION_PROPERTIES xapoRegProp_;	//	プロパティ

	WAVEFORMATEX inputFmt_;		//	入力フォーマットの記憶場所
	WAVEFORMATEX outputFmt_;	//	出力フォーマットの記憶場所

};

