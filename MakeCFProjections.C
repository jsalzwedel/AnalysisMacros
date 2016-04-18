// Take the contents of an output file (train or local result)
// and project out the same- and mixed-pair distributions from
// 3D histograms

TString ConvertCentBinToString(Int_t bin, Bool_t upper)
{
  TString s = "";
  if(upper) {
    Int_t x = bin * 5;
    s += x;
  } else {
    Int_t x = (bin - 1) * 5;
    s += x;
  }

  return s;
}



TString Get1DHistNameSuffix(Int_t lowBin, Int_t highBin, TString pairType)
{

  TString lowCent = ConvertCentBinToString(lowBin, kFALSE);
  TString highCent = ConvertCentBinToString(lowBin, kTRUE);

  TString suffix = pairType + lowCent + highCent;

  return suffix;
}




TH1D* ProjectCentralityBin(TH3F* h3D, Int_t centLow, Int_t centHigh, TString pairType, Int_t varBinLow = 1, Int_t varBinHigh = 1)
{

  // Assume only one "variable cut" bin
  TString name1D = Get1DHistNameSuffix(centLow, centHigh, pairType);
  TH1D *h1D = h3D->ProjectionZ(name1D, varBinLow,varBinHigh, centLow, centHigh);
  // h1D->SetTitle(Get1DHistNameSuffix(centLow, centHigh, pairType));
  
  return h1D;
}

void SaveNumsDens(TDirectory *dataDir, TString fieldName, TH3F *num3D, TH3F* den3D, TString pairType)
{
  // Save numerator and denominator distributions


  //Figure out how many histogram bins there are for different cut values
  Int_t nCutBins = num3D->GetNbinsX();
  assert(nCutBins == den3D->GetNbinsX());

  // Loop over the cut bins
  for(Int_t iCut = 1; iCut < nCutBins + 1; iCut++) {

    // Setup/open directories
    TString cutDirName = "Cut";
    cutDirName += iCut;
    TDirectory *cutDir = dataDir->GetDirectory(cutDirName);
    if(!cutDir) {
      cutDir = dataDir->mkdir(cutDirName);
    }
    TDirectory *fieldDir = cutDir->GetDirectory(fieldName);
    if(!fieldDir) {
      fieldDir = cutDir->mkdir(fieldName);
    }
    TDirectory *numDir = fieldDir->GetDirectory("Num");
    if(!numDir) {
      numDir = fieldDir->mkdir("Num");
    }
    TDirectory *denDir = fieldDir->GetDirectory("Den");
    if(!denDir) {
      denDir = fieldDir->mkdir("Den");
    }

    // Loop over the centrality bins
    for(Int_t iCent = 1; iCent < 21; iCent++) {
      TH1D *num1D = ProjectCentralityBin(num3D, iCent, iCent, pairType, iCut, iCut);
      if(!num1D) {
	cout<<"Centrality bin "<<iCent*5<<" does not have the needed projections"<<endl;
	if(num1D) {delete num1D; num1D = NULL;}
	continue;
      }
    
      TString numName = "Num";
      TString currentNameNum = num1D->GetName();
      numName += currentNameNum;
      num1D->SetTitle(numName);
      num1D->SetName(numName);
      num1D->SetDirectory(0);
      numDir->cd();
      num1D->Write(numName, TObject::kOverwrite);

      cout<<"Wrote "<<numName<<" to "<<num1D->GetName()<<endl;

      TH1D *den1D = ProjectCentralityBin(den3D, iCent, iCent, pairType, iCut, iCut);
      if(!den1D) {
	cout<<"Centrality bin "<<iCent*5<<" does not have the needed projections"<<endl;
	if(num1D) {delete num1D; num1D = NULL;}
	if(den1D) {delete den1D; den1D = NULL;}
	continue;
      }
      TString denName = "Den";
      TString currentNameDen =  den1D->GetName();
      denName += currentNameDen;
      den1D->SetTitle(denName);
      den1D->SetName(denName);
      den1D->SetDirectory(0);
      denDir->cd();
      den1D->Write(denName, TObject::kOverwrite);
      cout<<"Wrote "<<denName<<" to "<<den1D->GetName()<<endl;

      if(num1D) {delete num1D; num1D = NULL;}
      if(den1D) {delete den1D; den1D = NULL;}
    }
  }
}

void RunOverTList(TList *list, TString dataName, TString fieldName)
{

  TFile outFile("CFs.root", "update");
  
  // Setup dataset directory
  TDirectory *dataDir = NULL;
  if(!dataName.IsNull()) { 
    dataDir = outFile.GetDirectory(dataName);
    if(!dataDir) {
      dataDir = outFile.mkdir(dataName);
    }
  } else {
    dataDir = &outFile;
  }


  // Now project nums and dens for each pair type
  TString pairTypes[3] = {"LamLam", "ALamALam", "LamALam"};
  for(Int_t i = 0; i < 3; i++) {
    TH3F *num3D = (TH3F*) list->FindObject("fSignal" + pairTypes[i]);
    TH3F *den3D = (TH3F*) list->FindObject("fBkg" + pairTypes[i]);
    if(!num3D) {
      cout<<"Could not find 3D Num!"<<endl;
      if(num3D) {delete num3D; num3D = NULL;}
      if(den3D) {delete den3D; den3D = NULL;}
      return;
    }
    if(!den3D) {
      cout<<"Could not find 3D Den!"<<endl;
      if(num3D) {delete num3D; num3D = NULL;}
      if(den3D) {delete den3D; den3D = NULL;}
      return;
    }
    SaveNumsDens(dataDir, fieldName, num3D, den3D, pairTypes[i]);
  }
}


void MakeCFProjectionForDataSet(TString fieldName)
{
  // Make all the numerator and denominator projections for one data set
  // and save them to a root file in their own directory.


  // Generate the file name and open the file
  TString inFileName;
  inFileName = "AnalysisResults" + fieldName + ".root";
  TFile inFile(inFileName,"read");
  if (inFile.IsZombie()) {
    cout << "Error opening file " << inFileName << endl;
    return;
  }

  // Get the output TList (or TLists in the case of train results)
  vector<TList*> inputLists;
  TDirectory *dir = inFile.GetDirectory("Results");

  // Iterate over contents of directory and get all the TLists
  TIter nextkey(dir->GetListOfKeys());
  TKey *key;
  while ((key = (TKey*)nextkey())){
    TList *list = dynamic_cast<TList*>(key->ReadObj());
    if(!list) continue;
    inputLists.push_back(list);
  }
  // }

  for(UInt_t iList = 0; iList < inputLists.size(); iList++) {
    TString outputDataName;
    TString listName = inputLists[iList]->GetName();
    if(listName.Contains("MyList")) {
      // Remove the "MyList" characters
      TString subName = listName.Remove(0,6);
      outputDataName += subName;
    }
    cout<<"Projecting out "<<outputDataName<<" for "<<fieldName<<endl;
    RunOverTList(inputLists[iList], outputDataName, fieldName);

    // Clean up
    if(inputLists[iList]) {
      delete inputLists[iList];
      inputLists[iList] = NULL;
    }
  }
  // Now do CF stuff? Probably in separate function.  Get and save counts, make cfs
}



void MakeCFProjections(Bool_t isDataCompact, Bool_t isTrainResult)
{
  if (!isTrainResult) {
    TString fileSuffix = "Local";
    MakeCFProjectionForDataSet(fileSuffix);
  } else if (!isDataCompact) {
    TString fieldNamesReal[5] = {"mm1", "mm2", "mm3", "pp1", "pp2"};
    for (Int_t i = 0; i < 5; i++) {
      MakeCFProjectionForDataSet(fieldNamesReal[i]);
    }
  } else {
    TString fieldNamesMC[2] = {"mm", "pp"};
    for (Int_t i = 0; i < 2; i++) {
      MakeCFProjectionForDataSet(fieldNamesMC[i]);
    }
  }
}
