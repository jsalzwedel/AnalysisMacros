// Compute ratios or differences of two correlated histograms.
// Use ComputeRogerBarlowDifference


Double_t *GetSigmaDelta(TH1D* h1, TH1D* h2)
{
  Double_t lh1NBins = h1->GetNbinsX(); 
  Double_t lh2NBins = h2->GetNbinsX(); 

  if( lh1NBins != lh2NBins ){ 
    cout<<"Problem! Number of bins doesn't match! "<<endl;
    return NULL;
  }

  Double_t *lSigmaDelta = new Double_t[(Int_t)lh1NBins]; 
  for( Int_t i = 1; i < lh1NBins + 1; i++){ 
    //Computation of roger barlow sigma_{delta} 
    lSigmaDelta[i] = TMath::Sqrt( TMath::Abs( TMath::Power(h1->GetBinError(i),2) - TMath::Power(h2->GetBinError(i),2) ) );
  }
  return lSigmaDelta;
}

// Double_t *GetSigmaRatio(TH1D* h1, TH1D* h2)
// {

// This is definitely not calculating things correctly.  Don't
// use it until you fix the math.

//   Double_t lh1NBins = h1->GetNbinsX(); 
//   Double_t lh2NBins = h2->GetNbinsX(); 

//   if( lh1NBins != lh2NBins ){ 
//     cout<<"Problem! Number of bins doesn't match! "<<endl;
//     return NULL;
//   }

//   Double_t *lSigmaRatio = new Double_t[(Int_t)lh1NBins];

//   for( Int_t i = 1; i < lh1NBins + 1; i++){ 
//     //Computation of roger barlow sigma_{Ratio} 
//     lSigmaRatio[i] = TMath::Sqrt( TMath::Abs( TMath::Power(h1->GetBinError(i),2) - TMath::Power(h2->GetBinError(i),2) ) );
//     //Computation of relationship to h2 for plotting in ratio plot 
//     if ( h2->GetBinContent(i) > 1e-12 ){ 
//       lSigmaRatio[i] /= h2->GetBinContent(i); 
//     }else{ 
//       lSigmaRatio[i] = 0; 
//     }
//   }
//   return lSigmaRatio;
// }


TH1D* ComputeRogerBarlowDifference( TH1D* h1, TH1D *h2 )
{ 
  //Use Roger Barlow "sigma_{delta}" as errors for difference in histograms
  Double_t *lSigmaDelta = GetSigmaDelta(h1,h2);
  TH1D *hDifference = (TH1D*)h1->Clone("Difference");
  //Regular histogram subtraction
  hDifference->Add(h2,-1.);
  //replace errors
  for( Int_t i=1; i<hDifference->GetNbinsX()+1; i++){ 
    hDifference->SetBinError(i, lSigmaDelta[i]);
  }
  delete[] lSigmaDelta;
  return hDifference;
}

TH1D* ComputeRogerBarlowRatio( TH1D* h1, TH1D *h2 )
{ 
  // //Use Roger Barlow "sigma_{delta}" as errors for ratios of histograms
  // Double_t *lSigmaDelta = GetSigmaDelta(h1,h2);
  // TH1D *hRatio = (TH1D*)h1->Clone("Ratio");
  // //Regular histogram division
  // hRatio->Divide(h2);
  // //Replace errors
  // for( Int_t i=1; i<hRatio->GetNbinsX()+1; i++){
  //   // cout<<i<<"\t"<<lSigmaDelta[i]<<endl;
  //   hRatio->SetBinError(i, lSigmaDelta[i]);
  // }
  // delete[] lSigmaDelta;
  // return hRatio;

  cout<<"RogerBarlowRatio not implemented.  Use Difference instead"<<endl;
  return NULL;
}


