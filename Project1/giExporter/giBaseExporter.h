/**
 * @file    giBaseExporter.h
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
#include <maya/MPxFileTranslator.h>
#include <iosfwd>

class giBaseWriter;

class giBaseExporter :public MPxFileTranslator {

public:
  
  giBaseExporter() = default;
  
  ~giBaseExporter() override = default;

  /**
   * @brief 
   * @param      file 
   * @param      optionsString 
   * @param      mode 
   * @return 
   */
  MStatus			
  writer(const MFileObject& file,
         const MString& optionsString,
         MPxFileTranslator::FileAccessMode mode) override;
  
  /**
   * @brief 
   * @return 
   */
  bool			
  haveWriteMethod() const override;
  
  /**
   * @brief 
   * @return 
   */
  bool			
  haveReadMethod() const override;
  
  /**
   * @brief 
   * @return 
   */
  bool			
  canBeOpened() const override;

  /**
   * @brief 
   * @return 
   */
  MString			
  defaultExtension() const override = 0;


protected:
  /**
   * @brief 
   * @param      fnDag 
   * @param      status 
   * @return 
   */
  virtual	bool 
  isVisible(MFnDagNode& fnDag, MStatus& status);
  
  /**
   * @brief 
   * @param      os 
   * @return 
   */
  virtual	MStatus 
  exportAll(std::ostream& os);
  
  /**
   * @brief 
   * @param      os 
   * @return 
   */
  virtual	MStatus 
  exportSelection(std::ostream& os);
  
  /**
   * @brief 
   * @param      os 
   */
  virtual void 
  writeHeader(std::ostream& os);
  
  /**
   * @brief `
   * @param os 
   */
  virtual void 
  writeFooter(std::ostream& os);
  
  /**
   * @brief 
   * @param      dagPath 
   * @param      os 
   * @return 
   */
  virtual MStatus 
  processMesh(const MDagPath dagPath, 
                  std::ostream& os);
  
  /**
   * @brief 
   * @param      dagPath 
   * @param      status 
   * @return 
   */
  virtual giBaseWriter*
  createWriter(const MDagPath dagPath, 
                   MStatus& status) = 0;
};
