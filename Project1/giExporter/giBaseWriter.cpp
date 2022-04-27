/**
 * @file    giBaseWriter.cpp
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
#include <maya/MFnSet.h>
#include <maya/MDagPath.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlug.h>
#include <maya/MItMeshPolygon.h>

#include "giBaseWriter.h"


giBaseWriter::giBaseWriter(MDagPath dagPath, 
                               MStatus& status) {
  fDagPath = new MDagPath(dagPath);
  fMesh = new MFnMesh(*fDagPath, &status);
}


giBaseWriter::~giBaseWriter() {
  if (NULL != fDagPath) {
    delete fDagPath;
  }
  if (NULL != fMesh) {
    delete fMesh;
  }
}


MStatus 
giBaseWriter::extractGeometry() {

  if (MStatus::kFailure == fMesh->getPoints(fVertexArray, MSpace::kWorld)) {
    MGlobal::displayError("MFnMesh::getPoints");
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == fMesh->getFaceVertexColors(fColorArray)) {
    MGlobal::displayError("MFnMesh::getFaceVertexColors");
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == fMesh->getNormals(fNormalArray, MSpace::kWorld)) {
    MGlobal::displayError("MFnMesh::getNormals");
    return MStatus::kFailure;
  }
  if (MStatus::kFailure == fMesh->getCurrentUVSetName(fCurrentUVSetName)) {
    MGlobal::displayError("MFnMesh::getCurrentUVSetName");
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == fMesh->getTangents(fTangentArray, MSpace::kWorld, &fCurrentUVSetName)) {
    MGlobal::displayError("MFnMesh::getTangents");
    return MStatus::kFailure;
  }

  if (MStatus::kFailure == fMesh->getBinormals(fBinormalArray, MSpace::kWorld, &fCurrentUVSetName)) {
    MGlobal::displayError("MFnMesh::getBinormals");
    return MStatus::kFailure;
  }

  //Have to make the path include the shape below it so that
  //we can determine if the underlying shape node is instanced.
  //By default, dag paths only include transform nodes.
  fDagPath->extendToShape();

  //If the shape is instanced then we need to determine which
  //instance this path refers to.
  int instanceNum = 0;
  if (fDagPath->isInstanced())
    instanceNum = fDagPath->instanceNumber();

  //Get the connected sets and members - these will be used to determine texturing of different
  //faces
  if (!fMesh->getConnectedSetsAndMembers(instanceNum, fPolygonSets, fPolygonComponents, true)) {
    MGlobal::displayError("MFnMesh::getConnectedSetsAndMembers");
    return MStatus::kFailure;
  }

  return MStatus::kSuccess;
}


void 
giBaseWriter::outputTabs(ostream& os, 
                           unsigned int tabCount) {
  unsigned int i;
  for (i = 0; i < tabCount; i++) {
    os << "\t";
  }
}


MObject 
giBaseWriter::findShader(const MObject& setNode) {

  MFnDependencyNode fnNode(setNode);
  MPlug shaderPlug = fnNode.findPlug("surfaceShader", true);

  if (!shaderPlug.isNull()) {
    MPlugArray connectedPlugs;

    //Get all the plugs that are connected as the destination of this 
    //surfaceShader plug so we can find the surface shaderNode
    MStatus status;
    shaderPlug.connectedTo(connectedPlugs, true, false, &status);
    if (MStatus::kFailure == status) {
      MGlobal::displayError("MPlug::connectedTo");
      return MObject::kNullObj;
    }

    if (1 != connectedPlugs.length()) {
      MGlobal::displayError("Error getting shader for: " + fMesh->partialPathName());
    }
    else {
      return connectedPlugs[0].node();
    }
  }

  return MObject::kNullObj;
}


MStatus 
giBaseWriter::outputSets(ostream& os) {
  MStatus status;

  //If there is more than one set, the last set simply consists of all 
  //polygons, so we won't include it
  unsigned int setCount = fPolygonSets.length();
  if (setCount > 1) {
    setCount--;
  }

  MIntArray faces;

  unsigned int i;
  for (i = 0; i < setCount; i++) {
    MObject set = fPolygonSets[i];
    MObject comp = fPolygonComponents[i];
    MFnSet fnSet(set, &status);
    if (MS::kFailure == status) {
      MGlobal::displayError("MFnSet::MFnSet");
      continue;
    }

    //Make sure the set is a polygonal set.  If not, continue.
    MItMeshPolygon itMeshPolygon(*fDagPath, comp, &status);

    if ((MS::kFailure == status)) {
      MGlobal::displayError("MItMeshPolygon::MItMeshPolygon");
      continue;
    }

    //Add the current set's face indices to the faces array
    //
    faces.setLength(itMeshPolygon.count());

    unsigned int j = 0;
    for (itMeshPolygon.reset(); !itMeshPolygon.isDone(); itMeshPolygon.next()) {
      faces[j++] = itMeshPolygon.index();
    }

    //Find the texture that is applied to this set.  First, get the
    //shading node connected to the set.  Then, if there is an input
    //attribute called "color", search upstream from it for a texture
    //file node.
    MObject shaderNode = findShader(set);
    if (MObject::kNullObj == shaderNode) {
      continue;
    }

    MPlug colorPlug = MFnDependencyNode(shaderNode).findPlug("color", true, &status);
    if (MS::kFailure == status) {
      MGlobal::displayError("MFnDependencyNode::findPlug");
      continue;
    }

    MItDependencyGraph itDG(colorPlug, MFn::kFileTexture,
      MItDependencyGraph::kUpstream,
      MItDependencyGraph::kBreadthFirst,
      MItDependencyGraph::kNodeLevel,
      &status);

    if (MS::kFailure == status) {
      MGlobal::displayError("MItDependencyGraph::MItDependencyGraph");
      continue;
    }

    //Disable automatic pruning so that we can locate a specific plug.
    itDG.disablePruningOnFilter();

    //If no texture file node was found, pass in an empty string as the texture filename 
    //so that color information is outputted instead.
    MString textureName("");
    if (itDG.isDone()) {
      if (MStatus::kFailure == outputSingleSet(os, MString(fnSet.name()), faces, textureName)) {
        return MStatus::kFailure;
      }
      //Otherwise retrieve the filename and pass it in to output texture information.
    }
    else {
      MObject textureNode = itDG.currentItem();
      MPlug filenamePlug = MFnDependencyNode(textureNode).findPlug("fileTextureName", true);
      filenamePlug.getValue(textureName);
      if (MStatus::kFailure == outputSingleSet(os, MString(fnSet.name()), faces, textureName)) {
        return MStatus::kFailure;
      }

    }
  }
  return MStatus::kSuccess;
}
