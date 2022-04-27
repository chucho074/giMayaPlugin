/**
 * @file    giBaseExporter.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    24/04/2022
 * @brief   A basic description of the what do the doc.
 * @bug     No known Bugs.
 */
 
/**
 * @include
 */
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>
#include <fstream>
#include <ios>
#include "giBaseExporter.h"
#include "giBaseWriter.h"


MStatus 
giBaseExporter::writer(const MFileObject& file,
                       const MString& options,
                       MPxFileTranslator::FileAccessMode mode) {
  const MString fileName = file.expandedFullName();

  std::ofstream newFile(fileName.asChar(), std::ios::out);
  if (!newFile) {
    MGlobal::displayError(fileName + ": could not be opened for reading");
    return MS::kFailure;
  }
  newFile.setf(std::ios::unitbuf);

  writeHeader(newFile);

  //Check which objects are to be exported, and invoke the corresponding
  //methods; only 'export all' and 'export selection' are allowed.
  if (MPxFileTranslator::kExportAccessMode == mode) {
    if (MStatus::kFailure == exportAll(newFile)) {
      return MStatus::kFailure;
    }
  }
  else if (MPxFileTranslator::kExportActiveAccessMode == mode) {
    if (MStatus::kFailure == exportSelection(newFile)) {
      return MStatus::kFailure;
    }
  }
  else {
    return MStatus::kFailure;
  }

  writeFooter(newFile);
  newFile.flush();
  newFile.close();

  MGlobal::displayInfo("Export to " + fileName + " successful!");
  return MS::kSuccess;
}


bool 
giBaseExporter::haveWriteMethod() const {
  return true;
}


bool 
giBaseExporter::haveReadMethod() const {
  return false;
}


bool 
giBaseExporter::canBeOpened() const {
  return true;
}


MStatus 
giBaseExporter::exportAll(std::ostream& os) {
  MStatus status;

  //Create an iterator for only the mesh components of the DAG.
  MItDag itDag(MItDag::kDepthFirst, MFn::kMesh, &status);

  if (MStatus::kFailure == status) {
    MGlobal::displayError("MItDag::MItDag");
    return MStatus::kFailure;
  }

  for (; !itDag.isDone(); itDag.next()) {
    //Get the current DAG path.
    MDagPath dagPath;
    if (MStatus::kFailure == itDag.getPath(dagPath)) {
      MGlobal::displayError("MDagPath::getPath");
      return MStatus::kFailure;
    }

    MFnDagNode visTester(dagPath);

    //If this node is visible, then process the poly mesh it represents.
    if (isVisible(visTester, status) && MStatus::kSuccess == status) {
      if (MStatus::kFailure == processMesh(dagPath, os)) {
        return MStatus::kFailure;
      }
    }
  }
  return MStatus::kSuccess;
}


MStatus 
giBaseExporter::exportSelection(std::ostream& os) {
  MStatus status;

  //Create an iterator for the selected mesh components of the DAG.
  MSelectionList selectionList;
  if (MStatus::kFailure == MGlobal::getActiveSelectionList(selectionList)) {
    MGlobal::displayError("MGlobal::getActiveSelectionList");
    return MStatus::kFailure;
  }

  MItSelectionList itSelectionList(selectionList, MFn::kMesh, &status);
  if (MStatus::kFailure == status) {
    return MStatus::kFailure;
  }

  for (itSelectionList.reset(); !itSelectionList.isDone(); itSelectionList.next()) {
    MDagPath dagPath;

    //Get the current dag path and process the poly mesh on it.
    if (MStatus::kFailure == itSelectionList.getDagPath(dagPath)) {
      MGlobal::displayError("MItSelectionList::getDagPath");
      return MStatus::kFailure;
    }

    if (MStatus::kFailure == processMesh(dagPath, os)) {
      return MStatus::kFailure;
    }
  }
  return MStatus::kSuccess;
}


MStatus 
giBaseExporter::processMesh(const MDagPath dagPath, 
                                std::ostream& os) {

  MStatus status;
  giBaseWriter* pWriter = createWriter(dagPath, status);
  if (MStatus::kFailure == status) {
    delete pWriter;
    return MStatus::kFailure;
  }
  if (MStatus::kFailure == pWriter->extractGeometry()) {
    delete pWriter;
    return MStatus::kFailure;
  }
  if (MStatus::kFailure == pWriter->writeToFile(os)) {
    delete pWriter;
    return MStatus::kFailure;
  }
  delete pWriter;
  return MStatus::kSuccess;
}


bool 
giBaseExporter::isVisible(MFnDagNode& fnDag, 
                          MStatus& status) {
  if (fnDag.isIntermediateObject())
    return false;

  MPlug visPlug = fnDag.findPlug("visibility", true, &status);
  if (MStatus::kFailure == status) {
    MGlobal::displayError("MPlug::findPlug");
    return false;
  }
  else {
    bool visible;
    status = visPlug.getValue(visible);
    if (MStatus::kFailure == status) {
      MGlobal::displayError("MPlug::getValue");
    }
    return visible;
  }
}


void 
giBaseExporter::writeHeader(std::ostream& os) {
  os << "";
}


void 
giBaseExporter::writeFooter(std::ostream& os) {
  os << "";
}
