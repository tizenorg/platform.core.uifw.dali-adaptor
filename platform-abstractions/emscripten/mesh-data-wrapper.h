#ifndef __DALI_MESH_DATA_WRAPPER_H__
#define __DALI_MESH_DATA_WRAPPER_H__

/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd.
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

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
#include <dali/devel-api/geometry/mesh-data.h>
#include "emscripten/emscripten.h"
#include "emscripten/bind.h"

// INTERNAL INCLUDES
#include "material-wrapper.h"

namespace Dali
{
namespace Internal
{
namespace Emscripten
{

//
// MeshDataWrapper
//
class MeshDataWrapper
{
public:
  Dali::MeshData meshData;

  void SetData( const Dali::MeshData::VertexContainer& vertices,
                const Dali::MeshData::FaceIndices& faceIndices,
                const Dali::BoneContainer&   bones,
                const MaterialWrapper  material );

  void SetLineData( const Dali::MeshData::VertexContainer& vertices,
                    const Dali::MeshData::FaceIndices& faceIndices,
                    const MaterialWrapper  material );

  void SetFaceIndices( const Dali::MeshData::FaceIndices& faceIndices );
  void SetVertices( const Dali::MeshData::VertexContainer& vertices );
  void SetHasNormals(bool hasNormals);
  void SetHasTextureCoords(bool hasTexCoords);
  void SetHasColor(bool hasColor);
};


}; // namespace Emscripten
}; // namespace Internal
}; // namespace Dali

#endif // header
