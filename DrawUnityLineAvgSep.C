#include "TFile.h"
#include "TDirectory.h"
// #include "TIter.h"
#include "TObject.h"
#include "TCanvas.h"
#include "TF1.h"
#include "TString.h"
#include "TKey.h"
#include "TH1D.h"
#include "TStyle.h"

void DrawUnityLineAvgSep(Bool_t isDrawLines = kTRUE)
{
  TFile f("AvgSep.root");
  TDirectory *dir = (TDirectory*)f.GetDirectory("AvgSepNew");

  TIter iter(dir->GetListOfKeys());
  TObject *obj = NULL;
  gStyle->SetOptStat(0);

  TCanvas *c1 = new TCanvas("AvgSep","AvgSep");
  c1->Divide(5,2,0.0000,0.01);

  TF1 *unity = new TF1("unity","1",0., 40.);
  unity->SetLineColor(kBlack);

  TString titles[] = {"p p",
		      "#pi- #pi-",
		      "#bar{p} #bar{p}",
		      "#pi+ #pi+",
		      "#bar{p} #pi-",
		      "p #pi+",
		      "p #pi-",
		      "#bar{p} #pi+",
		      "p #bar{p}",
		      "#pi- #pi+"};

  Double_t vert[] = {12, // pp
		     10, // pi- pi-
		     12, // pbar pbar
		     10,// pi+ pi+
		     10,// pbar pi-
		     10,// p pi+
		     15,// p pi-
		     15,// pbar pi+
		     10,// p pbar
		     25};// pi- pi+
		      
  gStyle->SetTitleSize(0.2,"t");
  Int_t iPad = 1;
  while( (obj = iter()) ) {
    TKey *key = dynamic_cast<TKey*>(obj);

    //Get Histo
    TH1D *hist = dynamic_cast<TH1D*>(key->ReadObj());
    if(!hist) continue;

    hist->SetTitle(titles[iPad-1]);
    hist->GetXaxis()->SetLabelSize(0.10);
    hist->GetYaxis()->SetLabelSize(0.075);
    hist->GetXaxis()->SetNdivisions(304);
    hist->GetYaxis()->SetNdivisions(304);
    hist->GetXaxis()->SetTitleSize(0.10); // Just move it off the plot to get it out of the way...
    hist->GetYaxis()->SetLabelOffset(0.02);

    c1->cd(iPad);
    // c1->cd(iPad)->SetTopMargin(0.05);
    // c1->cd(iPad)->SetBottomMargin(0.05);
    iPad++;

    hist->DrawCopy();

    if(!isDrawLines) continue;
    unity->Draw("same");

    TLine *l = new TLine(vert[iPad-2], 0.5, vert[iPad-2], 1.5);
    l->SetLineColor(kRed);
    l->Draw("same");
    
  }
}
