// #include <vector.h>
#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Macro_Repository/RogerBarlowHelper.C"


void Chi2TestWithZero(TH1D* h1)
{

  cout<<"\t\t********************Doing Chi2 test****************"<<endl;
  Double_t fitRangeLow = 0.;
  Double_t fitRangeHigh = 1.;
  TF1 *f = new TF1("f","0*x", fitRangeLow, fitRangeHigh);

  Int_t binLow = h1->FindBin(fitRangeLow);
  Int_t binHigh = h1->FindBin(fitRangeHigh);

  Int_t ndf = binHigh - binLow + 1;

  Double_t chi2 = h1->Chisquare(f);

  Double_t prob = TMath::Prob(chi2, ndf);
  cout<<"Chi2:\t"<<chi2
      <<".\tP-value:\t"<<prob
      <<endl<<endl;
}

void FitWithConstant(TH1D* h1, TFile *out)
{
  Double_t fitRangeLow = 0.;
  Double_t fitRangeHigh = .4;
  TF1 *f = new TF1("f","[0]", fitRangeLow, fitRangeHigh);

  Int_t binLow = h1->FindBin(fitRangeLow);
  Int_t binHigh = h1->FindBin(fitRangeHigh);
  cout<<"\t\t********************Fitting with a constant value********************"<<endl<<endl;;
  h1->Fit(f, "R0");

  TString fitName = h1->GetName();
  fitName += "Fit";
  f->SetName(fitName);
  f->SetTitle(fitName);

  // TFile out("Compare.root","update");
  TDirectory *dir = out->GetDirectory("Fit");
  if(!dir) dir = out->mkdir("Fit");
  dir->cd();
  // f->SetDirectory(0); 
  f->Write(f->GetName(), TObject::kOverwrite);
  // out->Close();
}

void LookAtMean(TH1D* h1)
{
  cout<<"\t\t********************Looking at histogram mean********************"
      <<endl;
  cout<<"Mean:\t"<<h1->GetMean(2)
      <<"\tError:\t"<<h1->GetMeanError(2)<<endl<<endl;
 
}

void RebinHist(TH1D* h1, TFile *out)
{
  // This rebinning method doesn't work
  // It needs to divide by the entries to make an average
  // Also, it doesn't include error weighting.
  cout<<"Rebinning histogram"<<endl;
  Int_t nBins = h1->GetNbinsX();
  TH1D *rebinHist = (TH1D*) h1->Clone();
  rebinHist->Rebin(nBins/2);

  // TFile out("Compare.root","update");
  TDirectory *dir = out->GetDirectory("Rebin");
  if(!dir) dir = out->mkdir("Rebin");
  dir->cd();

  TString histName = h1->GetName();
  histName += "Rebin";
  rebinHist->SetName(histName);
  rebinHist->SetTitle(histName);
  rebinHist->SetDirectory(0);
  rebinHist->Write(rebinHist->GetName(), TObject::kOverwrite);
  // out->Close();

}

void ManuallyRebin(TH1D* h1, TFile *out)
{
  cout<<"Taking weighted average"<<endl;
  Int_t nCombine = 40; // Merge 100 bins into 1

  Int_t totalBins = h1->GetNbinsX();
  Int_t finalBins = totalBins/nCombine;
  cout<<"Final N bins:\t"<<finalBins<<endl;
  TString newName = h1->GetName();
  newName += "WgtAvg";
  TH1D *newH = new TH1D(newName, newName, finalBins,
			h1->GetXaxis()->GetXmin(),
			h1->GetXaxis()->GetXmax());
  newH->SetLineColor(kGreen);

  for(Int_t iNewBin = 0; iNewBin < finalBins; iNewBin++)
  {
    Double_t weightedSum = 0.;
    Double_t weights = 0.;
    
    for(Int_t iOldBin = 1; iOldBin < nCombine+1; iOldBin++)
    {
      Int_t binNum = iNewBin * nCombine + iOldBin;
      Double_t errorSq = pow(h1->GetBinError(binNum), 2.);
      Double_t content = h1->GetBinContent(binNum);

      weights += 1./errorSq;
      weightedSum += (1./errorSq) * content;
    }

    Double_t avg = weightedSum/weights;
    Double_t newErr = 1./sqrt(weights);

    newH->SetBinContent(iNewBin + 1, avg);
    newH->SetBinError(iNewBin + 1, newErr);
  }

  TDirectory *dir = out->GetDirectory("WeightedAvg");
  if(!dir) {
    dir = out->mkdir("WeightedAvg");
  }
  dir->cd();
  newH->SetDirectory(0);
  newH->Write(newH->GetName(), TObject::kOverwrite);
  
  // dir->Close();
  
}



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

  TFile *compareFile = new TFile("Compare.root","update");
  TDirectory *dir = compareFile->GetDirectory("Delta");
  if(!dir) dir = compareFile->mkdir("Delta");
  for(UInt_t i = 0; i < newCFs.size(); i++) {
    // TH1D *ratio = (TH1D*)newCFs[i]->Clone();
    // TString name = ratio->GetName();
    // ratio->SetName(name + "Ratio");
    // ratio->SetTitle(name + "Ratio");
    // ratio->Divide(oldCFs[i]);

    // TH1D *barlowRatio = ComputeRogerBarlowRatio(newCFs[i], oldCFs[i]);
    // barlowRatio->SetName(name + "BarlowRatio");
    // barlowRatio->SetTitle(name + "BarlowRatio");

    TString name = newCFs[i]->GetName();
    TH1D *barlowDifference = ComputeRogerBarlowDifference(newCFs[i], oldCFs[i]);
    barlowDifference->SetName(name + "BarlowDifference");
    barlowDifference->SetTitle(name + "BarlowDifference");
    
    dir->cd();
    // ratio->Write();
    // barlowRatio->Write();
    barlowDifference->Write(barlowDifference->GetName(), TObject::kOverwrite);
    Chi2TestWithZero(barlowDifference);
    FitWithConstant(barlowDifference, compareFile);
    // LookAtMean(barlowDifference);
    RebinHist(barlowDifference, compareFile);
    ManuallyRebin(barlowDifference, compareFile);
  }
    
  compareFile->Close();
}


