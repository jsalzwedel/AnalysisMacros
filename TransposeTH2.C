TH2 *TransposeTH2(TH2 *hist, TString nameSuffix = "")
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
  result->SetName(newName);
  result->SetTitle(newTitle);

  // Create empty histogram with correct new dimensions
  TH2F *transHist = new TH2F(newName, newTitle,
			     nBinsX, lowXrange, highXrange,
			     nBinsY, lowYrange, highYrange);

  // Copy over elements
  for (Int_t iOldCol = 1; iOldCol < hist->GetNbinsX(); iOldCol++) {
    for (Int_t iOldRow = 1; iOldRow < hist->GetNbinsY(); iOldRow++) {
      Double_t val = hist->GetBinContent(iOldCol, iOldRow);
      Double_t err = hist->GetBinError(iOldCol, iOldRow);
      transHist->SetBinContent(iOldRow, iOldCol, val);
      transHist->SetBinError(iOldRow, iOldCol, err);
    }
  }

  return transHist;
}
