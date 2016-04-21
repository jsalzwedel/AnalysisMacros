#include "AnalyzeSystematics.C"
#include "CombineSystematics.C"
#include "DrawCFWithErrors.C"
#include "CombineTGraphErrors.C"
#include "DefineEnums.C"

void FinalizeSystematics(Bool_t shouldAddInQuad) {

    // Compute systematic differences between cuts and fit
    // to look for significance.
  Bool_t useNSigmaTest = kTRUE;
  Double_t nSigmaCut = 2.;
  Double_t fitRangeLow = 0.0;
  Double_t fitRangeHigh = 1.;
  AnalyzeSystematics(nSigmaCut, useNSigmaTest, fitRangeLow, fitRangeHigh, kTopStudy);
  AnalyzeSystematics(nSigmaCut, useNSigmaTest, fitRangeLow, fitRangeHigh, kAvgSepStudy);
  
  CombineSystematics(shouldAddInQuad);
  DrawCFWithErrors(kTopStudy, shouldAddInQuad);
  DrawCFWithErrors(kAvgSepStudy, shouldAddInQuad);
  CombineTGraphErrors(shouldAddInQuad);
}
