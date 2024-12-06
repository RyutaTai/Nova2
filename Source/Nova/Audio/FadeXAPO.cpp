#include "FadeXAPO.h"

//	XAPOプロパティの定義
XAPO_REGISTRATION_PROPERTIES FadeXAPO::xapoRegProp_ =
{
	__uuidof(FadeXAPO),		//	クラスに設定されているGUIDを渡す
	L"FadeXAPO",			//	フレンドリ名(このクラスの名前)
	L"(c) Ryuta Tai",		//	著作権情報(著作者名・会社名など)
	1,0,					//	メジャーバージョン番号:1、マイナーバージョン番号:0
	XAPOBASE_DEFAULT_FLAG,	//	動作指定フラグ(このクラスがどのようなデータを受け付けられるかの動作指定)
	1,1,1,1
};

//	コンストラクタ
FadeXAPO::FadeXAPO() :CXAPOBase(&xapoRegProp_)
{

}

//	LockForProcess関数(事前処理をここに書き、Process関数の処理を少しでも軽くする)
HRESULT FadeXAPO::LockForProcess(
	UINT32 InputLockedParameterCount,const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pInputLockedParameters, 
	UINT32 OutputLockedParameterCount,	const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* pOutputLockedParameters)
{
	inputFmt_ = *pInputLockedParameters[0].pFormat;
	outputFmt_ = *pOutputLockedParameters[0].pFormat;

	return CXAPOBase::LockForProcess(InputLockedParameterCount, pInputLockedParameters, OutputLockedParameterCount, pOutputLockedParameters);
}

//	Process関数
void FadeXAPO::Process(UINT32 InputProcessParameterCount, const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters, 
	UINT32 OutputProcessParameterCount, XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters, BOOL IsEnabled)
{
	const XAPO_PROCESS_BUFFER_PARAMETERS& inputParam = pInputProcessParameters[0];
	XAPO_PROCESS_BUFFER_PARAMETERS& outputParam = pOutputProcessParameters[0];

	memcpy(outputParam.pBuffer, inputParam.pBuffer, outputFmt_.nBlockAlign * inputParam.ValidFrameCount);

	outputParam.ValidFrameCount = inputParam.ValidFrameCount;
	outputParam.BufferFlags = inputParam.BufferFlags;

}