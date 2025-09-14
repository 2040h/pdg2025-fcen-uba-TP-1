//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:14:44 gtaubin>
//------------------------------------------------------------------------
//
// SaverStl.cpp
//
// Written by: Rundong He
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "SaverStl.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

#include "core/Faces.hpp"

// debugging
#include <iostream>
#define DBG(x) cout << #x << " = " << x << endl;

const char* SaverStl::_ext = "stl";

//////////////////////////////////////////////////////////////////////
bool SaverStl::save(const char* filename, SceneGraph& wrl) const {
  bool success = false;
  if(filename!=(char*)0) {

    // Check these conditions
    #define check(condition) if (not (condition)) return false;

    // 1) the SceneGraph should have a single child
    check(wrl.getNumberOfChildren() == 1);

    // 2) the child should be a Shape node
    auto it = wrl.getChildren().begin();
    pNode child = *it;
    check(child->isShape());

    // 3) the geometry of the Shape node should be an IndexedFaceSet node
    Shape* shape = static_cast<Shape*>(child);
    pNode shapeGeometry = shape->getGeometry();
    check(shapeGeometry->isIndexedFaceSet());

    // - construct an instance of the Faces class from the IndexedFaceSet
    // - remember to delete it when you are done with it (if necessary)
    //   before returning
    IndexedFaceSet* indexedFaceSet = static_cast<IndexedFaceSet*>(shapeGeometry);
    Faces faces(0xdeadbeef, indexedFaceSet->getCoordIndex());

    // 4) the IndexedFaceSet should be a triangle mesh
    check(indexedFaceSet->isTriangleMesh());

    // 5) the IndexedFaceSet should have normals per face
    check(not indexedFaceSet->getNormalPerVertex());

    // if (all the conditions are satisfied) {

    FILE* fp = fopen(filename,"w");
    if(	fp!=(FILE*)0) {

      // if set, use ifs->getName()
      // otherwise use filename,
      // but first remove directory and extension

      fprintf(fp,"solid %s\n",filename);

      // TODO ...
      // for each face {
      //   ...
      // }
      for (int iF = 0; iF < faces.getNumberOfFaces(); ++iF) {

        int c1 = faces.getFaceFirstCorner(iF),
            c2 = faces.getNextCorner(c1),
            c3 = faces.getNextCorner(c2);
        
        int v1 = faces.getFaceVertex(iF, c1),
            v2 = faces.getFaceVertex(iF, c2),
            v3 = faces.getFaceVertex(iF, c3);

        // DBG(iF) DBG(c1) DBG(c2) DBG(c3) DBG(v1) DBG(v2) DBG(v3);

        vector<float>& normal = indexedFaceSet->getNormal();

        // DBG(faces.getNumberOfFaces());
        // DBG(normal.size());

        Vec3f n = { normal[iF * 3], normal[iF * 3 + 1], normal[iF * 3 + 2] };

        fprintf(fp, "\tfacet normal %f %f %f\n", n.x, n.y, n.z);
        fprintf(fp, "\t\touter loop\n");

        // DBG(faces.getNumberOfVertices());
        // DBG(indexedFaceSet->getCoord().size());
        for (int v : {v1, v2, v3}) {
          fprintf(fp, "\t\t\tvertex ");
          vector<float>& coord = indexedFaceSet->getCoord();
          float x = coord[3 * v],
                y = coord[3 * v + 1],
                z = coord[3 * v + 2];
          fprintf(fp, "%f %f %f\n", x, y, z);
        }
        fprintf(fp, "\t\tendloop\n");
        fprintf(fp, "\tendfacet\n");
      }

      fprintf(fp, "endsolid %s\n", filename);
      fclose(fp);
      success = true;
    }
    // DBG(10)
    // } endif (all the conditions are satisfied)

  }
  return success;
  #undef check
}
