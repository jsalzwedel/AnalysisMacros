#include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/GetDataDirectories.C"

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


void RunRebinForField(TDirectory *dataDir, Int_t rebinNumber) {

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


void RebinNumDenInDirectory(TDirectory *dataDir, Bool_t isDataCompact)
{
  Int_t rebinNumber = 4;

  vector<TString> fieldNames;
  if(!isDataCompact) {
    TString fieldNamesArr[5] = {"mm1", "mm2", "mm3", "pp1", "pp2"};
    fieldNames.assign(fieldNamesArr, fieldNamesArr+5);
  } else {
    TString fieldNamesArr[2] = {"mm", "pp"};
    fieldNames.assign(fieldNamesArr, fieldNamesArr+2);
  }

  for(UInt_t i = 0; i < fieldNames.size(); i++) {
    TDirectory *fieldDir = dataDir->GetDirectory(fieldNames[i]);
    if(!fieldDir) {
      cout<<"Could not find data directory named "
	  <<fieldNames[i]<<endl;
      continue;
    }
    RunRebinForField(fieldDir, rebinNumber);
  }
}

void RebinNumDen(Bool_t isDataCompact, Bool_t isTrainResult)
{
  TFile cfFile("CFs.root", "update");
  vector<TDirectory*> dataDirs = GetDataDirectories(cfFile, isTrainResult);

  for(UInt_t iDir = 0; iDir < dataDirs.size(); iDir++) {
    RebinNumDenInDirectory(dataDirs[iDir], isDataCompact);
  }

}
