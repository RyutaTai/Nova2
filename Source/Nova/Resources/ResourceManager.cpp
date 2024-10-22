#include"ResourceManager.h"

#include "../Graphics/Graphics.h"
#include "../Others/Converter.h"

//	FBX���f�����\�[�X�ǂݍ���
std::shared_ptr<Model> ResourceManager::LoadFbxModelResource(const char* fileName, bool triangulate, float samplingRate)
{
	//	���f��������
	for (auto& modelPair : models_)
	{
		if (modelPair.first == fileName)	//	�������r
		{
			std::shared_ptr<Model> existModel = modelPair.second.lock();
			if (existModel)
			{
				return existModel;
			}
		}
	}
	//	���f���쐬�A�ǂݍ���
	std::shared_ptr<Model> model = std::make_shared<Model>(Graphics::Instance().GetDevice(), fileName);
	models_[fileName] = model;

	//	������Ȃ�����
	return model;
}

//	GLTF���f�����\�[�X�ǂݍ���
std::shared_ptr<GltfModel> ResourceManager::LoadGltfModelResource(const std::string& fileName, const std::string& rootNodeName)
{
	//	���f��������
	for (auto& gltfModelPair : gltfModels_)
	{
		if (gltfModelPair.first == fileName)	//	�������r
		{
			std::shared_ptr<GltfModel> existModel = gltfModelPair.second.lock();
			if (existModel)
			{
				return existModel;
			}
		}
	}
	//	���f���쐬�A�ǂݍ���
	std::shared_ptr<GltfModel> gltfModel = std::make_shared<GltfModel>(fileName, rootNodeName);
	gltfModels_[fileName] = gltfModel;

	//	������Ȃ�����
	return gltfModel;
}

//	GLTF���f�����\�[�X�ǂݍ���
std::shared_ptr<GltfModelStaticBatching> ResourceManager::LoadGltfModelStaticResource(const std::string& fileName, const bool setColor, const DirectX::XMFLOAT4 color)
{
	//	���f��������
	for (auto& gltfModelPair : gltfStaticModels_)
	{
		if (gltfModelPair.first == fileName)	//	�������r
		{
			std::shared_ptr<GltfModelStaticBatching> existModel = gltfModelPair.second.lock();
			if (existModel)
			{
				return existModel;
			}
		}
	}
	//	���f���쐬�A�ǂݍ���
	std::shared_ptr<GltfModelStaticBatching> gltfStaticModel = std::make_shared<GltfModelStaticBatching>(fileName, setColor, color);
	gltfStaticModels_[fileName] = gltfStaticModel;

	//	������Ȃ�����
	return gltfStaticModel;
}

//	�X�v���C�g���\�[�X�ǂݍ���
std::shared_ptr<Sprite> ResourceManager::LoadSpriteResource(const std::string& fileName)
{
	//	string����wstring�֕ϊ�
	std::wstring wFileName = ConvertStringToWstring(fileName);
	//	wstring����wchar_t*�֕ϊ�
	const wchar_t* wcharFileName = wFileName.c_str();

	//	�X�v���C�g����
	for (auto& spritePair : sprites_)
	{
		if (spritePair.first == fileName)	//	�������r
		{
			std::shared_ptr<Sprite> existSprite = spritePair.second.lock();
			if (existSprite)
			{
				return existSprite;
			}
		}
	}
	//	���f���쐬�A�ǂݍ���
	std::shared_ptr<Sprite> sprite = std::make_shared<Sprite>(wcharFileName);
	sprites_[fileName] = sprite;

	//	������Ȃ�����
	return sprite;
}

//	�G�t�F�N�g���\�[�X�ǂݍ���
std::shared_ptr<Effect> ResourceManager::LoadEffectResource(const char* fileName)
{
	//	�G�t�F�N�g������
	for (auto& effectPair : effects_)
	{
		if (effectPair.first == fileName)	//	�������r
		{
			std::shared_ptr<Effect> existEffect = effectPair.second.lock();
			if (existEffect)
			{
				return existEffect;
			}
		}
	}
	//	���f���쐬�A�ǂݍ���
	std::shared_ptr<Effect> effect = std::make_shared<Effect>(fileName);
	effects_[fileName] = effect;

	//	������Ȃ�����
	return effect;
}