// #include <vector.h>
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Macro_Repository/RogerBarlowHelper.C"

void CompareCFLADiffCuts()
{
  TFile newFile("~/Analysis/lambda/AliAnalysisLambda/Results/2016-01/15-NoOppChargeCut/All/CFs.root");
  TDirectory *newDir = newFile.GetDirectory("Merged");
  vector<TH1D*> newCFs;
  newCFs.push_back((TH1D*)newDir->Get("CFLamALam010"));
  newCFs.push_back((TH1D*)newDir->Get("CFLamALam1030"));
  newCFs.push_back((TH1D*)newDir->Get("CFLamALam3050"));

  TFile oldFile("~/Analysis/lambda/AliAnalysisLambda/Results/2016-01/08-NewAvgSepCuts/All/CFs.root");
  TDirectory *oldDir = oldFile.GetDirectory("Merged");
  vector<TH1D*> oldCFs;
  oldCFs.push_back((TH1D*)oldDir->Get("CFLamALam010"));
  oldCFs.push_back((TH1D*)oldDir->Get("CFLamALam1030"));
  oldCFs.push_back((TH1D*)oldDir->Get("CFLamALam3050"));

  TFile compareFile("Compare.root","update");

  for(UInt_t i = 0; i < newCFs.size(); i++) {
    TH1D *ratio = (TH1D*)newCFs[i]->Clone();
    TString name = ratio->GetName();
    ratio->SetName(name + "Ratio");
    ratio->SetTitle(name + "Ratio");
    ratio->Divide(oldCFs[i]);

    TH1D *barlowRatio = ComputeRogerBarlowRatio(newCFs[i], oldCFs[i]);
    barlowRatio->SetName(name + "BarlowRatio");
    barlowRatio->SetTitle(name + "BarlowRatio");

    TH1D *barlowDifference = ComputeRogerBarlowDifference(newCFs[i], oldCFs[i]);
    barlowDifference->SetName(name + "BarlowDifference");
    barlowDifference->SetTitle(name + "BarlowDifference");
    
    compareFile.cd();
    ratio->Write();
    barlowRatio->Write();
    barlowDifference->Write();
  }
    

}
