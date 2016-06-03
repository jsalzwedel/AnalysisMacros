TH2F *TransposeTH2(TH2 *hist, TString nameSuffix = "")
{
  // Return a transpose of hist

  // Get dimensions for new hist
  Int_t nBinsX = hist->GetNbinsY();
  Double_t lowXrange = hist->GetYaxis()->GetBinLowEdge(1);
  Double_t highXrange = hist->GetYaxis()->GetBinUpEdge(200);
  Int_t nBinsY = hist->GetNbinsX();
  Double_t lowYrange = hist->GetXaxis()->GetBinLowEdge(1);
  Double_t highYrange = hist->GetXaxis()->GetBinUpEdge(200);

  // Get name/title for new hist
  TString newName = hist->GetName() + nameSuffix;
  TString newTitle = hist->GetTitle() + nameSuffix;

  // Create empty histogram with correct new dimensions
  TH2F *transHist = new TH2F(newName, newTitle,
			     nBinsX, lowXrange, highXrange,
			     nBinsY, lowYrange, highYrange);
  transHist->SetEntries(hist->GetEntries());
  transHist->GetXaxis()->SetTitle(hist->GetYaxis()->GetTitle());
  transHist->GetYaxis()->SetTitle(hist->GetXaxis()->GetTitle());
  // Copy over elements (including overflow/underflow)
  for (Int_t iOldCol = 0; iOldCol < hist->GetNbinsX()+2; iOldCol++) {
    for (Int_t iOldRow = 0; iOldRow < hist->GetNbinsY()+2; iOldRow++) {
      Double_t val = hist->GetBinContent(iOldCol, iOldRow);
      Double_t err = hist->GetBinError(iOldCol, iOldRow);
      transHist->SetBinContent(iOldRow, iOldCol, val);
      transHist->SetBinError(iOldRow, iOldCol, err);
    }
  }

  return transHist;
}

void TransposeTransformMatrices()
{
  TFile file("~/Analysis/lambda/AliAnalysisLambda/Results/AnalysisResults/NormalizedTransformMatrices.root", "Update");
  TDirectory *inDir = file.GetDirectory("Normalized");


  // Loop over inDir and grab all histograms
  vector<TH2*> inputHists;
  TIter iter(inDir->GetListOfKeys());
  TObject *obj = NULL;
  while ((obj = iter())) {
    TKey *key = dynamic_cast<TKey*>(obj);
    if (!key) {
      cout << "Not a valid TKey" << endl;
      continue;
    }
    TH2* hist = dynamic_cast<TH2*>(key->ReadObj());
    if (!hist) {
      cout << "Object found, but it isnt a TH2" << endl;
      continue;
    }
    inputHists.push_back(hist);
  }

  // Get/make output directory
  TDirectory *outDir = file.GetDirectory("TransposedNorm");
  if (!outDir) {
    outDir = file.mkdir("TransposedNorm");
  }

  // Transpose and save all histograms
  for (UInt_t iHist = 0; iHist < inputHists.size(); iHist++) {
    TH2F *transposedHist = TransposeTH2(inputHists[iHist]);
    outDir->cd();
    transposedHist->Write(transposedHist->GetName(), TObject::kOverwrite);
    cout << "Wrote " <<transposedHist->GetName()
	 << "to " << file.GetName() << ":" << outDir->GetName()
	 << endl;
  }
}
