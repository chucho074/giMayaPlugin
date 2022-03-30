#pragma once
#include <string.h>
#include <maya/MIOStream.h>
#include <math.h>

#include <maya/MPxGeometryFilter.h>
#include <maya/MItGeometry.h>

#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>

#include <maya/MFnNumericAttribute.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnDependencyNode.h>

#include <maya/MPoint.h>
#include <maya/MMatrix.h>


#define McheckErr(stat,msg)		\
	if ( MS::kSuccess != stat ) {	\
		cerr << msg;				\
		return MS::kFailure;		\
	}

class spike : public MPxGeometryFilter
{
 public:
  
  spike() = default;

  ~spike() override;


  /**
   * @brief      .
   */
  static void * 
  creator();

  MStatus   	
  deform(MDataBlock& block,
         MItGeometry& iter,
         const MMatrix& mat,
         unsigned int 	multiIndex) override;

  /**
   * @brief      .
   */
  static MStatus 
  initialize();

  static MObject angle;		 // The input value.
  
  static MTypeId id;

  //MDagPath	fPath;

};

MTypeId spike::id(0x001386c5);

MObject spike::angle;