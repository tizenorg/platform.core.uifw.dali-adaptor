#ifndef __DALI_MESH_DATA_WRAPPER_H__
#define __DALI_MESH_DATA_WRAPPER_H__

/*
Copyright (c) 2000-2013 Samsung Electronics Co., Ltd All Rights Reserved

This file is part of Dali

PROPRIETARY/CONFIDENTIAL

This software is the confidential and proprietary information of
SAMSUNG ELECTRONICS ("Confidential Information"). You shall not
disclose such Confidential Information and shall use it only in
accordance with the terms of the license agreement you entered
into with SAMSUNG ELECTRONICS.

SAMSUNG make no representations or warranties about the suitability
of the software, either express or implied, including but not limited
to the implied warranties of merchantability, fitness for a particular
purpose, or non-infringement. SAMSUNG shall not be liable for any
damages suffered by licensee as a result of using, modifying or
distributing this software or its derivatives.
*/

// EXTERNAL INCLUDES
#include <dali/public-api/dali-core.h>
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
