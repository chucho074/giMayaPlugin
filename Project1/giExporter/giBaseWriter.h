/**
 * @file    giBaseWriter.h
 * @author  Jesus Alberto Del Moral Cupil
 * @e       idv18c.jmoral@uartesdigitales.edu.mx
 * @date    24/04/2022
 * @brief   A basic description of the what do the doc.
 * @bug     No known Bugs.
 */
 
/**
 * @include
 */
#pragma once
#include <maya/MFnMesh.h>
#include <maya/MPointArray.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFloatArray.h>

#include <iosfwd>

class giBaseWriter {
 public:

  giBaseWriter(MDagPath dagPath, 
                 MStatus& status);
  
  virtual				
  ~giBaseWriter();
  
  /**
   * @brief 
   * @return 
   */
  virtual MStatus		
  extractGeometry();
  
  /**
   * @brief 
   * @param      os 
   * @return 
   */
  virtual MStatus		
  writeToFile(std::ostream& os) = 0;

 protected:
  /**
   * @brief 
   * @param      setNode 
   * @return 
   */
  MObject		
  findShader(const MObject& setNode);
  

  virtual MStatus		
  outputSets(std::ostream& os);
  
  /**
   * @brief 
   * @param      os 
   * @param      setName 
   * @param      faces 
   * @param      textureName 
   * @return 
   */
  virtual	MStatus		
  outputSingleSet(std::ostream& os,
                  MString setName,
                  MIntArray faces,
                  MString textureName) = 0;
  
  /**
   * @brief 
   * @param      os 
   * @param      tabCount 
   */
  static	void		
  outputTabs(std::ostream& os, unsigned int tabCount);



  //The current UV set's name
  MString				
  fCurrentUVSetName;

  //For storing general mesh information
  MPointArray			
  fVertexArray;
  
  MColorArray			
  fColorArray;
  
  MFloatVectorArray	
  fNormalArray;
  
  MFloatVectorArray	
  fTangentArray;
  
  MFloatVectorArray	
  fBinormalArray;

  //For storing DAG objects
  MFnMesh* 
  fMesh;
  
  MDagPath* 
  fDagPath;
  
  MObjectArray		
  fPolygonSets;
  
  MObjectArray		
  fPolygonComponents;
};