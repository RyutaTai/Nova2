#pragma once

#include "../Nova/Resources/GltfModelStaticBatching.h"
#include "../Nova/Resources/Effect.h"
#include "../Game/BulletManager.h"
#include "../Nova/Audio/AudioSource3D.h"
#include "../Nova/Audio/AudioSource.h"

//	弾丸クラス
class Bullet
{
public:
	enum class OpponentType	//	攻撃する相手の種類
	{
		Player,	//	プレイヤー
		Enemy,	//	敵
		None,	//	無し
		All,	//	全て
		Max
	};

public:
	Bullet(const std::string& filename);
	virtual ~Bullet() {}

	virtual void			Initialize();
	virtual void			Update(const float& elapsedTime);
	virtual void			Launch(const DirectX::XMFLOAT3& direction = {}, const DirectX::XMFLOAT3& position = {});
	virtual void			Render()			= 0;
	virtual void			RnederCoverModel()	= 0;
	
	//	----- デバッグ描画 -----
	virtual void			DrawDebug();																//	デバッグ描画
	void					DrawDebugPrimitive();														//	デバッグプリミティブ描画

	//	----- 弾丸の所有者 -----
	void					SetOwnerPosition(const DirectX::XMFLOAT3& pos)	{ ownerPosition_ = pos; }	//	弾丸所有者の位置設定
	const DirectX::XMFLOAT3 GetOwnerPosition()								{ return ownerPosition_; }	//	弾丸所有者の位置取得
	
	//	----- カバーモデル -----
	virtual void			CoverModelUpdate(const float& elapsedTime);									//	カバーモデル更新処理
	Transform*				GetCoverTransform()const { return coverModel_->GetTransform(); }
	
	//	----- オーディオ -----
	void					UpdateEmitter();
	void					UpdateAudioSource();	//	オーディオソース更新処理
	
	virtual void			Destroy(const float& elapsedTime);											//	破棄処理
	void					SetInvincible(const bool& isInvincible)			{ isInvincible_ = isInvincible; }
	void					SetDamaged(const bool& damaged)						{ isDamaged_ = damaged; }		//	ダメージフラグ設定

	Transform* GetTransform()const { return gltfStaticModelResource_->GetTransform(); }
	float		GetRadius()										{ return radius_; }			//	半径取得
	
	//	----- 攻撃力 -----
	void SetAttackPower(const float& attackPower) { attackPower_ = attackPower; }
	const float GetAttackPower()const { return attackPower_; }

	//	----- 攻撃する相手 -----
	void SetOpponentType(const OpponentType& opponentType) { opponentType_ = opponentType; }
	const OpponentType GetOpponentType()const { return opponentType_; }

private:
	enum class Audio3D	//	3Dで鳴らすSEの種類
	{
		Explosion = 0,			//	爆発音
		Move,					//	移動音
		Max
	};

protected:
	DirectX::XMFLOAT3							velocity_ = {};					//	速度
	DirectX::XMFLOAT3							direction_ = {};				//	弾が飛ぶ方向
	float										radius_ = 1.0f;					//	弾の半径
	
	std::shared_ptr<GltfModelStaticBatching>	gltfStaticModelResource_;		//	Gltfモデル
	std::unique_ptr<GltfModelStaticBatching>	coverModel_ = {};				//	弾の周りを覆うモデル
	
	//	----- ダメージ -----
	bool										isDamaged_ = false;				//	攻撃を受けたかどうか
	float										attackPower_ = 5.0f;			//	攻撃力

	//	----- 弾丸の所有者 -----
	DirectX::XMFLOAT3							ownerPosition_ = {};			//	弾丸所有者の位置

	//	----- カバーモデル -----
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	coverPixelShader_;				//	弾丸のキューブのピクセルシェーダー
	
	//	----- オーディオ -----
	SoundEmitter emitter_ = {};							//	エミッターを自分の位置で持つ
	AudioSource3D* se_[static_cast<int>(Audio3D::Max)] = { nullptr };	//	弾丸のSE(3Dで鳴らす)
	AudioSource* debugSE_ = nullptr;

	bool isInvincible_ = false;	//	無敵

	//	----- 攻撃する相手 -----
	OpponentType opponentType_;

};

