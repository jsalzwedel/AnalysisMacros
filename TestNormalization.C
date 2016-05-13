void DrawProjectionForHist(TString fileName, TString histName, Bool_t axisIsY)
{

  TFile f(fileName);
  TH2F *hist = (TH2F*)f.Get(histName);
  if(!hist) {
    cout << "Could not find hist named " << histName << endl;
    return;
  }

  // for (Int_t iYBin = 1; iYBin <= hist->GetNbinsY(); iYBin++)
  // {
  //   TH1D *
  // }
  TH1D *proj = NULL;
  if(axisIsY) {
    proj = hist->ProjectionY("_py", 1, hist->GetNbinsX());
  } else {
    proj = hist->ProjectionX("_px", 1, hist->GetNbinsY());
  }
  TCanvas *c1 = new TCanvas(histName, histName);
  proj->DrawCopy();

  TFile out("NormTest.root", "update");
  proj->Write(proj->GetName(), TObject::kOverwrite);

}

void OutputSummedBinContents(TString fileName, TString histName, Bool_t axisIsY) {
  cout << "\n\n\n\nResults for histogram " << histName << endl;

  
  TFile f(fileName);
  TH2F *hist = (TH2F*)f.Get(histName);
  if(!hist) {
    cout << "Could not find hist named " << histName << endl;
    return;
  }

  for (Int_t iBinY = 1; iBinY <= hist->GetNbinsY(); iBinY++) {
    Double_t sum = 0.;
    for(Int_t iBinX = 1; iBinX <= hist->GetNbinsX(); iBinX++) {
      if(axisIsY) sum += hist->GetBinContent(iBinX, iBinY);
      else sum += hist->GetBinContent(iBinY, iBinX);
    }
    cout <<  "For row/column " << iBinY << ", sum = " << sum << endl;
    
  }

}

void TestNormalization()
{

  DrawProjectionForHist("SmearHistograms.root", "SmearMatrixSigmaLambdaNormLLAA", kFALSE);
  DrawProjectionForHist("NormalizedTransformMatrices.root", "Normalized/TransformMatrixSigmaLambdaNorm", kFALSE);
  DrawProjectionForHist("MomentumResolutionMatrices.root", "Normalized/fResMatrixLLAAMixedAllRebinNorm", kTRUE);
  
  OutputSummedBinContents("SmearHistograms.root", "SmearMatrixSigmaLambdaNormLLAA", kFALSE);
  OutputSummedBinContents("NormalizedTransformMatrices.root", "Normalized/TransformMatrixSigmaLambdaNorm", kFALSE);
  OutputSummedBinContents("MomentumResolutionMatrices.root", "Normalized/fResMatrixLLAAMixedAllRebinNorm", kTRUE);
}




