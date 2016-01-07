


TH1D *MakeCFFromNum(TDirectory *dir, TH1D *num)
{
  if(!num) {
    cout<<"Numerator does not exist"<<endl;
    return NULL;
  }
  if(!dir) {
    cout<<"Dir does not exist"<<endl;
    return NULL;
  }

  TString numName = num->GetName();

  TString denName = numName;
  denName.ReplaceAll("Signal","Bkg");

  TH1D *den = (TH1D*) dir->Get(denName);
  if(!den) {
    cout<<"Could not find denominator hist with name "<<denName<<endl;
    return NULL;
  }

  // num->Sumw2();
  // den->Sumw2();

  Int_t rebinNumber = 8;
  num->Rebin(rebinNumber);
  den->Rebin(rebinNumber);

  Double_t lowVal = 34.;
  Double_t highVal = 38.;
  Int_t lowBin = num->FindBin(lowVal);
  Int_t highBin = num->FindBin(highVal);

  // cout<<"LowBin:\t"<<lowBin<<".\tHighBin:\t"<<highBin<<endl;

  
  Double_t numScale = num->Integral(lowBin, highBin);
  Double_t denScale = den->Integral(lowBin, highBin);

  // cout<<"Num Scale:\t"<< numScale << ".\tDen Scale:\t"<<denScale
  //     <<".\tRatio:\t" << numScale/denScale <<endl;
  
  num->Scale(1./numScale);
  den->Scale(1./denScale);

  Double_t numScaleNew = num->Integral(lowBin, highBin);
  Double_t denScaleNew = den->Integral(lowBin, highBin);

  // cout<<"new Scale:\t"<< numScaleNew << ".\tDen Scale:\t"<<denScaleNew
  //     <<".\tRatio:\t" << numScaleNew/denScaleNew <<endl;

  
  TH1D *cf = (TH1D*)num->Clone();
  cf->Divide(den);

  TString cfName = numName;
  cfName.ReplaceAll("fSignal", "AvgSepCF");
  cfName.ReplaceAll("_px", "");
  cf->SetName(cfName);
  cf->SetTitle(cfName);

  cf->SetAxisRange(0.5, 1.5, "Y");

  return cf;
}

vector<TH1D*> GetNumsFromTDirectory(TDirectory *dir)
{
  // Loop over TDirectory and get all signal histograms from it.
  vector<TH1D*> nums;

  TIter iter(dir->GetListOfKeys());

  TObject *obj = NULL;

  while ( (obj = iter()) ) {
    // Loop over all the objects in the directory
    TKey *key = dynamic_cast<TKey*>(obj);
    
    // Get histogram
    TH1D *hist = dynamic_cast<TH1D*>(key->ReadObj());
    if(!hist) continue;

    // Check that it is a numerator histogram
    TString name = hist->GetName();
    if(!name.Contains("Signal")) {
      // delete hist; //???
      continue;
    }

    // Add it to the collection
    nums.push_back(hist);
  }
  return nums;
}


void MakeAvgSepCFsForDirectory(TString fileName, TString dirName)
{
  // Run this after making the projections
  TFile f(fileName, "read");
  TDirectory *inDir = f.GetDirectory(dirName);
  if(!inDir) {
    cout<<"No directory named "<<dirName<<" in "<<fileName<<endl;
    return;
  }
  
  TFile outputFile("AvgSep.root","update");
  TDirectory *outDir = outputFile.GetDirectory(dirName);
  if(!outDir) {
    outDir = outputFile.mkdir(dirName);
  }
  
  vector<TH1D*> nums = GetNumsFromTDirectory(inDir);

  for(UInt_t iNum = 0; iNum < nums.size(); iNum++) {
    TH1D *cf = MakeCFFromNum(inDir, nums[iNum]);
    if(!cf) {
      cout<<"Failed to make CF"<<endl;
      continue;
    }

    cf->SetDirectory(0);
    outDir->cd();
    cf->Write(cf->GetName(), TObject::kOverwrite);
    cout<<"Wrote "<<cf->GetName()<<" to "<<dirName<<" in "<<outDir->GetName()<<endl;
  }
  cout<<"Closing files"<<endl;
  
  outputFile.Close();
  f.Close();

}

void MakeAvgSepCFs()
{

  TString fileName = "Projections.root";
  MakeAvgSepCFsForDirectory(fileName, "AvgSepNew");
  MakeAvgSepCFsForDirectory(fileName, "AvgSepOld");

}
