void MakeMomResCF()
{
  TFile f("ResProjections.root");
  TDirectory *dirData = f.GetDirectory("ResolutionLL");
  TDirectory *dirNum = dirData->GetDirectory("kstarVsPtNum");
  TDirectory *dirDen = dirData->GetDirectory("kstarVsPtDen");

  TH1D *num = (TH1D*) dirNum->Get("kstarVsPt0.5to1");
  TH1D *den = (TH1D*) dirDen->Get("kstarVsPt0.5to1");
  
  TH1D *cf = (TH1D*) num->Clone("CF");
  cf->Divide(den);
  cf->Scale(10.);
  cf->DrawCopy();

}

void MakeUnslicedMomResCF()
{
  TFile f("Projections.root");
  TDirectory *dirData = f.GetDirectory("ResolutionLL");
  // TDirectory *dirNum = dirData->GetDirectory("kstarVsPtNum");
  // TDirectory *dirDen = dirData->GetDirectory("kstarVsPtDen");

  TH1D *num = (TH1D*) dirData->Get("fSignalMomResTruthVsPtLL_px");
  TH1D *den = (TH1D*) dirData->Get("fBkgMomResTruthVsPtLL_px");
  
  TH1D *cf = (TH1D*) num->Clone("CF");
  Double_t numScale = num->Integral(num->FindBin(1.), num->FindBin(1.4));
  Double_t denScale = den->Integral(den->FindBin(1.), den->FindBin(1.4));
  cf->Scale(1./numScale);
  den->Scale(1./denScale);
  cf->Divide(den);
  cf->DrawCopy();

}
