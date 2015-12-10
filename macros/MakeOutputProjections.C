
void TObjArrProject(TObjArray *dir, TFile &outputFile, TString projectionType /*, Int_t centBinLow, Int_t centBinHigh*/)
{
  TString dirName = dir->GetName();
  
  // CHeck if dir exists.  If not, make it
  
  TDirectory *outDir = NULL;
  outDir = outputFile.GetDirectory(dirName);
  if(!outDir) {
    outDir = outputFile.mkdir(dirName);
  }
  TIter next(dir);
  TObject *obj = NULL;
  while( (obj = next()) ) {
    TH2F *hist2D = dynamic_cast<TH2F*>(obj);
    hist2D->SetDirectory(0);
    if(!hist2D) continue;
    TH1D *h1 = NULL;
    if(projectionType == "X"){
      h1 = hist2D->ProjectionX();
    } else if(projectionType == "Y") {
      h1 = hist2D->ProjectionY();
    } else {
      cout<<"Bad projection direction input:\t"<<projectionType<<endl;
      assert(0);
    }
     
    if(!h1) continue;
    h1->Sumw2();
    h1->SetDirectory(0);
    outDir->cd();
    h1->Write(h1->GetName(), TObject::kOverwrite);

    delete h1; h1 = NULL;
    delete hist2D; hist2D = NULL;

    // TObjArray *arr = dynamic_cast<TList*>(obj);
    // if (arr) arr->Delete();  // alternatively arr->SetOwner(true);
  }
  // femtolist->Delete(); // alternatively  list->SetOwner(true);
  // delete femtolist;
}



void MakeProjectionsOfDirectories(vector<TString> &dirNames, TString projectionType)
{
  TString inFileName = "MyOutputAll.root";
  TFile f(inFileName, "read");
  TFile outFile("Projections.root", "update");
  TList *list = (TList*) f.Get("MyList");
  
  for(UInt_t i = 0; i < dirNames.size(); i++)
  {
    TObjArray *arr = (TObjArray*) list->FindObject(dirNames[i]);
    //Now read in all the histograms in arr, project them, and save them
    // to a directory in outFile
    if(!arr) {
      cout<<"Directory for: "<<dirNames[i]<<" does not exist"<<endl;
    } else {
      TObjArrProject(arr, outFile, projectionType);
    }
  }

}


void MakeMomResProjections()
{
  vector<TString> dirNames;
  dirNames.push_back("ResolutionLL");
  dirNames.push_back("ResolutionLA");
  TString projectionType = "Y";
  MakeProjectionsOfDirectories(dirNames, projectionType);

}

void MakeAvgSepProjections()
{
  vector<TString> dirNames;
  dirNames.push_back("AvgSepNew");
  dirNames.push_back("AvgSepOld");
  TString projectionType = "X";
  MakeProjectionsOfDirectories(dirNames, projectionType);
}




void MakeOutputProjections()
{
  // Main function for projecting momentum resolution
  // and average separation plots
  MakeMomResProjections();
  MakeAvgSepProjections();
}

//******************************************


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




TH1D* ProjectCentralityBin(TH3F* h3D, Int_t centLow, Int_t centHigh, TString pairType)
{

  // Assume only one "variable cut" bin
  Int_t varBinLow = 1;
  Int_t varBinHigh = 1;
  TString name1D = Get1DHistNameSuffix(centLow, centHigh, pairType);
  TH1D *h1D = h3D->ProjectionZ(name1D, varBinLow,varBinHigh, centLow, centHigh);
  // h1D->SetTitle(Get1DHistNameSuffix(centLow, centHigh, pairType));
  
  return h1D;
}

void SaveNumsDens(TDirectory *numDir, TDirectory *denDir, TList *list, TString pairType)
{
  // Save numerator and denominator distributions
  TH3F *num3D = (TH3F*) list->FindObject("fSignal" + pairType);
  TH3F *den3D = (TH3F*) list->FindObject("fBkg" + pairType);
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
  

  // Loop over the centrality bins
  for(Int_t i = 1; i < 21; i++) {
    TH1D *num1D = ProjectCentralityBin(num3D, i, i, pairType);
    if(!num1D) {
      cout<<"Centrality bin "<<i*5<<" does not have the needed projections"<<endl;
      if(num1D) {delete num1D; num1D = NULL;}
      // if(den1D) {delete den1D; den1D = NULL;}
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

    TH1D *den1D = ProjectCentralityBin(den3D, i, i, pairType);
    if(!den1D) {
      cout<<"Centrality bin "<<i*5<<" does not have the needed projections"<<endl;
      if(num1D) {delete num1D; num1D = NULL;}
      if(den1D) {delete den1D; den1D = NULL;}
      continue;
    }
    TString denName = "Den";
    TString currentNameDen =  den1D->GetName();
    // cout<<currentNameDen<<endl;
    denName += currentNameDen;
    // cout<<numName<<"\t"<<denName<<endl;
    den1D->SetTitle(denName);
    den1D->SetName(denName);
    den1D->SetDirectory(0);
    denDir->cd();
    den1D->Write(denName, TObject::kOverwrite);
    // cout<<"Num:\t"<<num1D->GetName()<<"\tDen:\t"<<den1D->GetName()<<endl;
    cout<<"Wrote "<<denName<<" to "<<den1D->GetName()<<endl;


  //   cout<<"About to delete 1D hists"<<endl;
  //   if(num1D) {delete num1D; num1D = NULL;}
  //   if(den1D) {delete den1D; den1D = NULL;}
  }
  
  // if(num3D) {delete num3D; num3D = NULL;}
  // if(den3D) {delete den3D; den3D = NULL;}
}



void MakeCFProjectionForDataSet(TString dataName)
{
  // Make all the numerator and denominator projections for one data set
  // and save them to a root file in their own directory.

  // TString dataName = "mm1";
  TString inFileName = "MyOutput" + dataName + ".root";
  TFile inFile(inFileName,"read");
  if (inFile.IsZombie()) {
    cout << "Error opening file " << inFileName << endl;
    return;
  }

  
  TList *list = (TList*)inFile.Get("MyList");
  
  

  TFile outFile("CFs.root", "update");
  // Setup dataset directory
  TString outDirName = dataName;
  TDirectory *outDir = outFile.GetDirectory(outDirName);
  if(!outDir) {
    outDir = outFile.mkdir(outDirName);
  }
  // Setup subdirectories
  TDirectory *outDirNum = outDir->GetDirectory("Num");
  if(!outDirNum) {
    outDirNum = outDir->mkdir("Num");
  }
  TDirectory *outDirDen = outDir->GetDirectory("Den");
  if(!outDirDen) {
    outDirDen = outDir->mkdir("Den");
  }
  // TDirectory *outDirCF = outDir.GetDirectory("CF");
  // if(!outDirCF) {
  //   outDirCF = outDir.mkdir("CF");
  // }
  TString pairTypes[3] = {"LamLam", "ALamALam", "LamALam"};
  for(Int_t i = 0; i < 3; i++) {
    SaveNumsDens(outDirNum, outDirDen, list, pairTypes[i]);
  }

  // Now do CF stuff? Probably in separate function.  Get and save counts, make cfs
}



void MakeCFProjections(Bool_t isDataReal)
{
  if(isDataReal) {
    TString dataNamesReal[5] = {"mm1", "mm2", "mm3", "pp1", "pp2"};
    for(Int_t i = 0; i < 5; i++) {
      MakeCFProjectionForDataSet(dataNamesReal[i]);
    }
  } else {
    TString dataNamesMC[2] = {"mm", "pp"};
    for(Int_t i = 0; i < 2; i++) {
      MakeCFProjectionForDataSet(dataNamesMC[i]);
    }
  }
}





