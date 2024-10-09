#pragma once

#include <memory>
#include <string>
#include <map>

#include "Model.h"
#include "GltfModel.h"
#include "GltfModelStaticBatching.h"
#include "Sprite.h"
#include "Effect.h"

class Model;

//	���\�[�X�}�l�[�W���[
class ResourceManager
{
private:
	ResourceManager() {};
	~ResourceManager() {};
public:
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	//	Fbx���f�����\�[�X�ǂݍ���
	std::shared_ptr <Model>						LoadFbxModelResource(const char* fileName, bool triangulate = false, float samplingRate = 0);

	//	Gltf���f�����\�[�X�ǂݍ���
	std::shared_ptr <GltfModel>					LoadGltfModelResource(const std::string& fileName, const std::string& rootNodeName = "root");

	//	Gltf���f�����\�[�X�ǂݍ���
	std::shared_ptr <GltfModelStaticBatching>	LoadGltfModelStaticResource(const std::string& fileName, const bool setColor = false, const DirectX::XMFLOAT4 color = { 0,0,0,1 });

	//	�X�v���C�g���\�[�X�ǂݍ���
	std::shared_ptr <Sprite>					LoadSpriteResource(const std::string& fileName);

	//	�G�t�F�N�g���\�[�X�ǂݍ���
	std::shared_ptr <Effect>					LoadEffectResource(const char* fileName);

private:
	//	Fbx���f�����\�[�X�}�b�v
	using ModelMap = std::map<std::string, std::weak_ptr <Model>>;
	ModelMap models_;

	//	Gltf���f�����\�[�X�}�b�v
	using GltfModelMap = std::map<std::string, std::weak_ptr <GltfModel>>;
	GltfModelMap gltfModels_;

	//	Gltf���f�����\�[�X�}�b�v
	using GltfModelStaticMap = std::map<std::string, std::weak_ptr <GltfModelStaticBatching>>;
	GltfModelStaticMap gltfStaticModels_;

	//	�X�v���C�g���\�[�X�}�b�v
	using SpriteMap = std::map<std::string, std::weak_ptr<Sprite>>;
	SpriteMap sprites_;

	//	�G�t�F�N�g���\�[�X�}�b�v
	using EffectMap = std::map<std::string, std::weak_ptr <Effect>>;
	EffectMap effects_;

};