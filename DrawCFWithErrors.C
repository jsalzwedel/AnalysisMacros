#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/DefineEnums.C"

void GetAndDrawCF(TDirectory *dir, TString cfName, TString dirSuffix = "")
{
  TH1D *hist = (TH1D*)dir->Get(cfName);
  if(!hist) {
    cout << "Could not find histogram " << cfName << endl;
    return;
  }
  TGraphAsymmErrors *errors = (TGraphAsymmErrors*)dir->Get(cfName + "AsymmErrors");
  if(!errors) {
    cout << "Could not find TGraph for " << cfName << endl;
    return;
  }

  
  // Pretty up the plot
  hist->SetNdivisions(505,"xy");
  hist->SetAxisRange(0.8, 1.1, "y");
  hist->SetAxisRange(0., 0.5, "X");
  hist->SetMarkerStyle(kFullCircle);
  // hist->SetMarkerSize(0.8);

  errors->SetLineColor(kRed);
  errors->SetFillStyle(0);

  gStyle->SetOptStat(0);

  TCanvas c1(cfName + "Can", cfName);
  hist->Draw();
  errors->Draw("E2");

  // Check if system directory for output exists.
  // If it doesn't, make it.
  // Then save plots into that directory.
  TString outputBashDir = "Plots/" + dirSuffix;
  if (!gSystem->OpenDirectory(outputBashDir)) {
    gSystem->mkdir(outputBashDir, kTRUE);
  }
  
  TString outputName = outputBashDir + "/" + cfName + dirSuffix;
  // outputName = outputName + cfName + dirSuffix;
  c1.SaveAs(outputName + ".png");
  c1.SaveAs(outputName + ".eps");
  c1.SaveAs(outputName + ".pdf");
}



void DrawCFWithErrors(const StudyType sysStudyType, Bool_t isAddedQuadrature)
{

  TFile inputFile("SysErrors.root");
  TString dirName;
  if(sysStudyType == kTopStudy) {
    dirName = "TopologicalSystematics";
  } else if (sysStudyType == kAvgSepStudy) {
    dirName = "AvgSepSystematics";
  } else {
    cout << "Not a valid Study Type!" << endl;
    return;
  }
  TDirectory *dir = inputFile.GetDirectory(dirName);
  if (!dir) {
    cout << "Could not find " << dirName << endl;
    return;
  }


  vector<TString> cfNames = {"CFLamALam010", "CFLamALam1030", "CFLamALam3050",
			     "CFLLAA010", "CFLLAA1030", "CFLLAA3050"};
  if (isAddedQuadrature) {
    dirName += "Quadrature";
  } else {
    dirName += "Maximum";
  }
		    
  for (UInt_t iName = 0; iName < cfNames.size(); iName++) {
    
    GetAndDrawCF(dir, cfNames[iName], dirName);
  }
}
