#include "BulletManager.h"

//	コンストラクタ
BulletManager::BulletManager()
{
	
}

//	デストラクタ
BulletManager::~BulletManager()
{
	Clear();
}

//	初期化処理
void BulletManager::Initialize()
{
	for (Bullet* bullet : bullets_)
	{
		bullet->Initialize();
	}
}

//	更新処理
void BulletManager::Update(const float& elapsedTime)
{
	//	更新処理
	for (Bullet* bullet : bullets_)
	{
		bullet->Update(elapsedTime);
	}

	//	破棄処理
	//	※bulletsの範囲for文内でerase()すると不具合が発生するため,
	//	更新処理が終わった後に、破棄リストに積まれたオブジェクトを削除する
	for (Bullet* remove : removes_)
	{
		//	std::vectorから要素を破棄するときはイテレーターで削除しなければならない
		//	std::vectorで管理されている要素を削除するにはerase()関数を使用する
		//	破棄リストのポインタからイテレーターを検索し、erase()に渡す
		std::vector	<Bullet*>::iterator it =
			std::find(bullets_.begin(), bullets_.end(), remove);

		if (it != bullets_.end())
		{
			bullets_.erase(it);
		}

		//	弾丸処理
		delete remove;
	}
	//	破棄リストをクリア
	removes_.clear();
}

//	カバーモデル更新処理
void BulletManager::CoverModelUpdate(const float& elapsedTime)
{
	//	スケール設定(BGMに合うようにしたい)今は仮で一定周期で変化させる
	if (isCoverModelUpdate_)
	{
		//	スケール加算
#if 0
		coverScale_ += scaleAdd_ * elapsedTime;
		if (coverScale_ > COVER_SCALE_MAX)
		{
			coverScale_ = COVER_SCALE_MIN;
			scaleAdd_ = -scaleAdd_;
		}
		if (coverScale_ < COVER_SCALE_MIN)
		{
			coverScale_ = COVER_SCALE_MIN;
			scaleAdd_ = -scaleAdd_;
		}
#endif

		//	スケール減算
#if 1
		coverScale_ -= scaleAdd_ * elapsedTime;
		if (coverScale_ > COVER_SCALE_MAX)
		{
			coverScale_ = COVER_SCALE_MAX;
			scaleAdd_ = -scaleAdd_;
		}
		if (coverScale_ < COVER_SCALE_MIN)
		{
			coverScale_ = COVER_SCALE_MAX;
			scaleAdd_ = -scaleAdd_;
		}
#endif

	}

	for (Bullet* bullet : bullets_)
	{
		bullet->CoverModelUpdate(elapsedTime);
	}

}

//	描画処理
void BulletManager::Render()
{
	//	弾丸描画
	for (Bullet* bullet : bullets_)
	{
		bullet->Render();
	}
	//	弾丸カバーモデル描画
	if (coverModelDraw_)
	{
		for (Bullet* bullet : bullets_)	//	透明処理は最後に呼ぶ(この後にモデルを描画しても、透明が優先されてしまう)
		{
			bullet->RnederCoverModel();
		}
	}
}

//	弾丸登録
void BulletManager::Register(Bullet* bullet)
{
	bullets_.emplace_back(bullet);
}

//	弾丸全削除
void BulletManager::Clear()
{
	for (Bullet* bullet : bullets_)
	{
		delete bullet;
	}
	bullets_.clear();
}

//	弾丸削除
void BulletManager::Remove(Bullet* bullet)
{
	//	破棄リストに追加
	//	直接projectilesの要素を削除すると、範囲for文内で不具合を起こすため破棄リストに追加する
	removes_.insert(bullet);
}

//	デバッグプリミティブ描画
void BulletManager::DrawDebugPrimitive()
{
	for (Bullet* bullet : bullets_)
	{
		bullet->DrawDebugPrimitive();
	}
}

//	デバッグ描画
void BulletManager::DrawDebug()
{
	if (ImGui::TreeNode(u8"BulletManager"))
	{
		ImGui::Checkbox("Bullet Destroy ", &isBulletDestroy_);						//	弾丸破棄
		ImGui::Checkbox("CoverDraw ", &coverModelDraw_);							//	カバーモデル描画
		ImGui::Checkbox("IsCoverModelUpdate", &isCoverModelUpdate_);				//	カバーモデル更新処理(デフォルトはtrue)
		ImGui::DragFloat("ScaleAdd", &scaleAdd_, 1.0f, -FLT_MAX, FLT_MAX);			//	カバースケールの増加量
		ImGui::DragFloat("CoverScale", &coverScale_, 1.0f, -FLT_MAX, FLT_MAX);		//	半径
		ImGui::DragFloat("CoverScaleTimer", &coverScaleTimer_, 1.0f, -FLT_MAX, FLT_MAX);
		ImGui::TreePop();
	}

	if (isBulletDestroy_)	//	弾丸破棄
	{
		Clear();
	}

	for (Bullet* bullet : bullets_)
	{
		bullet->DrawDebug();
	}
}