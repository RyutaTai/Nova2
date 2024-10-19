#pragma once
#define NOMINMAX

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <vector>
#include <unordered_map>

#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "../../tinygltf-release/tiny_gltf.h"

class GltfModelStaticBatching
{
public:
	struct Scene
	{
		std::string name_;
		std::vector<int> nodes_; // Array of 'root' nodes_
	};
	std::vector<Scene> scenes_;

	struct Node
	{
		std::string name_;
		int skin_{ -1 };  // index_ of skin_ referenced by this Node
		int mesh_{ -1 };  // index_ of mesh_ referenced by this Node

		std::vector<int> children_; // An array of indices of child nodes_ of this Node

		// Local transforms
		DirectX::XMFLOAT4 rotation_{ 0, 0, 0, 1 };
		DirectX::XMFLOAT3 scale_{ 1, 1, 1 };
		DirectX::XMFLOAT3 translation_{ 0, 0, 0 };

		DirectX::XMFLOAT4X4 globalTransform_{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	};
	std::vector<Node> nodes_;

	struct BufferView
	{
		DXGI_FORMAT format_ = DXGI_FORMAT_UNKNOWN;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;
		size_t strideInBytes_{ 0 };
		size_t sizeInBytes_{ 0 };
		size_t count() const
		{
			return sizeInBytes_ / strideInBytes_;
		}
		BufferView() = default;
	};

	struct Primitive
	{
		int material_;
		std::map<std::string, BufferView> vertexBufferViews_;
		BufferView indexBufferView_;
	};
	std::vector<Primitive> primitives_;

	struct TextureInfo
	{
		int index_ = -1;		// required.
		int texcoord_ = 0;		// The set index_ of Texture's TEXCOORD attribute used for Texture coordinate mapping.
	};
	
	struct NormalTextureInfo
	{
		int index_ = -1;		// required
		int texcoord_ = 0;		// The set index_ of Texture's TEXCOORD attribute used for Texture coordinate mapping.
		float scale_ = 1;		// scaledNormal = normalize((<sampled normal Texture value> * 2.0 - 1.0) * vec3(<normal scale_>, <normal scale_>, 1.0))
	};
	
	struct OcclusionTextureInfo
	{
		int index_ = -1;   // required
		int texcoord_ = 0;     // The set index_ of Texture's TEXCOORD attribute used for Texture coordinate mapping.
		float strength_ = 1;  // A scalar parameter controlling the amount of occlusion applied. A value of `0.0` means no occlusion. A value of `1.0` means full occlusion. This value affects the final occlusion value as: `1.0 + strength_ * (<sampled occlusion Texture value> - 1.0)`.
	};

	struct PbrMetallicRoughness
	{
		float baseColorFactor_[4] = { 1, 1, 1, 1 };  // len = 4. default [1,1,1,1]
		TextureInfo basecolorTexture_;
		float metallicFactor_ = 1;   // default 1
		float roughnessFactor_ = 1;  // default 1
		TextureInfo metallicRoughnessTexture_;
	};

	struct Material 
	{
		std::string name_;
		struct Cbuffer
		{
			float	emissiveFactor_[3] = { 0, 0, 0 };	// length 3. default [0, 0, 0]
			int		alphaMode_ = 0;						// "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
			float	alphaCutoff_ = 0.5f;				// default 0.5
			int		doubleSided_ = 0;					// default false;

			PbrMetallicRoughness pbrMetallicRoughness_;

			NormalTextureInfo normalTexture_;
			OcclusionTextureInfo occlusionTexture_;
			TextureInfo emissiveTexture_;
		};
		Cbuffer data_;
	};
	std::vector<Material> materials_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> materialResourceView_;

	struct Texture
	{
		std::string name_;
		int source_{ -1 };
	};
	std::vector<Texture> textures_;

	struct Image
	{
		std::string name_;
		int width_{ -1 };
		int height_{ -1 };
		int component_{ -1 };
		int bits{ -1 };			// bit depth per channel. 8(byte), 16 or 32.
		int pixelType_{ -1 };	// pixel type(TINYGLTF_COMPONENT_TYPE_***). usually UBYTE(bits = 8) or USHORT(bits = 16)
		int bufferView_;		// (required if no uri_)
		std::string mimeType_;	// (required if no uri_) ["Image/jpeg", "Image/png", "Image/bmp", "Image/gif"]
		std::string uri_;		// (required if no mimeType) uri_ is not decoded(e.g. whitespace may be represented as %20)

		// When this flag is true, data_ is stored to `image` in as-is format_(e.g. jpeg
		// compressed for "Image/jpeg" mime) This feature is good if you use custom
		// Image loader function. (e.g. delayed decoding of images_ for faster glTF
		// parsing) Default parser for Image does not provide as-is loading feature at
		// the moment. (You can manipulate this by providing your own LoadImageData
		// function)
		bool asIs_{ false };
	};
	std::vector<Image> images_;
	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textureResourceViews_;

private:
	struct PrimitiveConstants
	{
		DirectX::XMFLOAT4X4 world_;
		int material_{ -1 };
		int hasTangent_{ 0 };
		int pad_[2];
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitiveCbuffer_;

public:
	GltfModelStaticBatching(ID3D11Device* device, const std::string& fileName, const bool setColor = false, const DirectX::XMFLOAT4 color = { 0,0,0,1 });
	virtual ~GltfModelStaticBatching() = default;

	void Render(const DirectX::XMMATRIX& world);

	void DrawDebug();

	void SetPixelShader(ID3D11PixelShader* pixelShader) { pixelShader_ = pixelShader; }		//	PixelShaderê›íË

private:
	void FetchNodes(const tinygltf::Model& gltfModel);
	void FetchMeshes(ID3D11Device* device, const tinygltf::Model& gltfModel);
	void FetchMaterials(ID3D11Device* device, const tinygltf::Model& gltfModel, const bool setColor = false, const DirectX::XMFLOAT4 color = { 0,0,0,1 });
	void FetchTextures(ID3D11Device* device, const tinygltf::Model& gltfModel);
	void CumulateTransforms(std::vector<Node>& nodes);
	BufferView MakeBufferView(const tinygltf::Accessor& accessor);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	inputLayout_;

	std::string filename_;

};
