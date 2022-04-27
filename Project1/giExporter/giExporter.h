/**
 * @file    giExporter.h
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
#include "giBaseExporter.h"

class giExporter : public giBaseExporter {

public:
  
  giExporter() = default;

  ~giExporter() override = default;

  /**
   * @brief 
   * @return 
   */
  static	void* 
  creator();
  
  /**
   * @brief 
   * @return 
   */
  MString			
  defaultExtension() const override;
  
  /**
   * @brief 
   * @param      obj 
   * @return 
   */
  MStatus			
  initializePlugin(MObject obj);
  
  /**
   * @brief 
   * @param      obj 
   * @return 
   */
  MStatus			
  uninitializePlugin(MObject obj);


 private:

  /**
   * @brief 
   * @param      dagPath 
   * @param      status 
   * @return 
   */
  giBaseWriter* 
  createWriter(const MDagPath dagPath, 
               MStatus& status) override;
 
  /**
   * @brief 
   * @param      os 
   */
  void
  writeHeader(std::ostream& os) override;
};
