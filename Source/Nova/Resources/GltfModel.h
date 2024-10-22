#pragma once 

#define NOMINMAX
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <unordered_map>

#include "../../tinygltf-release/tiny_gltf.h"

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>
#include "../Others/Transform.h"

namespace DirectX
{
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

class GltfModel
{
public:
	struct Scene
	{
		std::string			name_;
		std::vector<int>	nodes_; // Array of 'root' nodes 

		template<class T>
		void serialize(T& archive)
		{
			archive(name_, nodes_);
		}

	};
	std::vector<Scene>		scenes_;

	struct Node
	{
		std::string name_;
		int skin_ = -1;  // index of skin referenced by this node 
		int mesh_ = -1;  // index of mesh referenced by this node 

		std::vector<int> children_; // An array of indices of child nodes of this node 

		// Local transforms 
		DirectX::XMFLOAT4 rotation_				{ 0, 0, 0, 1 };
		DirectX::XMFLOAT3 scale_				{ 1, 1, 1 };
		DirectX::XMFLOAT3 translation_			{ 0, 0, 0 };

		DirectX::XMFLOAT4X4 globalTransform_=
		{ 
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1
		};

		bool isRoot_ = false;	//	���[�g�m�[�h���ǂ���

		template<class T>
		void serialize(T& archive)
		{
			archive(name_, skin_, mesh_, children_, rotation_, scale_, translation_, globalTransform_, isRoot_);
		}

	};
	std::vector<Node> nodes_;

	struct BufferView
	{
		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
		size_t strideInBytes_ = 0;
		size_t sizeInBytes_ = 0;
		size_t count()const
		{
			return sizeInBytes_ / strideInBytes_;
		}

		std::vector<UINT8> verticesBinary_;	//	�V���A�����ł��Ȃ����̂�ێ����邽��

		template<class T>
		void serialize(T& archive)
		{
			archive(format_, strideInBytes_, sizeInBytes_, verticesBinary_);
		}

	};

	struct Mesh
	{
		std::string name_;
		struct Primitive
		{
			int material_;
			std::map<std::string, BufferView> vertexBufferViews_;
			BufferView indexBufferView_;

			template<class T>
			void serialize(T& archive)
			{
				archive(material_, vertexBufferViews_, indexBufferView_);
			}
		};
		std::vector<Primitive> primitives_;

		template<class T>
		void serialize(T& archive)
		{
			archive(name_, primitives_);
		}
	};
	std::vector<Mesh> meshes_;

	struct TextureInfo
	{
		int index_		= -1;
		int texcoord_	=  0;

		template<class T>
		void serialize(T& archive)
		{
			archive(index_, texcoord_);
		}
	};

	struct NormalTextureInfo
	{
		int		index_		= -1;
		int		texcoord_	=  0;
		float	scale_		=  1.0f;

		template<class T>
		void serialize(T& archive)
		{
			archive(index_, texcoord_, scale_);
		}
	};

	struct OcclusionTextureInfo
	{
		int		index_		= -1;
		int		texcoord_	=  0;
		float	strength_	=  1.0f;

		template<class T>
		void serialize(T& archive)
		{
			archive(index_, texcoord_, strength_);
		}
	};

	struct PbrMetallicRoughness
	{
		float		baseColorFactor_[4] = { 1.0f, 1.0f, 1.0f,1.0f };
		TextureInfo baseColorTexture_;
		float		metallicFactor_		=  1.0f;
		float		roughnessFactor_	= -1.0f;
		TextureInfo metallicRoughnessTexture_;

		template<class T>
		void serialize(T& archive)
		{
			archive(baseColorFactor_, baseColorTexture_, metallicFactor_, roughnessFactor_, metallicRoughnessTexture_);
		}
	};

	struct Material
	{
		std::string name_; 

		struct Cbuffer
		{
			float	emissiveFactor_[3]	= { 0.0f, 0.0f, 0.0f };
			int		alphaMode_			= 0;	//	"OPAQUE" : 0,"MASK" : 1,"BLEND" : 2
			float	alphaCutOff_		= 0.5f;
			bool	doubleSided_		= false;
			
			PbrMetallicRoughness	pbrMetallicRoughness_;

			NormalTextureInfo		normalTexture_;
			OcclusionTextureInfo	occlusionTexture_;
			TextureInfo				emissiveTexture_;

			template<class T>
			void serialize(T& archive)
			{
				archive(emissiveFactor_, alphaMode_, alphaCutOff_, doubleSided_, pbrMetallicRoughness_, normalTexture_, occlusionTexture_, emissiveTexture_);
			}
		};
		Cbuffer data_;

		template<class T>
		void serialize(T& archive)
		{
			archive(name_, data_);
		}
	};
	std::vector<Material> materials_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> materialResourceView_;

	struct Texture
	{
		std::string name_;
		int			source_ = -1;

		template<class T>
		void serialize(T& archive)
		{
			archive(name_, source_);
		}
	};
	std::vector<Texture> textures_;

	struct Image
	{
		std::string		name_;
		std::wstring	filename_;
		int				width_		= -1;
		int				height_		= -1;
		int				component_	= -1;
		int				bits_		= -1;
		int				pixelType_	= -1;
		int				bufferView_ = 0;
		std::string		mimeType_;
		std::string		uri_;
		bool			asIs_		= false;

		template<class T>
		void serialize(T& archive)
		{
			archive(name_, width_, height_, component_, bits_, pixelType_, bufferView_, mimeType_, uri_, asIs_, filename_);
		}
	};
	std::vector<Image> images_;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureResourceViews_;

	struct Skin
	{
		std::vector<DirectX::XMFLOAT4X4> inverseBindMatrices_;
		std::vector<int> joints_;

		template<class T>
		void serialize(T& archive)
		{
			archive(inverseBindMatrices_, joints_);
		}
	};
	std::vector<Skin> skins_;

	struct Animation
	{
		std::string name_;
		float		duration_ = 0.0f;
		struct Channel
		{
			int			sampler_	= -1;
			int			targetNode_ = -1;
			std::string targetPath_;

			template<class T>
			void serialize(T& archive)
			{
				archive(sampler_, targetNode_, targetPath_);
			}
		};
		std::vector<Channel> channels_;
	
		struct Sampler
		{
			int			input_	= -1;
			int			output_	= -1;
			std::string interpolation_;

			template<class T>
			void serialize(T& archive)
			{
				archive(input_, output_, interpolation_);
			}
		};
		std::vector<Sampler> samplers_;

		std::unordered_map<int/*sampler.input*/,	std::vector<float>>				timelines_;
		std::unordered_map<int/*sampler.output*/,	std::vector<DirectX::XMFLOAT3>> scales_;
		std::unordered_map<int/*sampler.output*/,	std::vector<DirectX::XMFLOAT4>> rotations_;
		std::unordered_map<int/*sampler.output*/,	std::vector<DirectX::XMFLOAT3>> translations_;
	
		template<class T>
		void serialize(T& archive)
		{
			archive(name_, duration_, channels_, samplers_, timelines_, scales_, rotations_, translations_);
		}
	};
	std::vector<Animation> animations_;

	struct PrimitiveConstants
	{
		DirectX::XMFLOAT4X4 world_;
		int					material_	= -1;
		int					hasTangent_ = 0 ;
		int					skin_		= -1;
		int					pad_;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitiveCbuffer_;

	static const size_t PRIMITIVE_MAX_JOINTS = 512;
	struct PrimitiveJointConstants
	{
		DirectX::XMFLOAT4X4 matrices_[PRIMITIVE_MAX_JOINTS];
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitiveJointCbuffer_;

public:
	GltfModel(const std::string& filename, const std::string& rootNodename = "root");
	virtual ~GltfModel() = default;

	void Render(const DirectX::XMMATRIX& world/*, const std::vector<Node>& animatedNodes*/);
	void DrawDebug();

	void PlayAnimation(const int& index, const bool& loop = false, const float& speed = 1.0f, const float& blendTime = 1.0f, const float& startFrame = 0.0f);
	void UpdateAnimation(const float& elapsedTime);

	void Animate(size_t animationIndex, float time, std::vector<Node>& animatedNodes);
	void AppendAnimation(ID3D11Device* device, const std::string& filename);
	void BlendAnimations(const std::vector<Node>& fromNodes, const std::vector<Node>& toNodes, float factor, std::vector<Node>& outNodes);
	bool IsPlayAnimation()const;

	void SetPixelShader(ID3D11PixelShader* pixelShader) { pixelShader_ = pixelShader; }		//	PixelShader�ݒ�
	
	Transform* GetTransform() { return &transform_; }
	int	GetCurrentAnimNum();	//	���ݍĐ����̃A�j���[�V�����ԍ�
	float const GetCurrentAnimationSeconds() { return currentAnimationSeconds_; }	//	���݂̃A�j���[�V�����Đ����Ԏ擾
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform);		//	�W���C���g�|�W�V�����擾
	DirectX::XMFLOAT3 GetJointPosition(size_t nodeIndex, const DirectX::XMFLOAT4X4& transform);
	const int GetNodeIndex(const std::string& nodeName);
	std::vector<Node>* GetNodes() { return &nodes_; }

	//	���[�g���[�V����
	void RootMotion(const float& scaleFactor);

	void SetRootJointIndex(const int& index) { rootJointIndex_ = index; }
	void SetUseRootMotion(bool useRootMotion);

private:
	void FetchNodes(const tinygltf::Model& gltfModel, const std::string& rootNodeName);
	void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltfModel);
	void FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltfModel, const bool setColor = false, const DirectX::XMFLOAT4 color = { 0,0,0,1 });
	void FetchTextures(ID3D11Device* device, const tinygltf::Model& gltfModel);
	void FetchAnimations(const tinygltf::Model& gltfModel);
	void CumulateTransforms(std::vector<Node>& nodes);
	BufferView MakeBufferView(const tinygltf::Accessor& accessor);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;

	std::string filename_;

	Transform				transform_ = {};

	// --- GLTF_ANIMATION ---
	bool	isAnimationLoop_			= false;	//	�A�j���[�V�������[�v�t���O
	int		currentAnimationIndex_		= -1;		//	�A�j���[�V�����ԍ�
	float	animationSpeed_				= 0.0f;		//	�Đ����x
	float   currentAnimationSeconds_	= 0.0f;		//	���݂̃A�j���[�V�����Đ�����
	bool	useRootMotionMovement_		= false;	//	���[�g���[�V�����̈ړ��l���g����
	std::vector<GltfModel::Node> animatedNodes_[2];
	std::vector<GltfModel::Node> blendedAnimatedNodes_;
	int		animationClip_ = 0;
	float	factor_ = 0.0f;
	int		transitionState_ = 0;
	float	transitionTime_ = 1.0f;			//	�ǂꂭ�炢���Ԃ������ău�����h���邩
	bool	isTransition_ = false;			//	�A�j���[�V�����J�ڒ����ǂ���
	bool	animationEndFlag_ = false;		//	�A�j���[�V�����Đ����I����Ă��邩�ǂ���

	//	���[�g���[�V����
	std::vector<GltfModel::Node> initAnimatedNode_;
	DirectX::XMFLOAT3	lastPosition_			= {};
	int					rootJointIndex_			= 1;
	float				rootMotionSpeed_		= 1.0f;
	bool				isFirstTimeRootMotion_	= false;	//	���[�g���[�V�������񂩂ǂ���

};

bool NullLoadImageData(tinygltf::Image*, const int, std::string*, std::string*,
	int, int, const unsigned char*, int, void*);