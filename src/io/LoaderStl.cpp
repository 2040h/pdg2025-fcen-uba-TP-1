//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:12:21 gtaubin>
//------------------------------------------------------------------------
//
// LoaderStl.cpp
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

#include <stdio.h>
#include "TokenizerFile.hpp"
#include "LoaderStl.hpp"
#include "StrException.hpp"

#include "wrl/Shape.hpp"
#include "wrl/Appearance.hpp"
#include "wrl/Material.hpp"
#include "wrl/IndexedFaceSet.hpp"

// debugging
#include <iostream>
#define DBG(x) cout << #x << " = " << x << endl;

// reference
// https://en.wikipedia.org/wiki/STL_(file_format)

const char* LoaderStl::_ext = "stl";

bool LoaderStl::parse_face(Tokenizer& tkn, Vec3f& n, Vec3f& v1, Vec3f& v2, Vec3f& v3)
{
  if (not tkn.expecting("normal")) throw new StrException("expected token: \"normal\"");
  if (not tkn.getVec3f(n)) throw new StrException("expected Vec3f as normal");
  if (not tkn.expecting("outer")) throw new StrException("expected token: \"outer\"");
  if (not tkn.expecting("loop")) throw new StrException("expected token: \"loop\"");
  for (Vec3f* v : {&v1, &v2, &v3}) {
    if (not tkn.expecting("vertex")) throw new StrException("expected token: \"vertex\"");
    Vec3f tmp;
    if (not tkn.getVec3f(tmp)) throw new StrException("expected Vec3f");
    *v = tmp;
  }
  if (not tkn.expecting("endloop")) throw new StrException("expected token: \"endloop\"");
  if (not tkn.expecting("endfacet")) throw new StrException("expected token: \"endfacet\"");
  return true;
}

bool LoaderStl::load(const char* filename, SceneGraph& wrl) {
  bool success = false;

  // clear the scene graph
  wrl.clear();
  wrl.setUrl("");

  FILE* fp = (FILE*)0;
  try {
    // open the file
    if(filename==(char*)0) throw new StrException("filename==null");
    fp = fopen(filename,"r");
    if(fp==(FILE*)0) throw new StrException("fp==(FILE*)0");
    // DBG(2)
    // use the io/Tokenizer class to parse the input ascii file

    TokenizerFile tkn(fp);
    // first token should be "solid"
    if(tkn.expecting("solid") && tkn.get()) {
      string stlName = tkn; // second token should be the solid name

      // TODO ...

      // create the scene graph structure :
      // SceneGraph* scene_graph;
      // 1) the SceneGraph should have a single Shape node a child
      Shape* shape = new Shape();
      // scene_graph->addChild(shape);
      wrl.addChild(shape);
      // 2) the Shape node should have an Appearance node in its appearance field
      Appearance* appearance = new Appearance();
      shape->setAppearance(appearance);
      // 3) the Appearance node should have a Material node in its material field
      Material* material = new Material();
      appearance->setMaterial(material);
      // 4) the Shape node should have an IndexedFaceSet node in its geometry node
      IndexedFaceSet* indexed_face_set = new IndexedFaceSet();
      shape->setGeometry(indexed_face_set);

      // from the IndexedFaceSet
      // 5) get references to the coordIndex, coord, and normal arrays
      vector<int>& coordIndex = indexed_face_set->getCoordIndex(); 
      vector<float>& coord = indexed_face_set->getCoord();
      vector<float>& normal = indexed_face_set->getNormal();
      // 6) set the normalPerVertex variable to false (i.e., normals per face)  
      indexed_face_set->setNormalPerVertex(false);

      // the file should contain a list of triangles in the following format

      // facet normal ni nj nk
      //   outer loop
      //     vertex v1x v1y v1z
      //     vertex v2x v2y v2z
      //     vertex v3x v3y v3z
      //   endloop
      // endfacet

      // - run an infinite loop to parse all the faces
      // - write a private method to parse each face within the loop
      // - the method should return true if successful, and false if not
      // - if your method returns tru
      //     update the normal, coord, and coordIndex variables
      // - if your method returns false
      //     throw an StrException explaining why the method failed

      while (true) {
        if (not tkn.get()) throw new StrException("expected content");
        if (tkn.equals("endsolid")) break;
        if (not tkn.equals("facet")) throw new StrException("expected token: \"facet\"");

        Vec3f n, v1, v2, v3;
        parse_face(tkn, n, v1, v2, v3);

        for (Vec3f* v : {&v1, &v2, &v3}) {
          coordIndex.push_back(coord.size() / 3);
          coord.push_back(v->x);
          coord.push_back(v->y);
          coord.push_back(v->z);
        }
        coordIndex.push_back(-1);
        for (float a : {n.x, n.y, n.z}) normal.push_back(a);
      }
      // DBG(10)
      success = true;
    }

    // close the file (this statement may not be reached)
    fclose(fp);
    
  } catch(StrException* e) {
    if(fp!=(FILE*)0) fclose(fp);
    fprintf(stderr,"ERROR | %s\n",e->what());
    delete e;

  }

  return success;
}

