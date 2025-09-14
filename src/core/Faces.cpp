//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-04 22:09:56 gtaubin>
//------------------------------------------------------------------------
//
// Faces.cpp
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

#include <math.h>
#include "Faces.hpp"
  
Faces::Faces(const int nV, const vector<int>& coordIndex) {
  // Build _nV
  _nV = 0;
  for (int iV : coordIndex)
    if (iV > _nV) _nV = iV;

  // Build _coordIndex and _nF
  _coordIndex = coordIndex;
  int iF = 0;
  for (int& i : _coordIndex) if (i < 0) {
    iF++;
    i = -iF;
  }
  _nF = iF;

  // Build _startPosition
  if (_coordIndex.size() > 0) _startPosition.push_back(0);
  for (int i = 0; i < _coordIndex.size(); ++i) if (_coordIndex[i] < 0)
    _startPosition.push_back(i + 1);
  _startPosition.pop_back();
}

int Faces::getNumberOfVertices() const {
  return _nV;
}

int Faces::getNumberOfFaces() const {
  return _nF;
}

int Faces::getNumberOfCorners() const {
  return _coordIndex.size();
}

int Faces::getFaceSize(const int iF) const {
  if (notValidFaceIndex(iF)) return 0;

  int pos = _startPosition[iF];
  if (iF == _startPosition.size() - 1)
    return _coordIndex.size() - 1 - pos;
  else
    return _startPosition[iF + 1] - 1 - pos;
}

int Faces::getFaceFirstCorner(const int iF) const {
  if (notValidFaceIndex(iF)) return -1;

  return _startPosition[iF];
}

int Faces::getFaceVertex(const int iF, const int j) const {
  if (notValidFaceIndex(iF) or notValidCornerIndex(j)) return -1;

  int fSize = getFaceSize(iF);
  int pos = _startPosition[iF];
  if (j < pos or j >= pos + fSize) return -1;

  return _coordIndex[j];
}

int Faces::getCornerFace(const int iC) const {
  if (notValidCornerIndex(iC) or _coordIndex[iC] < 0) return -1;

  int j = iC;
  while (_coordIndex[j] >= 0) j++;

  int iF = _coordIndex[j];
  // iF \in [-nF, -1]

  iF = -iF; // [1, nF]
  iF--;     // [0, nF)
  return iF;
}

int Faces::getNextCorner(const int iC) const {
  if (notValidCornerIndex(iC) or _coordIndex[iC] < 0) return -1;

  if (_coordIndex[iC + 1] >= 0) return iC + 1;

  int iF = getCornerFace(iC);
  return _startPosition[iF];
}

bool Faces::notValidFaceIndex(const int iF) const {
  return iF < 0 or iF >= _nF;
}

bool Faces::notValidCornerIndex(const int iC) const {
  return iC < 0 or iC >= _coordIndex.size();
}
