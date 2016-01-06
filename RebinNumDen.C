

void RebinDir(TDirectory *inDir, TDirectory *outDir, Int_t rebinNumber)
{
  // Read in everything from one directory, rebin it, then write
  // it to another directory

  TIter iter(inDir->GetListOfKeys());
  TObject *obj = NULL;
  
  while ((obj = iter())) {
    TKey *key = dynamic_cast<TKey*>(obj);
    //Get the num and den hists
    TH1D *hist = dynamic_cast<TH1D*>(key->ReadObj());
    if(!hist) {
      cout<<"Could not find appropriate hist"<<endl;
      return;
    }

    hist->Rebin(rebinNumber);
    outDir->cd();
    hist->Write(hist->GetName(), TObject::kOverwrite);
    delete hist; hist = NULL;
  }

}


void RunRebinForDataset(TDirectory *dataDir, Int_t rebinNumber) {

  TDirectory *numDir = (TDirectory*) dataDir->GetDirectory("Num");
  if(!numDir) {
    cout<<"No Num directory in "<<dataDir->GetName()<<endl;
    return;
  }
  TDirectory *numRebinDir = dataDir->GetDirectory("NumRebin");
  if(!numRebinDir) {
    numRebinDir = dataDir->mkdir("NumRebin");
  }
  RebinDir(numDir, numRebinDir, rebinNumber);

  TDirectory *denDir = dataDir->GetDirectory("Den");
  if(!denDir) {
    cout<<"No Den directory "<<dataDir->GetName()<<endl;
    return;
  }
  TDirectory *denRebinDir = dataDir->GetDirectory("DenRebin");
  if(!denRebinDir) {
    denRebinDir = dataDir->mkdir("DenRebin");
  }
  RebinDir(denDir, denRebinDir, rebinNumber);
}


void RebinNumDen()
{
  vector<TString> dataNames = {"mm1", "mm2", "mm3", "pp1", "pp2"};
  Int_t rebinNumber = 4;

  TFile inFile("CFs.root", "Update");

  for(UInt_t i = 0; i < dataNames.size(); i++) {
    TDirectory *dataDir = (TDirectory*) inFile.GetDirectory(dataNames[i]);
    if(!dataDir) {
      cout<<"Could not find data directory named "
	  <<dataNames[i]<<endl;
      continue;
    }
    RunRebinForDataset(dataDir, rebinNumber);
  }
}
