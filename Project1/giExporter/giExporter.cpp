/**
 * @file    giExporter.cpp
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    24/04/2022
 * @brief   A basic description of the what do the doc.
 * @bug     No known Bugs.
 */
 
/**
 * @include
 */
#include <maya/MFnPlugin.h>
#include <maya/MDagPath.h>
#include <sstream>
#include "giExporter.h"
#include "giWriter.h"


void* 
giExporter::creator() {
  return new giExporter();
}


MString 
giExporter::defaultExtension() const {
  return MString("giModel");
}


MStatus 
initializePlugin(MObject obj) {
  MStatus status;
  MFnPlugin plugin(obj, PLUGIN_COMPANY, "4.5", "Any");

  // Register the translator with the system
  status = plugin.registerFileTranslator("giModel",
                                         "",
                                         giExporter::creator,
                                         "",
                                         "option1=1",
                                         true);
  if (!status) {
    status.perror("registerFileTranslator");
    return status;
  }

  return status;
}


MStatus 
uninitializePlugin(MObject obj) {
  MStatus   status;
  MFnPlugin plugin(obj);

  status = plugin.deregisterFileTranslator("giModel");
  if (!status) {
    status.perror("deregisterFileTranslator");
    return status;
  }

  return status;
}


void 
giExporter::writeHeader(std::ostream& os) {
  os<< "default: {" << endl;
}


giBaseWriter* 
giExporter::createWriter(const MDagPath dagPath, 
                         MStatus& status) {
  return new giWriter(dagPath, status);
}
