

void GetAndDrawCF(TDirectory *dir, TString cfName, TString outputSuffix = "")
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

  TString outputName = "Plots/";
  outputName = outputName + cfName + outputSuffix;
  c1.SaveAs(outputName + ".png");
  c1.SaveAs(outputName + ".eps");
  c1.SaveAs(outputName + ".pdf");
}



void DrawCFWithErrors()
{

  TFile inputFile("SysErrors.root");
  TString dirName = "TopologicalSystematics";
  TDirectory *dir = inputFile.GetDirectory(dirName);
  if (!dir) {
    cout << "Could not find " << dirName << endl;
    return;
  }

  vector<TString> cfNames = {"CFLamALam010", "CFLamALam1030", "CFLamALam3050",
			     "CFLLAA010", "CFLLAA1030", "CFLLAA3050"};
  
  for (UInt_t iName = 0; iName < cfNames.size(); iName++) {
    GetAndDrawCF(dir, cfNames[iName]);
  }
}
