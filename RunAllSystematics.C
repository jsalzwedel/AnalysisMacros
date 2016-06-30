#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/AnalyzeSystematics.C"
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/CombineSystematics.C"
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/DrawCFWithErrors.C"
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/CombineTGraphErrors.C"
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/DefineEnums.C"

void RunAllSystematics(Bool_t shouldAddInQuad) {

    // Compute systematic differences between cuts and fit
    // to look for significance.
  Bool_t useNSigmaTest = kTRUE;
  Double_t nSigmaCut = 2.;
  Double_t fitRangeLow = 0.0;
  Double_t fitRangeHigh = 1.;
  AnalyzeSystematics(nSigmaCut, useNSigmaTest, fitRangeLow, fitRangeHigh, kTopStudy);
  AnalyzeSystematics(nSigmaCut, useNSigmaTest, fitRangeLow, fitRangeHigh, kAvgSepStudy);

  TString topCutsPath = "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-05/07-FixedTOFTopCuts";
  TString avgSepCutsPath = "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-04/22-Train-FixedTOF";
  
  CombineSystematics(shouldAddInQuad, topCutsPath, avgSepCutsPath);
  DrawCFWithErrors(kTopStudy, shouldAddInQuad);
  DrawCFWithErrors(kAvgSepStudy, shouldAddInQuad);
  CombineTGraphErrors(shouldAddInQuad);
}
