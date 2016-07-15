
void MakePrettyAxis(TAxis *axis)
{
  axis->SetNdivisions(505);
  axis->SetLabelSize(0.05);
  axis->SetTitleSize(0.05);
  axis->SetTitleOffset(0.75);
  axis->CenterTitle();
  axis->SetRangeUser(0., 1.);
}

void DrawSmearMatrixPretty(TH2F *matrix, TString histTitle, Bool_t isLogZ)
{

  // Make a canvas with a square pad
  //set a square virtual canvas size in a non-square canvas
  gStyle->SetOptStat(0);
  TCanvas *c1 = new TCanvas("c1","c1",200,50,800,800);
  c1->SetCanvasSize(500,500);
  c1->SetLogz(isLogZ);

  // Pretty Axes
  MakePrettyAxis(matrix->GetXaxis());
  MakePrettyAxis(matrix->GetYaxis());
  matrix->GetXaxis()->SetTitle("k*_{gen}");
  matrix->GetYaxis()->SetTitle("k*_{rec}");
  matrix->SetTitle(histTitle);
  
  // Save plots to a directory 
  matrix->SetDirectory(0);
  matrix->Draw("colz");

}



void TestDraw()
{
  
  TFile f("MomentumResolutionMatrices.root");
  TDirectory *dir = (TDirectory*)f.GetDirectory("Normalized");
  TH2F* matrix = (TH2F*) dir->Get("fResMatrixLLMixedAllRebinNorm");

  TString histTitle = "Resolution Matrix #Lambda#Lambda";
  Bool_t isLogZ = kTRUE;
  DrawSmearMatrixPretty(matrix, histTitle, isLogZ);

}
