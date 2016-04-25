// Take two correlation functions.
// For each k* bin, find the size of the error bars for each plot
// Take the ratio of C1error^2 / C2error^2 to find relative statistics
// Plot result in histogram.

void GetHistsAndCompareErrors(TH1D *h1, TH1D *h2, TString outputSuffix)
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

  TString outputFileName = "RelativeStatistics" + outputSuffix + ".root";
  TFile outputFile (outputFileName, "update");
  outputFile.cd();
  histRelative->Write(histRelative->GetName(), TObject::kOverwrite);
}

void GetHistsAndCompareHists(TH1D *h1, TH1D *h2, TString outputSuffix)
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

  TString outputFileName = "RelativeStatistics" + outputSuffix + ".root";
  TFile outputFile (outputFileName, "update");
  outputFile.cd();
  histRatio->Write(histRatio->GetName(), TObject::kOverwrite);

}

void CompareErrorSizes(TString file1Name, TString file2Name, TString rootPath1, TString rootPath2, TString outputSuffix)
{
  TFile file1(file1Name);
  TFile file2(file2Name);

  vector<TString> cfNames = {"CFLamALam010", "CFLamALam1030", "CFLamALam3050",
			     "CFLLAA010", "CFLLAA1030", "CFLLAA3050"};

  for (UInt_t iName = 0; iName < cfNames.size(); iName++) {
    TString histPath1 = rootPath1 + cfNames[iName];
    TString histPath2 = rootPath2 + cfNames[iName];
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
    GetHistsAndCompareErrors(hist1, hist2, outputSuffix);
    GetHistsAndCompareHists(hist1, hist2, outputSuffix);
  }
}


void CheckTOFDifferences()
{
  // Check if there are any differences from before and after the TOF code was fixed
  TString file1Name = "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-04/22-Train-FixedTOF/CFs.root";
  TString file2Name = "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-04/08-Train-TTCSys/CFs.root";

  TString rootPath1 = "Study2Var0/Cut2/Merged/";
  TString rootPath2 = "Study2Var0/Cut2/Merged/";

  TString outputSuffix = "FixedTOF";

  CompareErrorSizes(file1Name, file2Name, rootPath1, rootPath2, outputSuffix);
}

void CheckRefactoringCodeDifferences()
{
  // Check if there are any differences in the results from
  // before and after the code restructuring.
  // Spoilers: there are difference in LL+AA results.
  TString file1Name = "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-04/08-Train-TTCSys/CFs.root";
  TString file2Name = "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-03/04-Train-SysCutChecks/CFs.root";

  TString rootPath1 = "Study0Var0/Cut1/Merged/";
  TString rootPath2 = "Var0/Cut1/Merged/";
  
  TString outputSuffix = "RefactoredCode";
  CompareErrorSizes(file1Name, file2Name, rootPath1, rootPath2, outputSuffix);
}
