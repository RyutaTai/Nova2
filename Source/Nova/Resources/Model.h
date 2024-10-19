#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include "../Collision/Raycast.h"
#include "../Resources/ResourceManager.h"

//namespace DirectX
//{
//	template<class T>
//	void serialize(T& archive, DirectX::XMFLOAT2& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y)
//		);
//	}
//
//	template<class T>
//	void serialize(T& archive, DirectX::XMFLOAT3& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y),
//			cereal::make_nvp("z", v.z)
//		);
//	}
//
//	template<class T>
//	void serialize(T& archive, DirectX::XMFLOAT4& v)
//	{
//		archive(
//			cereal::make_nvp("x", v.x),
//			cereal::make_nvp("y", v.y),
//			cereal::make_nvp("z", v.z),
//			cereal::make_nvp("w", v.w)
//		);
//	}
//
//	template<class T>
//	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
//	{
//		archive(
//			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
//			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
//			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
//			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
//			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
//			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
//			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
//			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
//		);
//	}
//}

struct Skeleton
{
	struct Bone
	{
		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID_, name_, parentIndex_,nodeIndex_, offsetTransform_);
		}

		uint64_t uniqueID_{ 0 };
		std::string name_;
		// 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
		int64_t parentIndex_{ -1 };//-1 : the bone is orphan
		// 'node_index' is an index that refers to the node array of the scene.
		int64_t nodeIndex_{ 0 };

		// 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
		DirectX::XMFLOAT4X4 offsetTransform_{ 1, 0, 0, 0,
												0, 1, 0, 0,
												0, 0, 1, 0,
												0, 0, 0, 1 };

		bool isOrphan()const { return parentIndex_ < 0; }
	};

	template<class T>
	void serialize(T& archive)
	{
		archive(bones_);
	}

	std::vector<Bone>bones_;
	int64_t indexof(uint64_t uniqueID)const
	{
		int64_t index{ 0 };
		for (const Bone& bone : bones_)
		{
			if (bone.uniqueID_ == uniqueID)
			{
				return index;
			}
			++index;
		}
		return -1;
	}
};

struct Animation
{
	std::string name_;
	float		samplingRate_{ 0 };

	struct Keyframe
	{
		struct Node
		{
			template <class T>
			void serialize(T& archive)
			{
				archive(globalTransform_, scaling_, rotation_, translation_);
			}

			// 'global_transform' is used to convert from local space of node to global space of scene.
			DirectX::XMFLOAT4X4 globalTransform_ { 1, 0, 0, 0, 
													0, 1, 0, 0,
													0, 0, 1, 0,
													0, 0, 0, 1 };

			//The transform data of a node includes its translation,rotation and scaling vectors
			//with respect to its parent
			DirectX::XMFLOAT3 scaling_{ 1,1,1 };
			DirectX::XMFLOAT4 rotation_{ 0,0,0,1 };//Rotaion quaternion
			DirectX::XMFLOAT3 translation_{ 0,0,0 };
		};
		std::vector<Node>nodes_;

		template<class T>
		void serialize(T& archive)
		{
			archive(nodes_);
		}
	};
	std::vector<Keyframe>sequence_;

	template<class T>
	void serialize(T& archive)
	{
		archive(name_, samplingRate_, sequence_);
	}
};

struct SceneStruct
{
	struct Node
	{
		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID_, name_, attribute_, parentIndex_);
		}

		uint64_t	uniqueID_{ 0 };
		std::string name_;
		FbxNodeAttribute::EType attribute_{ FbxNodeAttribute::EType::eUnknown };
		int64_t		parentIndex_{ -1 };
	};
	std::vector	<Node>	nodes_;

	template<class T>
	void serialize(T& archive)
	{
		archive(nodes_);
	}

	int64_t indexof(uint64_t unique_id)const
	{
		int64_t index{ 0 };
		for (const Node& node : nodes_)
		{
			if (node.uniqueID_ == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}
};

class Model
{
public:
	static const int MAX_BONE_INFLUENCES{ 4 };
	struct Vertex
	{
		template<class T>
		void serialize(T& archive)
		{
			archive(position_, normal_, tangent_, texcoord_,boneWeights_, boneIndices_);
		}

		DirectX::XMFLOAT3 position_;
		DirectX::XMFLOAT3 normal_;
		DirectX::XMFLOAT4 tangent_;
		DirectX::XMFLOAT2 texcoord_;
		float boneWeights_[MAX_BONE_INFLUENCES]{ 1,0,0,0 };
		uint32_t boneIndices_[MAX_BONE_INFLUENCES]{};
	};
	static const int MAX_BONES{ 256 };

	struct Constants
	{
		DirectX::XMFLOAT4X4 world_;
		DirectX::XMFLOAT4   materialColor_;
		DirectX::XMFLOAT4X4 boneTransforms_[MAX_BONES]{ 
			{ 1, 0, 0, 0,
			  0, 1, 0, 0,
			  0, 0, 1, 0,
			  0, 0, 0, 1 } };
	};

	struct Mesh
	{
		uint64_t	uniqueID_{ 0 };
		std::string name_;
		// 'node_index'is an index that refers to the node array of the scene.
		int64_t		nodeIndex_{ 0 };
		std::vector	<Vertex>	vertices_;
		std::vector	<uint32_t>	indices_;
		DirectX::XMFLOAT4X4		defaultGlobalTransform_{ 
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1 };
		struct Subset
		{
			template<class T>
			void serialize(T& archive)
			{
				archive(materialUniqueID_, materialName_, startIndexLocation_, indexCount_);
			}

			uint64_t	materialUniqueID_{ 0 };
			std::string materialName_;

			uint32_t	startIndexLocation_{ 0 };
			uint32_t	indexCount_{ 0 };
		}; std::vector<Subset>subsets_;
		// RAYCAST
		const Subset* findSubset(uint32_t index) const
		{
			for (const Subset& subset : subsets_)
			{
				if (subset.startIndexLocation_ <= index && subset.startIndexLocation_ + subset.indexCount_ > index)
				{
					return &subset;
				}
			}
			return nullptr;
		}

		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID_, name_, nodeIndex_, subsets_, defaultGlobalTransform_,
				bindPose_, boundingBox_, vertices_, indices_);
		}
		Skeleton bindPose_;

	private:
		Microsoft::WRL::ComPtr <ID3D11Buffer> vertexBuffer_;
		Microsoft::WRL::ComPtr <ID3D11Buffer> indexBuffer_;
		friend class Model;
		DirectX::XMFLOAT3 boundingBox_[2]
		{
			{+D3D11_FLOAT32_MAX,+D3D11_FLOAT32_MAX,+D3D11_FLOAT32_MAX},
			{-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX,-D3D11_FLOAT32_MAX}
		};
	};
	std::vector <Mesh> meshes_;

	struct Material
	{
		template<class T>
		void serialize(T& archive)
		{
			archive(uniqueID_, name_, Ka_, Kd_, Ks_, textureFileNames_);
		}

		uint64_t uniqueID_{ 0 };
		std::string name_;

		DirectX::XMFLOAT4 Ka_{ 0.2f,0.2f,0.2f,1.0f };	//	�A���r�G���g(����)
		DirectX::XMFLOAT4 Kd_{ 0.8f,0.8f,0.8f,1.0f };	//	�f�B�t���[�Y(�g�U���ˌ�)
		DirectX::XMFLOAT4 Ks_{ 1.0f,1.0f,1.0f,1.0f };	//	�X�y�L����(���ʔ��ˌ�)

		std::string											textureFileNames_[5];
		Microsoft::WRL::ComPtr <ID3D11ShaderResourceView>	shaderResourceViews_[5];
	};
	std::unordered_map			<uint64_t, Material>		materials_;
	std::vector					<Animation>					animationClips_;

public:
	Model(ID3D11Device* device, const char* fbxFileName, bool triangulate = false, float SamplingRate = 0, bool usedAsCollider = false/*RAYCAST*/);
	virtual ~Model() = default;
	void Render(const DirectX::XMMATRIX& world, const DirectX::XMFLOAT4& materialColor, 
		const Animation::Keyframe* keyframe, ID3D11PixelShader** replacementPixelShader);

	void	DrawDebug();

	void	UpdateAnimation			(Animation::Keyframe& keyframe);
	void	UpdateAnimation			(const float& elapsedTime);
	bool	UpdateBlendAnimation	(const float& elapsedTime);
	void	UpdateBlendRate			(float blendRate,				const float& elasedTime);
	bool	AppendAnimations		(const char* animationFileName, float samplingRate);
	void	BlendAnimations			(const Animation::Keyframe* keyframes[2],	float factor,		Animation::Keyframe& keyframe);
	void	PlayAnimation			(int index,						bool loop,	float speed = 1.0f, float blendSeconds = 1.0f);
	void	PlayBlendAnimation		(int index1,					int index2, bool loop,			float speed = 1.0f);
	bool	IsPlayAnimation()const;

	//	RAYCAST
	//	The coordinate system of all function arguments is world space.
	bool Raycast(const DirectX::XMFLOAT3& position/*ray position*/, const DirectX::XMFLOAT3& direction/*ray direction*/, const DirectX::XMFLOAT4X4& worldTransform, DirectX::XMFLOAT4& closestPoint, DirectX::XMFLOAT3& intersectedNormal,
		std::string& intersectedMesh, std::string& intersectedMaterial);

	void SetCurrentAnimIndex(int currentAnimIndex)		{ currentAnimationIndex_ = currentAnimIndex; }
	void SetPixelShader(ID3D11PixelShader* pixelShader) { pixelShader_ = pixelShader; }		//	PixelShader�ݒ�
	void SetBlendRate(float blendRate)					{ blendRate_ = blendRate; }			//	�u�����h���ݒ�
	void SetWeight(float weight)						{ weight_ = weight; }				//	�u�����h��weight�l�ݒ�

	Animation::Keyframe& GetKeyFrame()			{ return keyframe_; }						//	�L�[�t���[���擾
	int		GetCurrentAnimIndex()		const	{ return currentAnimationIndex_; }			//	���݂̃A�j���[�V�����ԍ�
	int		GetCurrentBlendAnimIndex1()	const	{ return blendAnimationIndex1_; }			//	���݂̃u�����h�A�j���[�V����1
	int		GetCurrentBlendAnimIndex2()	const	{ return blendAnimationIndex2_; }			//	���݂̃u�����h�A�j���[�V����2
	float	GetCurrentAnimationSeconds()const	{ return currentAnimationSeconds_; }		//	���݂̃A�j���[�V�����Đ����Ԏ擾
	float	GetWeight()					const	{ return weight_; }							//	�u�����h��weight�擾
	float	GetBlendRate()				const	{ return blendRate_; }						//	�u�����h���擾
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const Animation::Keyframe* keyframe, const DirectX::XMFLOAT4X4& transform);	//	�W���C���g�|�W�V�����擾
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform);											//	�W���C���g�|�W�V�����擾
	DirectX::XMFLOAT3 GetJointPosition(size_t meshIndex, size_t boneIndex, const Animation::Keyframe* keyframe, const DirectX::XMFLOAT4X4& transform);							//	�W���C���g�|�W�V�����擾
	
private:
	void CreateComObjects(ID3D11Device* device, const char* fbxFileName, bool usedAsCollider/*RAYCAST*/);
	void FetchSkeleton	(FbxMesh* fbxMesh,   Skeleton& bindPose);
	void FetchMaterials	(FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);
	void FetchMeshes	(FbxScene* fbxScene, std::vector<Mesh>& meshes);
	void FetchAnimations(FbxScene* fbxScene, std::vector<Animation>& animationClips,
		float samplingRate /*If this value is 0, the animation data will be sampled at the default frame rate.*/);

public: 
	// �f�o�b�O�m�F�p
	bool isDebugBlendAnimation_ = true;		//	�u�����h�A�j���[�V�����@�I��/�I�t

protected:
	SceneStruct			sceneView_;
	//float animationInterval_;

private:
	Microsoft::WRL::ComPtr <ID3D11VertexShader>	vertexShader_;
	Microsoft::WRL::ComPtr <ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr <ID3D11InputLayout>	inputLayout_;
	Microsoft::WRL::ComPtr <ID3D11Buffer>		constantBufer_;

private:
	Animation::Keyframe		keyframe_ = {};						//	�A�j���[�V�����L�[�t���[��
	int						currentAnimationIndex_ = -1;				//	���݂̃A�j���[�V�����ԍ�
	float					currentAnimationSeconds_;			//	���݂̃A�j���[�V�����Đ�����
	int						currentKeyframeIndex_;				//	���݉��t���[���ڂ�
	float					animationSpeed_;					//	�A�j���[�V�����Đ����x
	bool					animationLoopFlag_;					//	�A�j���[�V���������[�v�Đ��t���O
	bool					animationEndFlag_;					//	�A�j���[�V�����Đ��I���t���O

	float					weight_					= 0.5f;		//	�u�����h�̏d��
	float					blendRate_				= 0.5f;		//	�u�����h��
	float					blendThreshold_			= 0.0f;		//	�A�j���[�V�����؂�ւ��̂������l
	float					animationBlendTime_		= 0.0f;		//	�u�����h���Ă��獡�܂ŉ��b�o�߂�����
	float					animationBlendSeconds_	= 0.0f;		//	���݂̎p�����玟�̃A�j���[�V�����̎p���܂łǂ̂��炢���Ԃ������邩
	int						blendAnimationIndex1_	= -1;		//	�u�����h������A�j���[�V�����ԍ�1(�u�����h��)
	int						blendAnimationIndex2_	= -1;		//	�u�����h������A�j���[�V�����ԍ�2(�u�����h��)

};

struct BoneInfluence
{
	uint32_t boneIndex_;
	float boneWeight_;
};
using BoneInfluencesPerControlPoint = std::vector<BoneInfluence>;

void FetchBoneInfluences(const FbxMesh* fbxMesh,
	std::vector<BoneInfluencesPerControlPoint>& boneInfluences);