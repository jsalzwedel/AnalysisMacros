#include <vector>




void DoComparison(TString newFileName, TString oldFileName, vector<TString> newHistName, vector<TString> oldHistName)
{
  TFile newF(newFileName,"read");
  TDirectory *newDir = newF.GetDirectory("Merged");

  TFile old(oldFileName);

  TFile output("Comparison.root","update");

  assert(newHistName.size() == oldHistName.size());


  vector<TH1D*> newHists;
  vector<TH1D*> oldHists;
  
  for(UInt_t i = 0; i < newHistName.size(); i++) {
    TH1D *newH = (TH1D*)newDir->Get(newHistName[i]);
    if(!newH) {
      cout<<"Could not find "<<newHistName[i]<< " in "<<newDir->GetName()<<endl;
      return;
    }
    TH1D *oldH = (TH1D*)old.Get(oldHistName[i]);
    if(!oldH) {
      cout<<"Could not find "<<oldHistName[i]<< " in "<<old.GetName()<<endl;
      return;
    }
    
    newHists.push_back(newH);
    oldHists.push_back(oldH);
  }

  for(UInt_t i = 0; i < newHists.size(); i++) {

    // Make ratio
    TH1D *ratio = (TH1D*)newHists[i]->Clone();
    ratio->Divide(oldHists[i]);

    ratio->SetDirectory(0);
    TString newName = newHists[i]->GetName();
    newName += "Ratio";
    ratio->SetName(newName);
    ratio->SetTitle(newName);
    ratio->SetMarkerStyle(20);
    output.cd();
    ratio->Write(newName, TObject::kOverwrite);
    cout<<"Wrote "<<newName<<" to "<< output.GetName()<<endl;

    // Make difference
    TH1D *difference = (TH1D*)newHists[i]->Clone();
    difference->Add(oldHists[i],-1.);
    difference->SetDirectory(0);
    newName = newHists[i]->GetName();
    newName += "Difference";
    difference->SetName(newName);
    difference->SetTitle(newName);
    difference->SetMarkerStyle(20);
    output.cd();
    difference->Write(newName, TObject::kOverwrite);
    cout<<"Wrote "<<newName<<" to "<< output.GetName()<<endl;

    // Make side by side plot
    TString canName = "Canvas";
    canName += newHists[i]->GetName();
    canName += "Comparison";
    TCanvas *can = new TCanvas(canName, canName);
    newHists[i]->SetLineColor(kBlack);
    newHists[i]->SetMarkerStyle(20);
    newHists[i]->SetMarkerColor(kBlack);
    oldHists[i]->SetLineColor(kRed);
    oldHists[i]->SetMarkerStyle(20);
    oldHists[i]->SetMarkerColor(kRed);
    oldHists[i]->Draw();
    newHists[i]->Draw("same");
    can->Write(canName, TObject::kOverwrite);
    delete can; can = NULL;
  }

}




void LLCompare()
{

  TString newFileName = "CFs.root";
  TString oldFileName = "~/Analysis/lambda/AliAnalysisLambda/Results/AnalysisResults/cfsLamLam.root";

  vector<TString> newHistName;
  newHistName.push_back("CFLamLam010");
  newHistName.push_back("CFLamLam1030");
  newHistName.push_back("CFLamLam3050");

  vector<TString> oldHistName;
  oldHistName.push_back("LamLam0-10");
  oldHistName.push_back("LamLam10-30");
  oldHistName.push_back("LamLam30-50");

  DoComparison(newFileName, oldFileName, newHistName, oldHistName);
}

void AACompare()
{

  TString newFileName = "CFs.root";
  TString oldFileName = "~/Analysis/lambda/AliAnalysisLambda/Results/AnalysisResults/cfsALamALam.root";

  vector<TString> newHistName;
  newHistName.push_back("CFALamALam010");
  newHistName.push_back("CFALamALam1030");
  newHistName.push_back("CFALamALam3050");

  vector<TString> oldHistName;
  oldHistName.push_back("ALamALam0-10");
  oldHistName.push_back("ALamALam10-30");
  oldHistName.push_back("ALamALam30-50");

  DoComparison(newFileName, oldFileName, newHistName, oldHistName);
  
}

void LACompare()
{

  TString newFileName = "CFs.root";
  TString oldFileName = "~/Analysis/lambda/AliAnalysisLambda/Results/2014-04/03-AvgSepSys-VaryProton-VaryNonIdent/11hcombined/cfsLamALamKstar.root";

  vector<TString> newHistName;
  newHistName.push_back("CFLamALam010");
  newHistName.push_back("CFLamALam1030");
  newHistName.push_back("CFLamALam3050");

  vector<TString> oldHistName;
  oldHistName.push_back("LamALam0-10centrality_varBin5BothFieldsKstar");
  oldHistName.push_back("LamALam10-30centrality_varBin5BothFieldsKstar");
  oldHistName.push_back("LamALam30-50centrality_varBin5BothFieldsKstar");

  DoComparison(newFileName, oldFileName, newHistName, oldHistName);
}




void CompareNewOld()
{
  LLCompare();
  AACompare();
  LACompare();
}
