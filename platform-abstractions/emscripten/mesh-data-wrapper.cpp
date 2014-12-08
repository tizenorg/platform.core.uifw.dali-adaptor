/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include "mesh-data-wrapper.h"

// EXTERNAL INCLUDES

// INTERNAL INCLUDES

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// MeshDataWrapper
//
void MeshDataWrapper::SetData( const Dali::MeshData::VertexContainer& vertices,
                               const Dali::MeshData::FaceIndices&     faceIndices,
                               const Dali::BoneContainer&   bones,
                               const MaterialWrapper  material )
{
  meshData.SetData(vertices, faceIndices, bones, material.GetMaterial());
}

void MeshDataWrapper::SetLineData( const Dali::MeshData::VertexContainer& vertices,
                                   const Dali::MeshData::FaceIndices& faceIndices,
                                   const MaterialWrapper  material )
{
  meshData.SetLineData(vertices,faceIndices,material.GetMaterial());
}



void MeshDataWrapper::SetFaceIndices( const Dali::MeshData::FaceIndices& faceIndices )
{
  meshData.SetFaceIndices(faceIndices);
}

void MeshDataWrapper::SetVertices( const Dali::MeshData::VertexContainer& vertices )
{
  meshData.SetVertices(vertices);
}

void MeshDataWrapper::SetHasNormals(bool hasNormals)
{
  meshData.SetHasNormals(hasNormals);
}

void MeshDataWrapper::SetHasTextureCoords(bool hasTexCoords)
{
  meshData.SetHasTextureCoords(hasTexCoords);
}

void MeshDataWrapper::SetHasColor(bool hasColor)
{
  meshData.SetHasColor(hasColor);
}


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali
