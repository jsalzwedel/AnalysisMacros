// Description...

#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/RogerBarlowHelper.C"


Bool_t Chi2TestWithZero(TH1D* h1, pCutoff,
			Double_t fitRangeLow, Double_t fitRangeHigh)
{

  cout<<"\t\t********************Doing Chi2 test****************"<<endl;
  // Double_t fitRangeLow = 0.;
  // Double_t fitRangeHigh = 1.;
  
  TF1 *f = new TF1("f","0*x", fitRangeLow, fitRangeHigh);

  Int_t binLow = h1->FindBin(fitRangeLow);
  Int_t binHigh = h1->FindBin(fitRangeHigh);

  Int_t ndf = binHigh - binLow + 1;

  Double_t chi2 = h1->Chisquare(f);

  Double_t prob = TMath::Prob(chi2, ndf);
  cout<<"Chi2:\t"<<chi2
      <<".\tP-value:\t"<<prob
      <<endl<<endl;

  return (prob > pCutoff);
}

void FitWithConstant(TH1D* h1, TDirectory *outputDir,
			Double_t fitRangeLow, Double_t fitRangeHigh)
{
  // Double_t fitRangeLow = 0.;
  // Double_t fitRangeHigh = .4;
  TString fitName = h1->GetName;
  fitName += "DiffFit";
  TF1 *fit = new TF1(fitName,"[0]", fitRangeLow, fitRangeHigh);

  Int_t binLow = h1->FindBin(fitRangeLow);
  Int_t binHigh = h1->FindBin(fitRangeHigh);
  h1->Fit(fit, "R0");

  TString fitName = h1->GetName();
  fitName += "Fit";
  fit->SetName(fitName);
  fit->SetTitle(fitName);

  // TDirectory *dir = outputDir->GetDirectory("Fit");
  // if(!dir) dir = out->mkdir("Fit");
  outputDir->cd();
  fit->Write(f->GetName(), TObject::kOverwrite);
}


void AnalyzeSystematicsForHists(TH1D *referenceHist, TH1D *tweakHist
				TDirectory *diffDir, TString nameSuffix,
				Double_t fitRangeLow, Double_t fitRangeHigh)
{
  // Take two hists (reference hist with nominal cut values, and
  // tweak hist with altered cut values), make difference hist.
  // Then check if result is consistent with zero.  If not,
  // fit to find systematic difference

  Double_t pCutoff = 0.01;

  TString newName = referenceHist->GetName();
  TH1D *barlowDifference = ComputeRogerBarlowDifference(referenceHist, tweakHist);
  barlowDifference->SetName(newName + "BarlowDifference" + nameSuffix);
  barlowDifference->SetTitle(newName + "BarlowDifference" + nameSuffix);

  diffDir->cd();
  barlowDifference->Write(barlowDifference->GetName(), TObject::kOverwrite);

  Bool_t checkPass = Chi2TestWithZero(barlowDifference, fitRangeLow, fitRangeHigh);
  if(checkPass) {
    cout<<"This passes the cut!"<<endl;
  } else {
    cout<<"This fails the cut!"<<endl;
    FitWithConstant(barlowDifference, diffDir, fitRangeLow, fitRangeHigh);
  }



  //...

}

