#pragma once

#include "../Nova/Resources/GltfModelStaticBatching.h"
#include "../Nova/Resources/Effect.h"
#include "../Game/BulletManager.h"
#include "../Nova/Audio/AudioSource3D.h"

//	前方宣言
//	相互インクルードしないように前方宣言する
class BulletManager;

//	弾丸クラス
class Bullet
{
public:
	Bullet(const std::string& filename);
	virtual ~Bullet() {}

	virtual void			Initialize();
	virtual void			Update(const float& elapsedTime);
	virtual void			Render()			= 0;
	virtual void			RnederCoverModel()	= 0;

	virtual void			CoverModelUpdate(const float& elapsedTime);									//	カバーモデル更新処理
	virtual void			Destroy(const float& elapsedTime);											//	破棄処理
	
	void					SetInvincible(const bool& isInvincible)			{ isInvincible_ = isInvincible; }
	void					SetDamaged(bool damaged)						{ damaged_ = damaged; }		//	ダメージフラグ設定
	void					SetOwnerPosition(const DirectX::XMFLOAT3& pos)	{ ownerPosition_ = pos; }	//	弾丸所有者の位置設定
	
	virtual void			DrawDebug();																//	デバッグ描画
	void					DrawDebugPrimitive();														//	デバッグプリミティブ描画

	Transform* GetTransform()const { return gltfStaticModelResource_->GetTransform(); }
	Transform* GetCoverTransform()const { return coverModel_->GetTransform(); }
	const DirectX::XMFLOAT3 GetOwnerPosition()								{ return ownerPosition_; }	//	弾丸所有者の位置取得
	float					GetRadius()										{ return radius_; }			//	半径取得
	
private:
	enum EFFECT_TYPE
	{
		SHORT = 0,
		LONG,
		MAX,
	};
	Effect* effect_[EFFECT_TYPE::MAX] = {};

	enum class AUDIO_SE_BULLET	//	3Dで鳴らすSE
	{
		Explosion = 0,			//	爆発音
		Max
	};

protected:
	DirectX::XMFLOAT3							velocity_ = {};					//	速度
	DirectX::XMFLOAT3							direction_ = {};				//	弾が飛ぶ方向
	float										radius_;						//	弾の半径
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	coverPixelShader_;				//	弾丸のキューブのピクセルシェーダー
	std::shared_ptr<GltfModelStaticBatching>	gltfStaticModelResource_;		//	Gltfモデル
	std::unique_ptr<GltfModelStaticBatching>	coverModel_ = {};				//	弾の周りを覆うモデル
	bool										damaged_ = false;				//	攻撃を受けたかどうか

	DirectX::XMFLOAT3							ownerPosition_ = {};			//	弾丸所有者の位置

	std::unique_ptr<SoundEmitter> emitter_ = nullptr;							//	エミッターを自分の位置で持つ
	std::unique_ptr<AudioSource3D> se_[static_cast<int>(AUDIO_SE_BULLET::Max)];	//	弾丸のSE(3Dで鳴らす)

	bool isInvincible_ = false;	//	無敵

};

