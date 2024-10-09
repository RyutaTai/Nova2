#include "CollisionMesh.h"

#include <stack>
#include <functional>
#include <fbxsdk.h>

#include "../Resources/GltfModel.h"
#include "../Others/Converter.h"

CollisionMesh::CollisionMesh(ID3D11Device* device, const std::string& fileName, bool triangulate)
{
	//	Fbx�ǂݍ���
	if (fileName.find(".fbx") != std::string::npos)
	{
		FbxManager* fbxManager = FbxManager::Create();
		FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
		FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
		bool importStatus = false;
		importStatus = fbxImporter->Initialize(fileName.c_str());
		assert(importStatus && "Failed to call FbxImporter::Initialize");
		importStatus = fbxImporter->Import(fbxScene);
		assert(importStatus && "Failed to call FbxImporter::Import");

		FbxGeometryConverter fbxConverter(fbxManager);
		if (triangulate)
		{
			fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
			fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
		}

		std::function<void(FbxNode*)> traverse = [&](FbxNode* fbxNode) {
			if (fbxNode->GetNodeAttribute() && fbxNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::EType::eMesh)
			{
				decltype(meshes_)::reference mesh = meshes_.emplace_back();
				mesh.name_ = fbxNode->GetName();

				DirectX::XMMATRIX globalTransform = XMLoadFloat4x4(&ToXMFLOAT4X4(fbxNode->EvaluateGlobalTransform()));

				FbxMesh* fbxMesh = fbxNode->GetMesh();
				const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
				mesh.subsets_.resize(materialCount > 0 ? materialCount : 1);
				for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
				{
					const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
					mesh.subsets_.at(materialIndex).materialName_ = fbxMaterial->GetName();
				}

				const FbxVector4* controlPoints = fbxMesh->GetControlPoints();
				const int polygon_count = fbxMesh->GetPolygonCount();
				for (int polygonIndex = 0; polygonIndex < polygon_count; ++polygonIndex)
				{
					const int materialIndex = materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;
					decltype(mesh.subsets_)::reference subset = mesh.subsets_.at(materialIndex);

					for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
					{
						DirectX::XMFLOAT3 position;
						const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);
						position.x = static_cast<float>(controlPoints[polygonVertex][0]);
						position.y = static_cast<float>(controlPoints[polygonVertex][1]);
						position.z = static_cast<float>(controlPoints[polygonVertex][2]);
						XMStoreFloat3(&position, XMVector3TransformCoord(XMLoadFloat3(&position), globalTransform));
						subset.positions_.emplace_back(position);
					}

				}
				for (decltype(mesh.subsets_)::const_reference subset : mesh.subsets_)
				{
					for (decltype(subset.positions_)::const_reference position : subset.positions_)
					{
						mesh.meshBoundingBox_[0].x = std::min<float>(mesh.meshBoundingBox_[0].x, position.x);
						mesh.meshBoundingBox_[0].y = std::min<float>(mesh.meshBoundingBox_[0].y, position.y);
						mesh.meshBoundingBox_[0].z = std::min<float>(mesh.meshBoundingBox_[0].z, position.z);
						mesh.meshBoundingBox_[1].x = std::max<float>(mesh.meshBoundingBox_[1].x, position.x);
						mesh.meshBoundingBox_[1].y = std::max<float>(mesh.meshBoundingBox_[1].y, position.y);
						mesh.meshBoundingBox_[1].z = std::max<float>(mesh.meshBoundingBox_[1].z, position.z);
					}
				}
			}
			for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
			{
				traverse(fbxNode->GetChild(childIndex));
			}
			};
		traverse(fbxScene->GetRootNode());

		fbxManager->Destroy();
	}
	else	//	Gltf�ǂݍ���
	{
		tinygltf::TinyGLTF tinyGltf;
		tinyGltf.SetImageLoader(NullLoadImageData, nullptr);

		tinygltf::Model gltfModel;
		std::string error, warning;
		bool succeeded{ false };

		if (fileName.find(".glb") != std::string::npos)
		{
			succeeded = tinyGltf.LoadBinaryFromFile(&gltfModel, &error, &warning, fileName.c_str());
		}
		else if (fileName.find(".gltf") != std::string::npos)
		{
			succeeded = tinyGltf.LoadASCIIFromFile(&gltfModel, &error, &warning, fileName.c_str());
		}
		assert(succeeded && "Failed to load glTF file");

		std::stack<DirectX::XMFLOAT4X4> parentGlobalTransforms;
		std::function<void(int)> traverse = [&](int nodeIndex)
			{
				decltype(gltfModel.nodes)::value_type gltfNode = gltfModel.nodes.at(nodeIndex);

				DirectX::XMFLOAT4X4 globalTransform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
				if (!gltfNode.matrix.empty())
				{
					DirectX::XMFLOAT4X4 transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
					for (size_t row = 0; row < 4; row++)
					{
						for (size_t column = 0; column < 4; column++)
						{
							transform(row, column) = static_cast<float>(gltfNode.matrix.at(4 * row + column));
						}
					}
					DirectX::XMStoreFloat4x4(&globalTransform, DirectX::XMLoadFloat4x4(&transform) * DirectX::XMLoadFloat4x4(&parentGlobalTransforms.top()));
				}
				else
				{
					DirectX::XMMATRIX S = gltfNode.scale.size() > 0 ? DirectX::XMMatrixScaling(static_cast<float>(gltfNode.scale.at(0)), static_cast<float>(gltfNode.scale.at(1)), static_cast<float>(gltfNode.scale.at(2))) : DirectX::XMMatrixIdentity();
					DirectX::XMMATRIX R = gltfNode.rotation.size() > 0 ? DirectX::XMMatrixRotationQuaternion(DirectX::XMVectorSet(static_cast<float>(gltfNode.rotation.at(0)), static_cast<float>(gltfNode.rotation.at(1)), static_cast<float>(gltfNode.rotation.at(2)), static_cast<float>(gltfNode.rotation.at(3)))) : DirectX::XMMatrixIdentity();
					DirectX::XMMATRIX T = gltfNode.translation.size() > 0 ? DirectX::XMMatrixTranslation(static_cast<float>(gltfNode.translation.at(0)), static_cast<float>(gltfNode.translation.at(1)), static_cast<float>(gltfNode.translation.at(2))) : DirectX::XMMatrixIdentity();
					DirectX::XMStoreFloat4x4(&globalTransform, S * R * T * DirectX::XMLoadFloat4x4(&parentGlobalTransforms.top()));
				}

				if (gltfNode.mesh > -1)
				{
					std::vector<tinygltf::Mesh>::const_reference gltfMesh = gltfModel.meshes.at(gltfNode.mesh);
					decltype(meshes_)::reference mesh = meshes_.emplace_back();
					mesh.name_ = gltfMesh.name;
					for (std::vector<tinygltf::Primitive>::const_reference gltfPrimitive : gltfMesh.primitives)
					{
						std::vector<tinygltf::Material>::const_reference gltfMaterial = gltfModel.materials.at(gltfPrimitive.material);
						decltype(mesh.subsets_)::reference subset = mesh.subsets_.emplace_back();
						subset.materialName_ = gltfMaterial.name;

						const tinygltf::Accessor& indexAccessor = gltfModel.accessors.at(gltfPrimitive.indices);
						const tinygltf::BufferView& indexBufferView = gltfModel.bufferViews.at(indexAccessor.bufferView);
						assert(indexAccessor.type == TINYGLTF_TYPE_SCALAR);
						assert(indexAccessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT);
						unsigned int* indices = reinterpret_cast<unsigned int*>(gltfModel.buffers.at(indexBufferView.buffer).data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset);

						const tinygltf::Accessor& positionAccessor = gltfModel.accessors.at(gltfPrimitive.attributes.at("POSITION"));
						const tinygltf::BufferView& positionBufferView = gltfModel.bufferViews.at(positionAccessor.bufferView);
						DirectX::XMFLOAT3* positions = reinterpret_cast<DirectX::XMFLOAT3*>(gltfModel.buffers.at(positionBufferView.buffer).data.data() + positionBufferView.byteOffset + positionAccessor.byteOffset);

						for (size_t indexIndex = 0; indexIndex < indexAccessor.count; ++indexIndex)
						{
							unsigned int index = indices[indexIndex];
							DirectX::XMFLOAT3 position = positions[index];
							DirectX::XMStoreFloat3(&position, XMVector3TransformCoord(XMLoadFloat3(&position), DirectX::XMLoadFloat4x4(&globalTransform)));
							subset.positions_.emplace_back(position);
						}
					}
				}
				for (decltype(gltfNode.children)::value_type childIndex : gltfNode.children)
				{
					parentGlobalTransforms.push(globalTransform);
					traverse(childIndex);
					parentGlobalTransforms.pop();
				}
			};
		decltype(gltfModel.scenes)::const_reference gltfScene = gltfModel.scenes.at(0);
		for (decltype(gltfScene.nodes)::value_type rootNode : gltfScene.nodes)
		{
			parentGlobalTransforms.push({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
			traverse(rootNode);
			parentGlobalTransforms.pop();
		}
	}

	//	Rebuild bounding box
	for (decltype(meshes_)::reference mesh : meshes_)
	{
		for (decltype(mesh.subsets_)::reference subset : mesh.subsets_)
		{
			for (decltype(subset.positions_)::reference position : subset.positions_)
			{
				mesh.meshBoundingBox_[0].x = std::min<float>(mesh.meshBoundingBox_[0].x, position.x);
				mesh.meshBoundingBox_[0].y = std::min<float>(mesh.meshBoundingBox_[0].y, position.y);
				mesh.meshBoundingBox_[0].z = std::min<float>(mesh.meshBoundingBox_[0].z, position.z);
				mesh.meshBoundingBox_[1].x = std::max<float>(mesh.meshBoundingBox_[1].x, position.x);
				mesh.meshBoundingBox_[1].y = std::max<float>(mesh.meshBoundingBox_[1].y, position.y);
				mesh.meshBoundingBox_[1].z = std::max<float>(mesh.meshBoundingBox_[1].z, position.z);
			}
		}
	}

	// �O�p�`���\�z���� mesh.triangles_ �ɒǉ�����
	for (auto& mesh : meshes_)
	{
		for (auto& subset : mesh.subsets_)
		{
			const DirectX::XMFLOAT3* positions = subset.positions_.data();
			const size_t triangleCount = subset.positions_.size() / 3;

			for (size_t triangleIndex = 0; triangleIndex < triangleCount; ++triangleIndex)
			{
				Mesh::Triangle triangle;

				// �O�p�`�̒��_��ݒ�
				triangle.positions_[0] = positions[triangleIndex * 3 + 0];
				triangle.positions_[1] = positions[triangleIndex * 3 + 1];
				triangle.positions_[2] = positions[triangleIndex * 3 + 2];

				// �O�p�`��ǉ�
				mesh.triangles_.emplace_back(triangle);
			}
		}
	}

	CreateAreas(16, 16);

}

//	�G���A���쐬�E�o�^���A�G���A�Ɋ܂܂��O�p�`��o�^
void CollisionMesh::CreateAreas(int gridSizeX, int gridSizeZ)
{
	for (Mesh& mesh : meshes_)
	{
		// ���b�V���̃o�E���f�B���O�{�b�N�X�擾
		auto& minBB = mesh.meshBoundingBox_[0];
		auto& maxBB = mesh.meshBoundingBox_[1];

		float sizeX = maxBB.x - minBB.x;
		float sizeZ = maxBB.z - minBB.z;

		float cellSizeX = sizeX / gridSizeX;
		float cellSizeZ = sizeZ / gridSizeZ;

		// �G���A���쐬���A�o�E���f�B���O�{�b�N�X��ݒ�
		for (int x = 0; x < gridSizeX; ++x)
		{
			for (int z = 0; z < gridSizeZ; ++z)
			{
				Mesh::Area area;
				area.areaBoundingBox_.Center = {
					minBB.x + cellSizeX * (x + 0.5f),
					(minBB.y + maxBB.y) / 2,  // ���S��y���̒���
					minBB.z + cellSizeZ * (z + 0.5f)
				};
				area.areaBoundingBox_.Extents = { cellSizeX / 2, (maxBB.y - minBB.y) / 2, cellSizeZ / 2 };

				mesh.areas_.emplace_back(area);
			}
		}

		// �e�O�p�`���G���A�ɓo�^
		for (int triIndex = 0; triIndex < mesh.triangles_.size(); ++triIndex)
		{
			auto& triangle = mesh.triangles_[triIndex];
			for (Mesh::Area& area : mesh.areas_)
			{
				// �O�p�`���G���A�Ɋ܂܂�邩�ǂ�������
				DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&triangle.positions_[0]);
				DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&triangle.positions_[1]);
				DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&triangle.positions_[2]);

				if (area.areaBoundingBox_.Contains(v0) != DirectX::ContainmentType::DISJOINT ||
					area.areaBoundingBox_.Contains(v1) != DirectX::ContainmentType::DISJOINT ||
					area.areaBoundingBox_.Contains(v2) != DirectX::ContainmentType::DISJOINT)
				{
					area.triangleIndices_.emplace_back(triIndex);
				}
			}
		}
	}
}

inline bool IntersectRayAABB(const float rayPos[3], const float rayDirection[3], const float p0[3], const float p1[3])
{
	float tMin = 0;
	float tMax = +FLT_MAX;

	for (size_t a = 0; a < 3; ++a)
	{
		float inverseDirection = 1.0f / rayDirection[a];
		float t0 = (p0[a] - rayPos[a]) * inverseDirection;
		float t1 = (p1[a] - rayPos[a]) * inverseDirection;
		if (inverseDirection < 0.0f)
		{
			std::swap<float>(t0, t1);
		}
		tMin = std::max<float>(t0, tMin);
		tMax = std::min<float>(t1, tMax);

		if (tMax <= tMin)
		{
			return false;
		}
	}
	return true;
}

// The coordinate system of all function arguments is world space.
bool CollisionMesh::Raycast(_In_ DirectX::XMFLOAT3 rayStartPosition, _In_ DirectX::XMFLOAT3 rayDirection, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersectionPosition, _Out_ DirectX::XMFLOAT3& intersectionNormal,
	_Out_ std::string& intersectionMesh, _Out_ std::string& intersectionMaterial, _In_ float rayLengthLimit, _In_ bool skipIf) const
{
	//	���f���̃��[���h�ϊ��s����擾���A�t�s����v�Z����
	DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMMATRIX inverseT = DirectX::XMMatrixInverse(NULL, T);

	//	���C�̎n�_�A�I�_�ƕ��������f����Ԃɕϊ�����
	DirectX::XMStoreFloat3(&rayStartPosition, DirectX::XMVector3TransformCoord(XMLoadFloat3(&rayStartPosition), inverseT));
	DirectX::XMStoreFloat3(&rayDirection, DirectX::XMVector3TransformNormal(XMLoadFloat3(&rayDirection), inverseT));

	//	���C�̎n�_�A�I�_�ƕ�����VECTOR�ɕϊ�����
	const DirectX::XMVECTOR RayStart = XMLoadFloat3(&rayStartPosition);
	const DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(XMLoadFloat3(&rayDirection));

	//	���������o���ꂽ�O�p�`�̐��ƁA�ł��߂������_�܂ł̋���
	int intersectionCount = 0;
	float closestDistance = FLT_MAX;

	//	�e���b�V���ɂ��ď������s��
	for (decltype(meshes_)::const_reference mesh : meshes_)
	{
#if 1
		const float* AABBMin = reinterpret_cast<const float*>(&mesh.meshBoundingBox_[0]);
		const float* AABBMax = reinterpret_cast<const float*>(&mesh.meshBoundingBox_[1]);
		if (!IntersectRayAABB(reinterpret_cast<const float*>(&rayStartPosition), reinterpret_cast<const float*>(&rayDirection), AABBMin, AABBMax))
		{
			continue;
		}
#endif
		//	�O�p�`�̖@�����v�Z����
		for (decltype(mesh.subsets_)::const_reference subset : mesh.subsets_)
		{
			const DirectX::XMFLOAT3* positions = subset.positions_.data();
			const size_t triangleCount = subset.positions_.size() / 3;

			for (size_t triangleIndex = 0; triangleIndex < triangleCount; triangleIndex++)
			{
				const DirectX::XMVECTOR A = XMLoadFloat3(&positions[triangleIndex * 3 + 0]);
				const DirectX::XMVECTOR B = XMLoadFloat3(&positions[triangleIndex * 3 + 1]);
				const DirectX::XMVECTOR C = XMLoadFloat3(&positions[triangleIndex * 3 + 2]);

				const DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A)));
				const float d = DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, A), 0);
				const float denominator{ DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, Direction), 0) };

				//	�O�p�`�����s�łȂ��ꍇ�A���������o����
				if (denominator < 0)
				{
					const float numerator = d - DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(N, RayStart), 0);
					const float t = numerator / denominator;

					//	���C���O�p�`�ƌ������A�����C�̒����������ɂ���ꍇ
					if (t > 0 && t < rayLengthLimit)
					{
						//	�����_���v�Z����
						DirectX::XMVECTOR Q = DirectX::XMVectorAdd(RayStart, DirectX::XMVectorScale(Direction, t));

						//	�����_���O�p�`�̓����ɂ��邩�ǂ����𔻒肷��
						const DirectX::XMVECTOR QA = DirectX::XMVectorSubtract(A, Q);
						const DirectX::XMVECTOR QB = DirectX::XMVectorSubtract(B, Q);
						const DirectX::XMVECTOR QC = DirectX::XMVectorSubtract(C, Q);

						DirectX::XMVECTOR U = DirectX::XMVector3Cross(QB, QC);
						DirectX::XMVECTOR V = DirectX::XMVector3Cross(QC, QA);
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(U, V), 0) < 0)
						{
							continue;
						}
						DirectX::XMVECTOR W = DirectX::XMVector3Cross(QA, QB);
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(U, W), 0) < 0)
						{
							continue;
						}
						if (DirectX::XMVectorGetByIndex(DirectX::XMVector3Dot(V, W), 0) < 0)
						{
							continue;
						}
						intersectionCount++;

						//	�����_�����O�̍ŒZ���������߂��ꍇ�A�ŒZ�����ƌ��������X�V����
						if (t < closestDistance)
						{
							closestDistance = t;

							XMStoreFloat3(&intersectionPosition, XMVector3TransformCoord(Q, T));
							XMStoreFloat3(&intersectionNormal, DirectX::XMVector3Normalize(XMVector3TransformNormal(N, T)));
							intersectionMesh = mesh.name_;
							intersectionMaterial = subset.materialName_;

							if (skipIf)
							{
								return true;
							}
						}
					}
				}
			}
		}
	}
	return intersectionCount > 0;
}

bool CollisionMesh::RaycastWithSpaceDivision(_In_ DirectX::XMFLOAT3 rayStartPosition,
	_In_ DirectX::XMFLOAT3 rayDirection,
	_In_ const DirectX::XMFLOAT4X4& transform,
	_Out_ DirectX::XMFLOAT3& intersectionPosition,
	_Out_ DirectX::XMFLOAT3& intersectionNormal,
	_Out_ std::string& intersectionMesh,
	_Out_ std::string& intersectionMaterial,
	_In_ float rayLengthLimit,
	_In_ bool skipIf) const
{
	// ���f���̃��[���h�ϊ��s����擾���A�t�s����v�Z����
	DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
	DirectX::XMMATRIX inverseT = DirectX::XMMatrixInverse(nullptr, T);

	// ���C�̎n�_�ƕ��������f����Ԃɕϊ�����
	DirectX::XMFLOAT3 localRayStart, localRayDirection;
	DirectX::XMStoreFloat3(&localRayStart, DirectX::XMVector3TransformCoord(XMLoadFloat3(&rayStartPosition), inverseT));
	DirectX::XMStoreFloat3(&localRayDirection, DirectX::XMVector3TransformNormal(XMLoadFloat3(&rayDirection), inverseT));

	const DirectX::XMVECTOR RayStart = XMLoadFloat3(&localRayStart);
	const DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(XMLoadFloat3(&localRayDirection)); // ���K��

	// Direction ���[���x�N�g�����ǂ������`�F�b�N
	if (DirectX::XMVector3Equal(Direction, DirectX::XMVectorZero()))
	{
		return false; // �[���x�N�g���̏ꍇ�͏������X�L�b�v
	}

	bool hit = false;
	float closestDistance = FLT_MAX;

	// �e���b�V���ɂ��ď������s��
	for (const auto& mesh : meshes_)
	{
		// AABB�Ƃ̌�������
		const float* AABBMin = reinterpret_cast<const float*>(&mesh.meshBoundingBox_[0]);
		const float* AABBMax = reinterpret_cast<const float*>(&mesh.meshBoundingBox_[1]);
		if (!IntersectRayAABB(reinterpret_cast<const float*>(&localRayStart), reinterpret_cast<const float*>(&localRayDirection), AABBMin, AABBMax))
		{
			continue; // �������Ȃ���Ύ��̃��b�V����
		}

		// �e�G���A�ɂ��ď������s��
		for (const auto& area : mesh.areas_)
		{
			float distance = FLT_MAX;

			// ���C���G���A�Ɍ������邩�m�F
			if (area.areaBoundingBox_.Intersects(RayStart, Direction, distance))
			{
				// �G���A���̎O�p�`�Ń��C�L���X�g���s��
				for (int triIndex : area.triangleIndices_)
				{
					const auto& triangle = mesh.triangles_[triIndex];

					// �O�p�`�̒��_�����[�h
					const DirectX::XMVECTOR A = XMLoadFloat3(&triangle.positions_[0]);
					const DirectX::XMVECTOR B = XMLoadFloat3(&triangle.positions_[1]);
					const DirectX::XMVECTOR C = XMLoadFloat3(&triangle.positions_[2]);

					// �O�p�`�̖@�����v�Z
					const DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A)));
					const float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, A));
					const float denominator = DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, Direction));

					// �O�p�`�����s�łȂ��ꍇ�A���������o����
					if (denominator < 0)
					{
						const float numerator = d - DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, RayStart));
						const float t = numerator / denominator;

						// ���C���O�p�`�ƌ������A�����C�̒����������ɂ���ꍇ
						if (t > 0 && t < rayLengthLimit)
						{
							// �����_���v�Z
							DirectX::XMVECTOR Q = DirectX::XMVectorAdd(RayStart, DirectX::XMVectorScale(Direction, t));

							// �����_���O�p�`�̓����ɂ��邩�ǂ����𔻒肷��
							const DirectX::XMVECTOR QA = DirectX::XMVectorSubtract(A, Q);
							const DirectX::XMVECTOR QB = DirectX::XMVectorSubtract(B, Q);
							const DirectX::XMVECTOR QC = DirectX::XMVectorSubtract(C, Q);

							DirectX::XMVECTOR U = DirectX::XMVector3Cross(QB, QC);
							DirectX::XMVECTOR V = DirectX::XMVector3Cross(QC, QA);
							if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(U, V)) < 0)
							{
								continue;
							}
							U = DirectX::XMVector3Cross(QA, QB);
							if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(U, V)) < 0)
							{
								continue;
							}
							if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(V, U)) < 0)
							{
								continue;
							}

							hit = true;

							// �����_���ŒZ�������߂��ꍇ�A�����X�V
							if (t < closestDistance)
							{
								closestDistance = t;

								// ���[���h��Ԃɖ߂��Č����_�̈ʒu�Ɩ@����ݒ�
								XMStoreFloat3(&intersectionPosition, XMVector3TransformCoord(Q, T));
								XMStoreFloat3(&intersectionNormal, N);
								intersectionMesh = mesh.name_;
								intersectionMaterial = triangle.materialName_; // �ގ������擾

								if (skipIf)
								{
									return true; // �ŏ��̌����_�ŏI��
								}
							}
						}
					}
				}
			}
		}
	}
	return hit;
}

//bool CollisionMesh::RaycastWithSpaceDivision(_In_ DirectX::XMFLOAT3 rayStartPosition,
//	_In_ DirectX::XMFLOAT3 rayDirection,
//	_In_ const DirectX::XMFLOAT4X4& transform,
//	_Out_ DirectX::XMFLOAT3& intersectionPosition,
//	_Out_ DirectX::XMFLOAT3& intersectionNormal,
//	_Out_ std::string& intersectionMesh,
//	_Out_ std::string& intersectionMaterial,
//	_In_ float rayLengthLimit,
//	_In_ bool skipIf) const
//{
//	// ���f���̃��[���h�ϊ��s����擾���A�t�s����v�Z����
//	DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
//	DirectX::XMMATRIX inverseT = DirectX::XMMatrixInverse(nullptr, T);
//
//	// ���C�̎n�_�ƕ��������f����Ԃɕϊ�����
//	DirectX::XMFLOAT3 localRayStart, localRayDirection;
//	DirectX::XMStoreFloat3(&localRayStart, DirectX::XMVector3TransformCoord(XMLoadFloat3(&rayStartPosition), inverseT));
//	DirectX::XMStoreFloat3(&localRayDirection, DirectX::XMVector3TransformNormal(XMLoadFloat3(&rayDirection), inverseT));
//
//	const DirectX::XMVECTOR RayStart = XMLoadFloat3(&localRayStart);
//	const DirectX::XMVECTOR Direction = DirectX::XMVector3Normalize(XMLoadFloat3(&localRayDirection)); // ���K��
//
//	// Direction ���[���x�N�g�����ǂ������`�F�b�N
//	if (DirectX::XMVector3Equal(Direction, DirectX::XMVectorZero()))
//	{
//		return false; // �[���x�N�g���̏ꍇ�͏������X�L�b�v
//	}
//
//	bool hit = false;
//	float closestDistance = FLT_MAX;
//
//	// �e���b�V���ɂ��ď������s��
//	for (const auto& mesh : meshes_)
//	{
//		// AABB�Ƃ̌�������
//		const float* AABBMin = reinterpret_cast<const float*>(&mesh.meshBoundingBox_[0]);
//		const float* AABBMax = reinterpret_cast<const float*>(&mesh.meshBoundingBox_[1]);
//		if (!IntersectRayAABB(reinterpret_cast<const float*>(&localRayStart), reinterpret_cast<const float*>(&localRayDirection), AABBMin, AABBMax))
//		{
//			continue; // �������Ȃ���Ύ��̃��b�V����
//		}
//
//		// �e�G���A�ɂ��ď������s��
//		for (const auto& area : mesh.areas_)
//		{
//			float distance = FLT_MAX;
//
//			// ���C���G���A�Ɍ������邩�m�F
//			if (area.areaBoundingBox_.Intersects(RayStart, Direction, distance))
//			{
//				// �G���A���̎O�p�`�Ń��C�L���X�g
//				for (int triIndex : area.triangleIndices_)
//				{
//					const auto& triangle = mesh.triangles_[triIndex];
//
//					// �O�p�`�̒��_�����[�h
//					const DirectX::XMVECTOR A = XMLoadFloat3(&triangle.positions_[0]);
//					const DirectX::XMVECTOR B = XMLoadFloat3(&triangle.positions_[1]);
//					const DirectX::XMVECTOR C = XMLoadFloat3(&triangle.positions_[2]);
//
//					// �O�p�`�̖@�����v�Z
//					const DirectX::XMVECTOR N = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(B, A), DirectX::XMVectorSubtract(C, A)));
//					const float d = DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, A));
//					const float denominator = DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, Direction));
//
//					// �O�p�`�����s�łȂ��ꍇ�A���������o����
//					if (denominator < 0)
//					{
//						const float numerator = d - DirectX::XMVectorGetX(DirectX::XMVector3Dot(N, RayStart));
//						const float t = numerator / denominator;
//
//						// ���C���O�p�`�ƌ������A�����C�̒����������ɂ���ꍇ
//						if (t > 0 && t < rayLengthLimit)
//						{
//							// �����_���v�Z
//							DirectX::XMVECTOR Q = DirectX::XMVectorAdd(RayStart, DirectX::XMVectorScale(Direction, t));
//
//							// �����_���O�p�`�̓����ɂ��邩�ǂ����𔻒肷��
//							const DirectX::XMVECTOR QA = DirectX::XMVectorSubtract(A, Q);
//							const DirectX::XMVECTOR QB = DirectX::XMVectorSubtract(B, Q);
//							const DirectX::XMVECTOR QC = DirectX::XMVectorSubtract(C, Q);
//
//							DirectX::XMVECTOR U = DirectX::XMVector3Cross(QB, QC);
//							DirectX::XMVECTOR V = DirectX::XMVector3Cross(QC, QA);
//							if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(U, V)) < 0)
//							{
//								continue;
//							}
//							U = DirectX::XMVector3Cross(QA, QB);
//							if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(U, V)) < 0)
//							{
//								continue;
//							}
//							if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(V, U)) < 0)
//							{
//								continue;
//							}
//
//							hit = true;
//
//							// �����_���ŒZ�������߂��ꍇ�A�����X�V
//							if (t < closestDistance)
//							{
//								closestDistance = t;
//
//								// ���[���h��Ԃɖ߂��Č����_�̈ʒu�Ɩ@����ݒ�
//								XMStoreFloat3(&intersectionPosition, XMVector3TransformCoord(Q, T));
//								XMStoreFloat3(&intersectionNormal, N);
//								intersectionMesh = mesh.name_;
//								intersectionMaterial = triangle.materialName_; // �ގ������擾
//
//								if (skipIf)
//								{
//									return true; // �ŏ��̌����_�ŏI��
//								}
//							}
//						}
//					}
//				}
//			}
//		}
//	}
//	return hit;
//}
