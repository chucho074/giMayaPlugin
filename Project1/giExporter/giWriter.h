/**
 * @file    giWriter.h
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
#include <iosfwd>
#include "giBaseWriter.h"

//Used to store UV set information
struct UVSet {
  MFloatArray	uArray;
  MFloatArray	vArray;
  MString		  name;
  UVSet*      next;
};

class giWriter : public giBaseWriter {

 public:
  
  giWriter(const MDagPath& dagPath, 
           MStatus& status);
  
  ~giWriter() override;
  
  
  MStatus 
  extractGeometry() override;
  
  
  MStatus 
  writeToFile(std::ostream& os) override;

 private:
  
  MStatus	
  outputSingleSet(std::ostream& os,
                  MString setName,
                  MIntArray faces,
                  MString textureName) override;
  
  
  MStatus 
  outputFaces(std::ostream& os);
  
  
  MStatus 
  outputVertices(std::ostream& os);
  
  
  MStatus	
  outputVertexInfo(std::ostream& os);
  
  
  MStatus	
  outputNormals(std::ostream& os);
  
  
  MStatus	
  outputTangents(std::ostream& os);
  
  
  MStatus	
  outputBinormals(std::ostream& os);
  
  
  MStatus	
  outputColors(std::ostream& os);
  
  
  MStatus	
  outputUVs(std::ostream& os);

  //For storing UV information.
  UVSet* 
  fHeadUVSet;
};
