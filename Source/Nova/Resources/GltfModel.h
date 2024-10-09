#pragma once 

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <unordered_map>

#include "../../tinygltf-release/tiny_gltf.h"

#define NOMINMAX
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE

class GltfModel
{
public:
	struct Scene
	{
		std::string			name_;
		std::vector<int>	nodes_; // Array of 'root' nodes 
	};
	std::vector<Scene>		scenes_;

	struct Node
	{
		std::string name_;
		int skin_{ -1 };  // index of skin referenced by this node 
		int mesh_{ -1 };  // index of mesh referenced by this node 

		std::vector<int> children_; // An array of indices of child nodes of this node 

		// Local transforms 
		DirectX::XMFLOAT4 rotation_				{ 0, 0, 0, 1 };
		DirectX::XMFLOAT3 scale_				{ 1, 1, 1 };
		DirectX::XMFLOAT3 translation_			{ 0, 0, 0 };

		DirectX::XMFLOAT4X4 globalTransform_	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		bool isRoot_ = false;	//	ルートノードかどうか
	};
	std::vector<Node> nodes_;

	struct BufferView
	{
		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
		size_t strideInBytes_{ 0 };
		size_t sizeInBytes_{ 0 };
		size_t count()const
		{
			return sizeInBytes_ / strideInBytes_;
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
		};
		std::vector<Primitive> primitives_;
	};
	std::vector<Mesh> meshes_;

	struct TextureInfo
	{
		int index_		= -1;
		int texcoord_	=  0;
	};

	struct NormalTextureInfo
	{
		int		index_		= -1;
		int		texcoord_	=  0;
		float	scale_		=  1;
	};

	struct OcclusionTextureInfo
	{
		int index_		= -1;
		int texcoord_	=  0;
		int strength_	= -1;
	};

	struct PbrMetallicRoughness
	{
		float		baseColorFactor_[4] = { 1,1,1,1 };
		TextureInfo baseColorTexture_;
		float		metallicFactor_		= 1;
		float		roughnessFactor_	= -1;
		TextureInfo metallicRoughnessTexture_;
	};

	struct Material
	{
		std::string name_; 

		struct Cbuffer
		{
			float	emissiveFactor_[3]	= { 0,0,0 };
			int		alphaMode_			= 0;	//	"OPAQUE" : 0,"MASK" : 1,"BLEND" : 2
			float	alphaCutOff_		= 0.5f;
			bool	doubleSided_		= false;
			
			PbrMetallicRoughness	pbrMetallicRoughness_;

			NormalTextureInfo		normalTexture_;
			OcclusionTextureInfo	occlusionTexture_;
			TextureInfo				emissiveTexture_;
		};
		Cbuffer data_;
	};
	std::vector<Material> materials_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> materialResourceView_;

	struct Texture
	{
		std::string name_;
		int			source_{ -1 };
	};
	std::vector<Texture> textures_;

	struct Image
	{
		std::string name_;
		int			width_		{ -1 };
		int			height_		{ -1 };
		int			component_	{ -1 };
		int			bits_		{ -1 };
		int			pixelType_	{ -1 };
		int			bufferView_;
		std::string mimeType_;
		std::string uri_;
		bool		asIs_		{ false };
	};
	std::vector<Image> images_;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureResourceViews_;

	struct Skin
	{
		std::vector<DirectX::XMFLOAT4X4> inverseBindMatrices_;
		std::vector<int> joints_;
	};
	std::vector<Skin> skins_;

	struct Animation
	{
		std::string name_;
		float		duration_{ 0.0f };
		struct Channel
		{
			int			sampler_	{ -1 };
			int			targetNode_	{ -1 };
			std::string targetPath_;
		};
		std::vector<Channel> channels_;
	
		struct Sampler
		{
			int			input_	{ -1 };
			int			output_	{ -1 };
			std::string interpolation_;
		};
		std::vector<Sampler> samplers_;

		std::unordered_map<int/*sampler.input*/,	std::vector<float>>				timelines_;
		std::unordered_map<int/*sampler.output*/,	std::vector<DirectX::XMFLOAT3>> scales_;
		std::unordered_map<int/*sampler.output*/,	std::vector<DirectX::XMFLOAT4>> rotations_;
		std::unordered_map<int/*sampler.output*/,	std::vector<DirectX::XMFLOAT3>> translations_;
	};
	std::vector<Animation> animations_;

	struct PrimitiveConstants
	{
		DirectX::XMFLOAT4X4 world_;
		int					material_{ -1 };
		int					hasTangent_{ 0 };
		int					skin_{ -1 };
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
	GltfModel(ID3D11Device* device, const std::string& fileName, const std::string& rootNodeName = "root");
	virtual ~GltfModel() = default;

	void Render(const DirectX::XMMATRIX& world/*, const std::vector<Node>& animatedNodes*/);
	void DrawDebug();

	void PlayAnimation(const int index, const bool loop = false, const float speed = 1.0f, const float blendTime = 1.0f, const float cutTime = 0.0f);
	void UpdateAnimation(const float& elapsedTime);

	//void PlayBlendAnimation(const int index, const bool loop = false, const float speed = 1.0f);
	//void UpdateBlendAnimation(const float elapsedTime);

	void Animate(size_t animationIndex, float time, std::vector<Node>& animatedNodes);
	void AppendAnimation(ID3D11Device* device, const std::string& fileName);
	void BlendAnimations(const std::vector<Node>& fromNodes, const std::vector<Node>& toNodes, float factor, std::vector<Node>& outNodes);
	bool IsPlayAnimation()const;

	int	GetCurrentAnimNum();	//	現在再生中のアニメーション番号
	DirectX::XMFLOAT3 GetJointPosition(const std::string& meshName, const std::string& boneName, const DirectX::XMFLOAT4X4& transform);		//	ジョイントポジション取得
	DirectX::XMFLOAT3 GetJointPosition(size_t nodeIndex, const DirectX::XMFLOAT4X4& transform);

	void SetPixelShader(ID3D11PixelShader* pixelShader) { pixelShader_ = pixelShader; }		//	PixelShader設定

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

	std::string fileName_;

	// --- GLTF_ANIMATION ---
	bool	isAnimationLoop_			= false;	//	アニメーションループフラグ
	int		currentAnimationIndex_		= -1;		//	アニメーション番号
	float	animationSpeed_				= 0.0f;		//	再生速度
	float   currentAnimationSeconds_	= 0.0f;		//	アニメーション再生用タイマー

	std::vector<GltfModel::Node> animatedNodes_[2];
	std::vector<GltfModel::Node> blendedAnimatedNodes_;
	int		animationClip_ = 0;
	float	time_ = 0;
	float	factor_ = 0;
	int		transitionState_ = 0;
	float	transitionTime_ = 1.0f;			//	どれくらい時間をかけてブレンドするか
	float	cutTime_ = 0.0f;

	bool	isTransition_ = false;			//	アニメーション遷移中かどうか
	bool	animationEndFlag_ = false;		//	アニメーション再生が終わっているかどうか


};

bool NullLoadImageData(tinygltf::Image*, const int, std::string*, std::string*,
	int, int, const unsigned char*, int, void*);