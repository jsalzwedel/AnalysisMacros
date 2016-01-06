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
