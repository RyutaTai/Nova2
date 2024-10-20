#include"ResourceManager.h"

#include "../Graphics/Graphics.h"
#include "../Others/Converter.h"

//	FBXモデルリソース読み込み
std::shared_ptr<Model> ResourceManager::LoadFbxModelResource(const char* fileName, bool triangulate, float samplingRate)
{
	//	モデルを検索
	for (auto& modelPair : models_)
	{
		if (modelPair.first == fileName)	//	文字列比較
		{
			std::shared_ptr<Model> existModel = modelPair.second.lock();
			if (existModel)
			{
				return existModel;
			}
		}
	}
	//	モデル作成、読み込み
	std::shared_ptr<Model> model = std::make_shared<Model>(Graphics::Instance().GetDevice(), fileName);
	models_[fileName] = model;

	//	見つからなかった
	return model;
}

//	GLTFモデルリソース読み込み
std::shared_ptr<GltfModel> ResourceManager::LoadGltfModelResource(const std::string& fileName, const std::string& rootNodeName)
{
	//	モデルを検索
	for (auto& gltfModelPair : gltfModels_)
	{
		if (gltfModelPair.first == fileName)	//	文字列比較
		{
			std::shared_ptr<GltfModel> existModel = gltfModelPair.second.lock();
			if (existModel)
			{
				return existModel;
			}
		}
	}
	//	モデル作成、読み込み
	std::shared_ptr<GltfModel> gltfModel = std::make_shared<GltfModel>(Graphics::Instance().GetDevice(), fileName, rootNodeName);
	gltfModels_[fileName] = gltfModel;

	//	見つからなかった
	return gltfModel;
}

//	GLTFモデルリソース読み込み
std::shared_ptr<GltfModelStaticBatching> ResourceManager::LoadGltfModelStaticResource(const std::string& fileName, const bool setColor, const DirectX::XMFLOAT4 color)
{
	//	モデルを検索
	for (auto& gltfModelPair : gltfStaticModels_)
	{
		if (gltfModelPair.first == fileName)	//	文字列比較
		{
			std::shared_ptr<GltfModelStaticBatching> existModel = gltfModelPair.second.lock();
			if (existModel)
			{
				return existModel;
			}
		}
	}
	//	モデル作成、読み込み
	std::shared_ptr<GltfModelStaticBatching> gltfStaticModel = std::make_shared<GltfModelStaticBatching>(Graphics::Instance().GetDevice(), fileName, setColor, color);
	gltfStaticModels_[fileName] = gltfStaticModel;

	//	見つからなかった
	return gltfStaticModel;
}

//	スプライトリソース読み込み
std::shared_ptr<Sprite> ResourceManager::LoadSpriteResource(const std::string& fileName)
{
	//	stringからwstringへ変換
	std::wstring wFileName = ConvertStringToWstring(fileName);
	//	wstringからwchar_t*へ変換
	const wchar_t* wcharFileName = wFileName.c_str();

	//	スプライト検索
	for (auto& spritePair : sprites_)
	{
		if (spritePair.first == fileName)	//	文字列比較
		{
			std::shared_ptr<Sprite> existSprite = spritePair.second.lock();
			if (existSprite)
			{
				return existSprite;
			}
		}
	}
	//	モデル作成、読み込み
	std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(wcharFileName);
	sprites_[fileName] = sprite;

	//	見つからなかった
	return sprite;
}

//	エフェクトリソース読み込み
std::shared_ptr<Effect> ResourceManager::LoadEffectResource(const char* fileName)
{
	//	エフェクトを検索
	for (auto& effectPair : effects_)
	{
		if (effectPair.first == fileName)	//	文字列比較
		{
			std::shared_ptr<Effect> existEffect = effectPair.second.lock();
			if (existEffect)
			{
				return existEffect;
			}
		}
	}
	//	モデル作成、読み込み
	std::shared_ptr<Effect> effect = std::make_shared<Effect>(fileName);
	effects_[fileName] = effect;

	//	見つからなかった
	return effect;
}