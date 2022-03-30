#include "spikeNode.h"

spike::~spike() {}

void* spike::creator() {
  return new spike();
}

MStatus 
spike::deform(MDataBlock& block, 
              MItGeometry& iter, 
              const MMatrix& mat, 
              unsigned int multiIndex) {
  MStatus status = MS::kSuccess;

  // determine the angle of the yTwist
  //
  MDataHandle angleData = block.inputValue(angle, &status);
  McheckErr(status, "Error getting angle data handle\n");
  double magnitude = angleData.asDouble();

  // determine the envelope (this is a global scale factor)
  //
  MDataHandle envData = block.inputValue(envelope, &status);
  McheckErr(status, "Error getting envelope data handle\n");
  float env = envData.asFloat();

  // iterate through each point in the geometry
  //
  for (; !iter.isDone(); iter.next()) {

    MPoint pt = iter.position();

    // do the twist
    //
    double ff = magnitude * pt.y * env;
    if (ff != 0.0) {
      double cct = cos(ff);
      double cst = sin(ff);
      double tt = pt.x * cct - pt.z * cst;
      pt.z = pt.x * cst + pt.z * cct;
      pt.x = tt;;
    }

    iter.setPosition(pt);
  }
  return status;

  return MStatus();
}

//MStatus 
//spike::compute(const MPlug& plug, MDataBlock& data) {
//
//  MDagPath node;
//  MObject component;
//  MSelectionList list;
//  MFnDagNode nodeFn;
//
//
//  MGlobal::getActiveSelectionList(list);
//
//  for (MItSelectionList listIter(list); !listIter.isDone(); listIter.next()) {
//    listIter.getDagPath(node, component);
//    nodeFn.setObject(node);
//
//
//    MFnMesh model(node);
//    MFloatPointArray tmpPoints;
//    MFloatPointArray tmpPointsRandom;
//    model.getPoints(tmpPoints);
//    for (int i = 0; i < tmpPoints.length(); ++i) {
//      float tmpPoint[4];
//      tmpPoints[i].get(tmpPoint);
//      tmpPoint[rand() % 4 + 1] = rand() % 5 + 1;
//      tmpPointsRandom.append(tmpPoint[0],tmpPoint[1],tmpPoint[2], tmpPoint[3]);
//    }
//    model.setPoints(tmpPointsRandom);
//  }
//
//  return MStatus::kSuccess;
//}

MStatus 
spike::initialize() {
  

  MFnNumericAttribute nAttr;
  angle = nAttr.create("angle", "fa", MFnNumericData::kDouble);
  nAttr.setDefault(0.0);
  nAttr.setKeyable(true);
  addAttribute(angle);

  
  attributeAffects(spike::angle, spike::outputGeom);

  return MS::kSuccess;
}

// The initializePlugin method is called by Maya when the circleNode
// plugin is loaded.  It registers the circleNode which provides
// Maya with the creator and initialize methods to be called when
// a circleNode is created.
//
MStatus initializePlugin(MObject obj) {
  MStatus   status;
  MFnPlugin plugin(obj, 
                   PLUGIN_COMPANY, 
                   "4.5", 
                   "Any");

  status = plugin.registerNode("giEngine", 
                               spike::id,
                               spike::creator, 
                               spike::initialize,
                               MPxNode::kDeformerNode);

  if (!status) {
    status.perror("registerNode");
    return status;
  }

  return status;
}

// The unitializePlugin is called when Maya needs to unload the plugin.
// It basically does the opposite of initialize by calling
// the deregisterCommand to remove it.
//
MStatus uninitializePlugin(MObject obj) {
  MStatus   status;
  MFnPlugin plugin(obj);

  status = plugin.deregisterNode(spike::id);
  if (!status) {
    status.perror("deregisterNode");
    return status;
  }
  return status;
}
