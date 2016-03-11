
TVectorD GetNumCounts(TH1D *num, Double_t lowNorm, Double_t highNorm)
{
  Double_t numCounts = num->Integral(num->FindBin(lowNorm), num->FindBin(highNorm));
  TVectorD count(1);
  count[0] = numCounts;
  return count;
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
  cfName.ReplaceAll("Num","CF");
  cf->SetName(cfName);
  // cout<<"CF Name is:\t"<< cf->GetName()<<endl;
  cf->SetTitle(cfName);
  return cf;
}




void MakeCFsForDataset(TDirectory *dataDir, Int_t rebinNumber, Double_t lowNorm, Double_t highNorm)
{
  TDirectory *cfDir = dataDir->GetDirectory("CF");
  if(!cfDir) {
    cfDir = dataDir->mkdir("CF");
  }
  TDirectory *countDir = dataDir->GetDirectory("Count");
  if(!countDir) {
    countDir = dataDir->mkdir("Count");
  }

  //Iterate over the nums and dens
  //For each pair, make a cf and save it to file
  TDirectory *numDir = dataDir->GetDirectory("Num");
  if(!numDir) {
    cout<<"No Num directory in "<<dataDir->GetName()<<endl;
    return;
  }
  TDirectory *denDir = dataDir->GetDirectory("Den");
  if(!denDir) {
    cout<<"No Den directory "<<dataDir->GetName()<<endl;
    return;
  }

  // Grab each numerator and denominator in sequence
  TIter numIter(numDir->GetListOfKeys());
  TIter denIter(denDir->GetListOfKeys());

  TObject *numObj = NULL;
  TObject *denObj = NULL;

  while ((numObj = numIter())) {
    TKey *numKey = dynamic_cast<TKey*>(numObj);
    //Get the num and den hists
    TH1D *numHist = dynamic_cast<TH1D*>(numKey->ReadObj());
    if(!numHist) {
      cout<<"Could not find appropriate num hist"<<endl;
      return;
    }
    denObj = denIter();
    TKey *denKey = dynamic_cast<TKey*>(denObj);
    TH1D *denHist = dynamic_cast<TH1D*>(denKey->ReadObj());
    if(!denHist) {
      cout<<"Could not find appropriate den hist"<<endl;
      delete numHist; numHist = NULL;
      continue;
    }
    // Check that they match each other by comparing names
    TString numName = numHist->GetName();
    TString denName = denHist->GetName();
    TString numCopy = numName;
    numCopy.ReplaceAll("Num","Den");
    if(numCopy != denName) {
      cout<<"Num and den hists don't match."<<endl;
      cout<<numName<<"\t"<<denName<<endl;
    
      delete numHist; numHist = NULL;
      delete denHist; denHist = NULL;
      continue;
    }
    // Finally, make and save the CF and counts
    TH1D *cf = MakeACF(numHist, denHist, rebinNumber, lowNorm, highNorm);
    TVectorD count = GetNumCounts(numHist, lowNorm, highNorm);  
    WriteObjectsToDir(cfDir, countDir, cf, count);

    cout<<"Finished making cf"<<endl;
  }
}






void MakeCFs(Bool_t isDataCompact, Bool_t isTrainResult)
{
  // Merge between datasets, then merge centralities
  Int_t rebinNumber = 4;
  Double_t lowNorm = 0.5;
  Double_t highNorm = 0.7;
  
  TFile f("CFs.root", "update");

  vector<TString> dataNames;
  if(!isDataCompact) {
    TString dataNamesArr[5] = {"mm1", "mm2", "mm3", "pp1", "pp2"};
    dataNames.assign(dataNamesArr, dataNamesArr+5);
  } else {
    TString dataNamesArr[2] = {"mm", "pp"};
    dataNames.assign(dataNamesArr, dataNamesArr+2);
  }

  TDirectory *dir = &f;
  vector<TDirectory*> dataDirs;
  if(isTrainResult) {
    // Get all the the directories and find their subdirectories
    TList *dirList = f.GetListOfKeys();
    TIter dirIter(dirList);
    TObject *dirObj = NULL;

    // Loop over all variable cut directories
    // and cut bin directories and add them to dataDirs
    while((dirObj = dirIter())) { // var cut directories
      TKey *dirKey = dynamic_cast<TKey*>(dirObj);
      TDirectory *currentDir = dynamic_cast<TDirectory*>(dirKey->ReadObj());
      if(!currentDir) continue;
      TString currentDirName = currentDir->GetName();
      assert(currentDirName.Contains("Var"));
      // This dir exists.  Now grab all cut bin directories inside
      TList *cutList = currentDir->GetListOfKeys();
      TIter cutIter(cutList);
      TObject *cutDirObj = NULL;
      while((cutDirObj = cutIter())) { // cut bin directories
	TKey *cutKey = dynamic_cast<TKey*>(cutDirObj);
	TDirectory *cutDir = dynamic_cast<TDirectory*>(cutKey->ReadObj());
	if(!cutDir) continue;
	TString cutDirName = cutDir->GetName();
	assert(cutDirName.Contains("Cut"));

	//If we got this far, the directory exists. Add it to the vector
	dataDirs.push_back(cutDir);
      } // End cut bin directory while loop
    } // End var cut directory while loop 
  } else {
    // The file itself is the only data directory.
    dataDirs.push_back(&f);
  }
  
  //Now make the CFs for each data and field directory
  for(UInt_t iDir = 0; iDir < dataDirs.size(); iDir++) {
    TDirectory *currentDataDir = dataDirs[iDir];
    cout<<"Making CFs for directory: "
	<<currentDataDir->GetName()<<endl;
    for(UInt_t iName = 0; iName < dataNames.size(); iName++) {
      TDirectory *fieldDir = currentDataDir->GetDirectory(dataNames[iName]);
      assert(fieldDir);
      MakeCFsForDataset(fieldDir, rebinNumber, lowNorm, highNorm);
    }
  }
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
