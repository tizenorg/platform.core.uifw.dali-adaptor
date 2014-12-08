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
