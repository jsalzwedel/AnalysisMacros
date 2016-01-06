
void WriteProjections(TDirectory *dir, vector<TH1D*> hists, TString subDirName)
{
  TDirectory *subDir = dir->GetDirectory(subDirName);
  if(!subDir) {
    subDir = dir->mkdir(subDirName);
  }

  subDir->cd();
  for(UInt_t i = 0; i < hists.size(); i++) {
    hists[i]->Write(hists[i]->GetName(), TObject::kOverwrite);
  }

}

vector<TH1D*> GetProjections(TH2D* h2)
{
  vector<TH1D*> hists;

  for(Int_t iProj = 1; iProj < 7; iProj++) {
    Double_t lowPt = 0.5 * iProj;
    Double_t highPt = 0.5 * (iProj + 1.);

    TString name = "kstarVsPt";
    name += lowPt;
    name += "to";
    name += highPt;
    TH1D *h1 = (TH1D*) h2->ProjectionX(name, h2->GetYaxis()->FindBin(lowPt), h2->GetYaxis()->FindBin(highPt));
    h1->SetTitle(name);
    hists.push_back(h1);
  }
  return hists;
}


void DrawProjections(vector<TH1D*> hists, TString histName)
{
  TString canvasName = histName;
  canvasName += "kstarTruth";
  TCanvas *c1 = new TCanvas(canvasName, canvasName);
  // c1->SetLogy();
  c1->Divide(3, 2, 0.0000, 0.01);

  for(Int_t iPad = 1; iPad < 7; iPad++) {
    TH1D *h1 = hists[iPad - 1];
    TPad *pad = (TPad*)c1->cd(iPad);
    h1->SetAxisRange(1., 150000., "Y");
    h1->GetXaxis()->SetLabelSize(0.07);
    h1->GetYaxis()->SetLabelSize(0.035);
    h1->GetXaxis()->SetNdivisions(405);
    h1->GetYaxis()->SetNdivisions(404);
    h1->GetXaxis()->SetTitleSize(0.10); // Just move it off the plot to get it out of the way...
    h1->GetYaxis()->SetLabelOffset(0.02);
    
    h1->DrawCopy();
    pad->SetLogy();
  }  
}

vector<TH1D*> DoProjectionsOf3DHist(TObjArray *arr, TString hist3DName)
{
  TH3D *hist3D = (TH3D*) arr->FindObject(hist3DName);
  // Let's look at just one of the pT axes
  // We'll make a projection of the other pT axis
  // hist3D->SetAxisRange(0.5,1.0,"Z");
  TH2D *hist2D = (TH2D*) hist3D->Project3D("YX");
  vector<TH1D*> hists1D = GetProjections(hist2D);
  DrawProjections(hists1D, hist3DName);

  return hists1D;
}

void RunOverDirectory(TList *list, TString dirSuffix)
{
  // Do analysis for one data directory
  TString dirName = "Resolution" + dirSuffix;
  TObjArray *arr = (TObjArray*) list->FindObject(dirName);



  TFile output("ResProjections.root","update");
  TDirectory *outDir = output.GetDirectory(dirName);
  if(!outDir) {
    outDir = output.mkdir(dirName);
  }


  TString hist3DNameNum = "fSignalMomResTruthVsPt";
  hist3DNameNum += dirSuffix;
  vector<TH1D*> hists1DNum = DoProjectionsOf3DHist(arr, hist3DNameNum);
  WriteProjections(outDir, hists1DNum, "kstarVsPtNum");

  TString hist3DNameDen = "fBkgMomResTruthVsPt";
  hist3DNameDen += dirSuffix;
  vector<TH1D*> hists1DDen = DoProjectionsOf3DHist(arr, hist3DNameDen);
  WriteProjections(outDir, hists1DDen, "kstarVsPtDen");
  
}


void PlotMomResProjections()
{
  TFile f("MyOutputAll.root","read");
  TList *list = (TList*) f.Get("MyList");

  RunOverDirectory(list, "LL");
  RunOverDirectory(list, "AA");
  RunOverDirectory(list, "LA");
  

}
