/**
 * @file    giWriter.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    24/04/2022
 * @brief   A basic description of the what do the doc.
 * @bug     No known Bugs.
 */
 
/**
 * @include
 */
#include <maya/MIOStream.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MFnSet.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MPlug.h>
#include <maya/MIOStream.h>
#include <time.h>
#include <maya/MItMeshPolygon.h>
#include "giWriter.h"


//Macros
#define TAB "\t"
#define CLOSE_SECTION "}"
#define FINISH_LINE ";\n"
#define LINE "\n"


giWriter::giWriter(const MDagPath& dagPath, 
                   MStatus& status) : giBaseWriter(dagPath, status),
                                      fHeadUVSet(NULL) {
  
}


giWriter::~giWriter() {
  if (NULL != fHeadUVSet) {
    delete fHeadUVSet;
  }
}


MStatus 
giWriter::extractGeometry() {

  if (MStatus::kFailure == giBaseWriter::extractGeometry()) {
    return MStatus::kFailure;
  }

  MStringArray uvSetNames;
  if (MStatus::kFailure == fMesh->getUVSetNames(uvSetNames)) {
    MGlobal::displayError("MFnMesh::getUVSetNames");
    return MStatus::kFailure;
  }

  unsigned int uvSetCount = uvSetNames.length();
  unsigned int i;

  UVSet* currUVSet = NULL;

  for (i = 0; i < uvSetCount; i++) {
    if (0 == i) {
      currUVSet = new UVSet;
      fHeadUVSet = currUVSet;
    }
    else {
      currUVSet->next = new UVSet;
      currUVSet = currUVSet->next;
    }

    currUVSet->name = uvSetNames[i];
    currUVSet->next = NULL;

    // Retrieve the UV values
    if (MStatus::kFailure == fMesh->getUVs(currUVSet->uArray, 
                                           currUVSet->vArray, 
                                           &currUVSet->name)) {
      return MStatus::kFailure;
    }
  }

  return MStatus::kSuccess;
}


MStatus 
giWriter::writeToFile(ostream& os) {
  MGlobal::displayInfo("Exporting " + fMesh->partialPathName());

  os << TAB <<"Name: {"       << fMesh->partialPathName() << CLOSE_SECTION << FINISH_LINE;
  os << TAB <<"Triangles: {"  << fMesh->numPolygons() * 3 << CLOSE_SECTION << FINISH_LINE;
  os << TAB <<"Vertex : {"    << fVertexArray.length()    << CLOSE_SECTION << FINISH_LINE;
  os << TAB <<"Faces : {"     << fMesh->numPolygons()     << CLOSE_SECTION << FINISH_LINE;
  os << TAB <<"Normals : {"   << fNormalArray.length()    << CLOSE_SECTION << FINISH_LINE;
  os << TAB <<"Tangents : {"  << fTangentArray.length()   << CLOSE_SECTION << FINISH_LINE;
  os << TAB <<"Binormals : {" << fBinormalArray.length()  << CLOSE_SECTION << FINISH_LINE;
  os << TAB <<"UVs : {"       << fColorArray.length()     << CLOSE_SECTION << FINISH_LINE;
  os << CLOSE_SECTION;
  os << LINE;
  os << LINE;


  if (MStatus::kFailure == outputVertices(os)) {
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == outputFaces(os)) {
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == outputNormals(os)) {
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == outputTangents(os)) {
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == outputBinormals(os)) {
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == outputUVs(os)) {
    return MStatus::kFailure;
  }
  
  return MStatus::kSuccess;
}


MStatus 
giWriter::outputFaces(ostream& os) {
  unsigned int faceCount = fMesh->numPolygons();
  if (0 == faceCount) {
    return MStatus::kFailure;
  }

  MStatus status;
  MIntArray indexArray;

  os << "Faces: {" << LINE;

  unsigned int i;
  for (i = 0; i < faceCount; i++) {

    unsigned int indexCount = fMesh->polygonVertexCount(i, &status);
    if (MStatus::kFailure == status) {
      MGlobal::displayError("MFnMesh::polygonVertexCount");
      return MStatus::kFailure;
    }

    status = fMesh->getPolygonVertices(i, indexArray);
    if (MStatus::kFailure == status) {
      MGlobal::displayError("MFnMesh::getPolygonVertices");
      return MStatus::kFailure;
    }

    os << TAB << "{";
    unsigned int j;
    for (j = 0; j < indexCount; j++) {
      os << indexArray[j]; 
      if(j+1<indexCount) {
        os << " ";
      }
    }

    os << CLOSE_SECTION << FINISH_LINE;
  }
  os << CLOSE_SECTION;
  os << LINE << LINE;

  return MStatus::kSuccess;
}


MStatus 
giWriter::outputVertices(ostream& os) {
  unsigned int vertexCount = fVertexArray.length();
  unsigned i;

  if (0 == vertexCount) {
    return MStatus::kFailure;
  }

  os << "Vertex : {" << LINE;

  for (i = 0; i < vertexCount; i++) {
    os << TAB << "{" 
       << fVertexArray[i].x << "  " 
       << fVertexArray[i].y << "  " 
       << fVertexArray[i].z << 
       CLOSE_SECTION << FINISH_LINE;
  }

  os << CLOSE_SECTION << LINE;
  os << LINE;
  return MStatus::kSuccess;
}


MStatus 
giWriter::outputVertexInfo(ostream& os) {
  //unsigned int faceCount = fMesh->numPolygons();
  //unsigned i, j, indexCount;

  //MStatus status;
  //MIntArray indexArray;

  ////output the header
  //os << "Vertex Info:\n";
  //os << HEADER_LINE;
  //os << "Format:  Face|faceVertexIndex|vertexIndex|normalIndex|colorIndex|";

  ////Add each uv set to the header
  //UVSet* currUVSet;
  //for (currUVSet = fHeadUVSet; currUVSet != NULL; currUVSet = currUVSet->next) {
  //  os << "| UV_" << currUVSet->name;
  //}
  //os << "\n";

  //os << LINE;

  //MIntArray normalIndexArray;
  //int colorIndex, uvID;

  //for (i = 0; i < faceCount; i++) {

  //  indexCount = fMesh->polygonVertexCount(i, &status);
  //  if (MStatus::kFailure == status) {
  //    MGlobal::displayError("MFnMesh::polygonVertexCount");
  //    return MStatus::kFailure;
  //  }

  //  status = fMesh->getPolygonVertices(i, indexArray);
  //  if (MStatus::kFailure == status) {
  //    MGlobal::displayError("MFnMesh::getPolygonVertices");
  //    return MStatus::kFailure;
  //  }

  //  status = fMesh->getFaceNormalIds(i, normalIndexArray);
  //  if (MStatus::kFailure == status) {
  //    MGlobal::displayError("MFnMesh::getFaceNormalIds");
  //    return MStatus::kFailure;
  //  }

  //  for (j = 0; j < indexCount; j++) {
  //    status = fMesh->getFaceVertexColorIndex(i, j, colorIndex);

  //    //output the face, face vertex index, vertex index, normal index, color index
  //    //for the current vertex on the current face
  //    os << i << DELIMITER << j << DELIMITER << indexArray[j] << DELIMITER
  //      << normalIndexArray[j] << DELIMITER << colorIndex << DELIMITER;

  //    //output each uv set index for the current vertex on the current face
  //    for (currUVSet = fHeadUVSet; currUVSet != NULL; currUVSet = currUVSet->next) {
  //      status = fMesh->getPolygonUVid(i, j, uvID, &currUVSet->name);
  //      if (MStatus::kFailure == status) {
  //        MGlobal::displayError("MFnMesh::getPolygonUVid");
  //        return MStatus::kFailure;
  //      }
  //      os << DELIMITER << uvID;
  //    }
  //    os << "\n";
  //  }

  //  os << "\n";
  //}
  //os << "\n";

  return MStatus::kSuccess;
}


MStatus 
giWriter::outputNormals(ostream& os) {
  unsigned int normalCount = fNormalArray.length();
  if (0 == normalCount) {
    return MStatus::kFailure;
  }

  os << "Normals:  {"<< LINE;

  unsigned int i;
  for (i = 0; i < normalCount; i++) {
    os << TAB << "{"
      << fNormalArray[i].x << " "
      << fNormalArray[i].y << " "
      << fNormalArray[i].z << CLOSE_SECTION << FINISH_LINE;
  }
  os << CLOSE_SECTION << LINE << LINE;

  return MStatus::kSuccess;
}

MStatus 
giWriter::outputTangents(ostream& os) {
  unsigned int tangentCount = fTangentArray.length();
  if (0 == tangentCount) {
    return MStatus::kFailure;
  }

  os << "Tangents: {" << LINE;

  unsigned int i;
  for (i = 0; i < tangentCount; i++) {
    os << TAB << "{"
      << fTangentArray[i].x << " "
      << fTangentArray[i].y << " "
      << fTangentArray[i].z << CLOSE_SECTION << FINISH_LINE;
  }
  os << CLOSE_SECTION << LINE << LINE;

  return MStatus::kSuccess;
}

MStatus 
giWriter::outputBinormals(ostream& os) {
  unsigned int binormalCount = fBinormalArray.length();
  if (0 == binormalCount) {
    return MStatus::kFailure;
  }

  os << "Binormals: {" << LINE;

  unsigned int i;
  for (i = 0; i < binormalCount; i++) {
    os << TAB << "{"
      << fBinormalArray[i].x << " "
      << fBinormalArray[i].y << " "
      << fBinormalArray[i].z << CLOSE_SECTION << FINISH_LINE;
  }
  os << CLOSE_SECTION << LINE << LINE;

  return MStatus::kSuccess;
}

MStatus 
giWriter::outputColors(ostream& os) {
  /*unsigned int colorCount = fColorArray.length();
  if (0 == colorCount) {
    return MStatus::kFailure;
  }

  os << "Colors:  " << colorCount << "\n";
  os << HEADER_LINE;
  os << "Format:  Index|R G B A\n";
  os << LINE;

  unsigned int i;
  for (i = 0; i < colorCount; i++) {
    os << i << DELIMITER
      << fColorArray[i].r << " "
      << fColorArray[i].g << " "
      << fColorArray[i].b << " "
      << fColorArray[i].a << "\n";
  }
  os << "\n\n";*/

  return MStatus::kSuccess;
}


MStatus 
giWriter::outputUVs(ostream& os) {
  UVSet* currUVSet;
  unsigned int i, uvCount;
  for (currUVSet = fHeadUVSet; currUVSet != NULL; currUVSet = currUVSet->next) {
    if (currUVSet->name == fCurrentUVSetName) {
      os << "Current ";
    }

    os << "UV Set:  " << currUVSet->name << ": {" << LINE;
    uvCount = currUVSet->uArray.length();

    for (i = 0; i < uvCount; i++) {
      os << TAB << "{" << currUVSet->uArray[i] << "  " << currUVSet->vArray[i] << CLOSE_SECTION << FINISH_LINE;
    }
    os << CLOSE_SECTION << LINE;
  }
  return MStatus::kSuccess;
}


MStatus 
giWriter::outputSingleSet(ostream& os, 
                          MString setName, 
                          MIntArray faces, 
                          MString textureName) {
  /*unsigned int i;
  unsigned int faceCount = faces.length();

  os << "Set:  " << setName << "\n";
  os << HEADER_LINE;
  os << "Faces:  ";
  for (i = 0; i < faceCount; i++) {
    os << faces[i] << " ";
  }
  os << "\n";
  if (textureName == "") {
    textureName = "none";
  }
  os << "Texture File: " << textureName << "\n";
  os << "\n\n";*/
  return MStatus::kSuccess;
}
