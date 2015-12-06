void MakeCFs()
{
  // Merge between datasets, then merge centralities
  
  
}

void MakeCFsForDataset(TFile &file, TString dataName)
{

  
  TDirectory *cfDir = file.GetDirectory("CF");
  if(!cfDir) {
    cfDir = file.mkdir("CF");
  }
  TDirectory *countDir = file.GetDirectory("Count");
  if(!countDir) {
    countDir = file.mkdir("Count");
  }

  //Iterate over the nums and dens
  //For each pair, make a cf and save it to file
  TDirectory *numDir = file.GetDirectory("Num");
  if(!numDir) {
    numDir = file.mkdir("Num");
  }
  TDirectory *denDir = file.GetDirectory("Den");
  if(!denDir) {
    denDir = file.mkdir("Den");
  }

  //

}

void WriteObjectsToDir(TDirectory *cfDir, TDirectory *countDir, TH1D *cf, TVectorD &counts)
{
  cout<<"Writing objects to "<<cfDir->GetName()<<endl;
  cfDir->cd();
  cf->SetDirectory(0);
  cf->Write(cf->GetName(), TObject::kOverwrite);

  countDir->cd();
  TString countName = cf->GetName();
  countName.ReplaceAll("CF","Count");
  counts.Write(countName, TObject::kOverwrite);
}

TH1D* MakeACF(TH1D *num, TH1D *den, Int_t rebinNum, Double_t lowNorm, Double_t highNorm)
{
  // Make a correlation function out of a single num/den pair

  num->Sumw2();
  den->Sumw2();
  
  // Rebin the histograms
  num->Rebin(rebinNum);
  den->Rebin(rebinNum);

  Double_t numScale = num->Integral(num->FindBin(lowNorm), num->FindBin(highNorm));
  Double_t denScale = den->Integral(den->FindBin(lowNorm), den->FindBin(highNorm));

  if(numScale > 0) num->Scale(1./numScale);
  if(denScale > 0) den->Scale(1./denScale);

  TH1D *cf = (TH1D*)num->Clone();
  cf->Divide(den);

  TString cfName = num->GetName();
  cout<<"Num name:\t"<<cfName<<endl;
  cfName.ReplaceAll("Num","CF");
  cf->SetName(cfName);
  cout<<"CF Name is:\t"<< cf->GetName()<<endl;
  cf->SetTitle(cfName);
  return cf;
}

TVectorD GetNumCounts(TH1D *num, Double_t lowNorm, Double_t highNorm)
{
  Double_t numCounts = num->Integral(num->FindBin(lowNorm), num->FindBin(highNorm));
  TVectorD count(1);
  count[0] = numCounts;
  return count;
}

void TestMakeCF()
{
  //Test read hists, creating CF, writing CF & count to file
  
  TFile f("CFs.root", "update");
  TDirectory *dir = (TDirectory*)f.Get("All");
  TDirectory *numDir = (TDirectory*)dir->Get("Num");
  TDirectory *denDir = (TDirectory*)dir->Get("Den");

  TH1D* n1 = (TH1D*)numDir->Get("NumLamALam05");
  TH1D* d1 = (TH1D*)denDir->Get("DenLamALam05");
  cout<<"Num name should be:\t"<<n1->GetName()<<endl;
  cout<<"Num title is:\t"<<n1->GetTitle()<<endl;
  TH1D *cf = MakeACF(n1, d1, 4, 0.5, 0.7);
  cf->DrawCopy();
  
  
  TVectorD count = GetNumCounts(n1, 0.5, 0.7);  
  
  TFile out("Test.root","update");

  WriteObjectsToDir(&out, &out, cf, count);


}
