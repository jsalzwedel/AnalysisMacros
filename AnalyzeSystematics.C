// Description...

#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/RogerBarlowHelper.C"


Bool_t Chi2TestWithZero(TH1D* h1, TDirectory *outputDir, Double_t acceptanceCutoff,
			Double_t fitRangeLow, Double_t fitRangeHigh, Bool_t useNSigmaTest)
{

  // Fit function and save TF1
  TString fitName = h1->GetName();
  fitName += "DiffFit";
  TF1 *fit = new TF1(fitName,"[0]", fitRangeLow, fitRangeHigh);
  h1->Fit(fit, "R0Q");

  Double_t chi2 = fit->GetChisquare();
  Int_t ndf = fit->GetNDF(); // subtract number of fit parameters 
  Double_t prob = fit->GetProb();
  Double_t fitValue = fit->GetParameter(0);
  Double_t fitError = fit->GetParError(0);
  Double_t nSigmas = 0;
  if(fabs(fitValue) > std::numeric_limits<Double_t>::epsilon() &&
     fabs(fitError) > std::numeric_limits<Double_t>::epsilon()) {
    nSigmas = fitValue/fitError;
  }


  

  Bool_t doesPass;
  if(!useNSigmaTest) {
    doesPass = (prob > acceptanceCutoff);
  } else {
    doesPass = (fabs(nSigmas) < acceptanceCutoff);
  }
  
  if(!doesPass) {
    cout<<"Chi2: "<<chi2
	<<".\tP-value: "<<prob
	<<".\tNDF: "<<ndf
	<<endl;
    cout<<"NSigmas: "<<nSigmas
	<<".\tFitValue: "<<fitValue
	<<".\tFitError: "<<fitError
	<<endl;
    outputDir->cd();
    fit->Write(fit->GetName(), TObject::kOverwrite);
  }
  return doesPass;
}

// void FitWithConstant(TH1D* h1, TDirectory *outputDir,
// 			Double_t fitRangeLow, Double_t fitRangeHigh)
// {
//   // Double_t fitRangeLow = 0.;
//   // Double_t fitRangeHigh = .4;
//   TString fitName = h1->GetName();
//   fitName += "DiffFit";

//   fit->SetName(fitName);
//   fit->SetTitle(fitName);

//   Int_t binLow = h1->FindBin(fitRangeLow);
//   Int_t binHigh = h1->FindBin(fitRangeHigh);
//   h1->Fit(fit, "R0");
  
//   // TDirectory *dir = outputDir->GetDirectory("Fit");
//   // if(!dir) dir = out->mkdir("Fit");
//   outputDir->cd();
//   fit->Write(fit->GetName(), TObject::kOverwrite);
// }


void AnalyzeSystematicsForHists(TH1D *referenceHist, TH1D *tweakHist,
				TDirectory *diffDir, TString nameSuffix,
				Double_t acceptanceCutoff, Bool_t useNSigmaTest,
				Double_t fitRangeLow, Double_t fitRangeHigh)
{
  // Take two hists (reference hist with nominal cut values, and
  // tweak hist with altered cut values), make difference hist.
  // Then check if result is consistent with zero.  If not,
  // fit to find systematic difference
  // cout<<"Analyzing result"<<endl;

  TString newName = referenceHist->GetName();
  TH1D *barlowDifference = ComputeRogerBarlowDifference(referenceHist, tweakHist);
  barlowDifference->SetName(newName + "BarlowDifference" + nameSuffix);
  barlowDifference->SetTitle(newName + "BarlowDifference" + nameSuffix);
  barlowDifference->SetDirectory(0);
  diffDir->cd();
  barlowDifference->Write(barlowDifference->GetName(), TObject::kOverwrite);

  Bool_t checkPass = Chi2TestWithZero(barlowDifference, diffDir, acceptanceCutoff, 
				      fitRangeLow, fitRangeHigh, useNSigmaTest);
  if(checkPass) {
    cout<<"This passes the cut!"<<endl<<endl;
  } else {
    cout<<"This fails the cut!"<<endl<<endl;
    // FitWithConstant(barlowDifference, diffDir, fitRangeLow, fitRangeHigh);
  }
}

vector<TString> GetUserVarDirectoryNames()
{
  // These are all the directories that have cuts to compare
  vector<TString> names;
  for(Int_t i = 1; i < 11; i++) {
    TString thisName = "Var";
    thisName += i;
    names.push_back(thisName);
  }
  return names;
}

vector< vector<TString> > GetUserDirectoriesToCompare()
{
  // These are the names of the cuts (directories) that are getting
  // compared for each analysis.  The checks will be performed
  // in each "VarDirectory".
  vector< vector<TString> > pairNames;

  vector<TString> pair1 = {"Cut2", "Cut1"};
  vector<TString> pair2 = {"Cut2", "Cut3"};

  pairNames.push_back(pair1);
  pairNames.push_back(pair2);
  return pairNames;
}

vector<TString> GetUserHistNames()
{
  vector<TString> names = {"Merged/CFLamALam010", "Merged/CFLamALam1030", "Merged/CFLamALam3050",
			   "Merged/CFLLAA010", "Merged/CFLLAA1030", "Merged/CFLLAA3050"};

  return names;
}

TH1D *GetHistogram(TDirectory *dir, TString subFolderName, TString histName)
{
  TString path = subFolderName;
  path += "/";
  path += histName;
  // cout<<"Getting histogram at "<<path<<endl;

  TH1D *hist = (TH1D*) dir->Get(path);
  if(!hist) {
    cout<<"Could not find histogram at "<<path<<endl;
    return NULL;
  }
  // cout<<"Histogram acquired!"<<endl;
  return hist;
}

void AnalyzeSystematics(Double_t acceptanceCutoff = 0.05, Bool_t useNSigmaTest = kTRUE, Double_t fitRangeLow = 0.0, Double_t fitRangeHigh = 0.4)
{
  

  // Open CFs.root.
  TFile file("CFs.root", "update");

  vector<TString> varNames = GetUserVarDirectoryNames();
  vector< vector<TString> > checkPairs = GetUserDirectoriesToCompare();
  vector<TString> histNames = GetUserHistNames();

  
  // Sequentially navigate to each Var directory.
  for(UInt_t iVar = 0; iVar < varNames.size(); iVar++) {
    TDirectory *varDir = file.GetDirectory(varNames[iVar]);
    if(!varDir) {
      cout<<"Could not find directory named "<<varNames[iVar]<<endl;
      return;
    }

    TDirectory *diffOutputDir = varDir->GetDirectory("Diff");
    if(!diffOutputDir) {
      cout<<"Making new directory in "<<varNames[iVar]<<endl;
      diffOutputDir = varDir->mkdir("Diff");
    }
    // Do all the necessary checks for each pair of cuts in this variable
    for(UInt_t iCutPair = 0; iCutPair < checkPairs.size(); iCutPair++) {
      // Do checks for each species/centrality
      cout<<"iCutPair = "<<iCutPair<<endl;
      for(UInt_t iSpecies = 0; iSpecies < histNames.size(); iSpecies++) {
	cout<<"iSpecies = "<<iSpecies<<endl;
	TH1D *hist1 = GetHistogram(varDir, checkPairs[iCutPair][0], histNames[iSpecies]);
	TH1D *hist2 = GetHistogram(varDir, checkPairs[iCutPair][1], histNames[iSpecies]);
	if(!hist1) {
	  cout<<"Could not find hist at "<<checkPairs[iSpecies][0]<<" "<<histNames[iSpecies]<<endl;
	  continue;
	}
	if(!hist2) {
	  cout<<"Could not find hist at "<<checkPairs[iSpecies][1]<<" "<<histNames[iSpecies]<<endl;
	  continue;
	}
	TString nameSuffix = checkPairs[iCutPair][0];
	nameSuffix += checkPairs[iCutPair][1];
	AnalyzeSystematicsForHists(hist1, hist2,
				   diffOutputDir, nameSuffix,
				   acceptanceCutoff, useNSigmaTest,
				   fitRangeLow, fitRangeHigh);
      }// end species
    } // end cut pair
  } // end var directory
  
  // If more than one cut directory, grab the pairs of cut directories
  // that are relevant.
  // Grab the correlations functions to compare in each directory.
  // Determine a naming scheme for cut difference results.
  // Run each matching correlation function pair through
  // AnalyzeSystematicsForHists.
  // Save results to "Systematics" directory within Var directory


}
