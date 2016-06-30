#include "~/Analysis/lambda/AliAnalysisLambda/Results/macros/AnalyzeSystematics.C"
#include "~/Analysis/lambda/AliAnalysisLambda/Results/macros/CombineSystematics.C"
#include "~/Analysis/lambda/AliAnalysisLambda/Results/macros/DrawCFWithErrors.C"
#include "~/Analysis/lambda/AliAnalysisLambda/Results/macros/CombineTGraphErrors.C"
#include "~/Analysis/lambda/AliAnalysisLambda/Results/macros/DefineEnums.C"

void RunAllSystematics(Bool_t shouldAddInQuad) {

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
