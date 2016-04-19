// Take two correlation functions.
// For each k* bin, find the size of the error bars for each plot
// Take the ratio of C1error^2 / C2error^2 to find relative statistics
// Plot result in histogram.

void GetHistsAndCompareErrors(TH1D *h1, TH1D *h2)
{

  TString newName = h1->GetName();
  newName += "RelativeStats";
  TH1D *histRelative = (TH1D*) h1->Clone(newName);
  histRelative->Reset();
  histRelative->SetTitle(newName);
  histRelative->SetAxisRange(0.5, 1.5, "Y");

  if (h1->GetNbinsX() != h2->GetNbinsX()) {
    cout << "Histograms do not have the same number of bins. Cannot continue"
	 << endl;
    return;
  }

  // Find the relative statistics and fill the new histogram
  for (Int_t iBin = 1; iBin <= h1->GetNbinsX(); iBin++) {
    Double_t err1 = h1->GetBinError(iBin);
    Double_t err2 = h2->GetBinError(iBin);

    if (err1 < pow(10,-6)) continue;
    if (err2 < pow(10,-6)) continue;

    histRelative->SetBinContent(iBin, (err1*err1) / (err2*err2));
  }

  TFile outputFile ("RelativeStatistics.root", "update");
  outputFile.cd();
  histRelative->Write(histRelative->GetName(), TObject::kOverwrite);
}

void GetHistsAndCompareHists(TH1D *h1, TH1D *h2)
{

  if (h1->GetNbinsX() != h2->GetNbinsX()) {
    cout << "Histograms do not have the same number of bins. Cannot continue"
	 << endl;
    return;
  }
  
  TString newName = h1->GetName();
  newName += "Ratio";
  TH1D *histRatio = (TH1D*) h1->Clone(newName);
  histRatio->Divide(h2);
  histRatio->SetTitle(newName);
  histRatio->SetAxisRange(0.5, 1.5, "Y");

  TFile outputFile ("RelativeStatistics.root", "update");
  outputFile.cd();
  histRatio->Write(histRatio->GetName(), TObject::kOverwrite);

}

void CompareErrorSizes()
{
  TFile file1("/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-04/08-Train-TTCSys/CFs.root");
  TFile file2("/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-03/04-Train-SysCutChecks/CFs.root");

  vector<TString> cfNames = {"CFLamALam010", "CFLamALam1030", "CFLamALam3050",
			     "CFLLAA010", "CFLLAA1030", "CFLLAA3050"};

  for (UInt_t iName = 0; iName < cfNames.size(); iName++) {
    TString histPath1 = "Study0Var0/Cut1/Merged/" + cfNames[iName];
    TString histPath2 = "Var0/Cut1/Merged/" + cfNames[iName];
    TH1D *hist1 = (TH1D*) file1.Get(histPath1);
    TH1D *hist2 = (TH1D*) file2.Get(histPath2);
    if (!hist1) {
      cout << "Could not find hist1" << endl;
      return;
    }
    if (!hist2) {
      cout << "Could not find hist2" << endl;
      return;
    }
    GetHistsAndCompareErrors(hist1, hist2);
    GetHistsAndCompareHists(hist1, hist2);
  }
}
