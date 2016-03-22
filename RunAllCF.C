#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/MakeCFProjections.C"
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/MakeCFs.C"
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/MakeCombinedCFs.C"
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/RebinNumDen.C"
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/AnalyzeSystematics.C"



void RunAllCF(Bool_t isDataCompact, Bool_t isTrainResult, Bool_t usePValueTest = kTRUE)
{
  // Project out out the signal and denominator distributions
  MakeCFProjections(isDataCompact, isTrainResult);

  // Make CFs from the (unmerged) data
  Double_t lowNorm = 0.5;
  Double_t highNorm = 0.7;
  Int_t rebinNumber = 4;
  MakeCFs(isDataCompact, isTrainResult, lowNorm, highNorm, rebinNumber);

  // Combine fields, centralities and LLAA for each cut
  MakeCombinedCFs(isDataCompact, isTrainResult);

  // Rebin the numerator and denominator distributions
  // for use with log fitting
  RebinNumDen(isDataCompact, isTrainResult, rebinNumber);

  if(isTrainResult) {
    // Compute systematic differences between cuts and fit
    // to look for significance.
    Double_t pValueCutoff = 0.05;
    Double_t fitRangeLow = 0.0;
    Double_t fitRangeHigh = 0.2;
    AnalyzeSystematics(pValueCutoff, usePValueTest, fitRangeLow, fitRangeHigh);
  }
}
