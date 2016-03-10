


void TObjArrProject(TObjArray *dir, TFile &outputFile, TString projectionType2D, TString projectionType3D, Double_t lowProjAxis1 = -1., Double_t highProjAxis1 = -1., Double_t lowProjAxis2 = -1., Double_t highProjAxis2 = -1./*, Int_t centBinLow, Int_t centBinHigh*/)
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
  cout<<"Axis ranges:\t"<<lowProjAxis1<<"\t"
      <<highProjAxis1<<"\t"
      <<lowProjAxis2<<"\t"
      <<highProjAxis2<<endl;
  while( (obj = next()) ) {
    // TH
    TH1D *h1 = NULL;    
    TH2F *hist2D = dynamic_cast<TH2F*>(obj);
    TH3F *hist3D = dynamic_cast<TH3F*>(obj);

    Int_t firstBinAxis1 = 0;
    Int_t lastBinAxis1 = 0;
    Int_t firstBinAxis2 = 0;
    Int_t lastBinAxis2 = 0;
    if(hist2D) {
      hist2D->SetDirectory(0);
      TString projName = hist2D->GetName();
      if(projectionType2D == "X"){
	if(lowProjAxis1 > -0.5 && highProjAxis1 > -0.5) {
	  firstBinAxis1 = hist2D->GetYaxis()->FindBin(lowProjAxis1);
	  lastBinAxis1 = hist2D->GetYaxis()->FindBin(highProjAxis1);
	}
	projName += "_px";
	h1 = hist2D->ProjectionX(projName, firstBinAxis1, lastBinAxis2);
      } else if(projectionType2D == "Y") {
	if(lowProjAxis1 > -0.5 && highProjAxis1 > -0.5) {
	  firstBinAxis1 = hist2D->GetXaxis()->FindBin(lowProjAxis1);
	  lastBinAxis2 = hist2D->GetXaxis()->FindBin(highProjAxis1);
	}
	projName += "_py";
	h1 = hist2D->ProjectionY(projName, firstBinAxis1, lastBinAxis2);
      } else {
	cout<<"Bad projection direction input:\t"<<projectionType2D<<endl;
	assert(0);
      }
    } else if(hist3D) {
      hist3D->SetDirectory(0);
      TString projName = hist3D->GetName();
      if(projectionType3D == "X"){
	if(lowProjAxis1 > -0.5 && highProjAxis1 > -0.5) {
	  firstBinAxis1 = hist3D->GetYaxis()->FindBin(lowProjAxis1);
	  lastBinAxis1 = hist3D->GetYaxis()->FindBin(highProjAxis1);
	}
	if(lowProjAxis2 > -0.5 && highProjAxis2 > -0.5) {
	  firstBinAxis2 = hist3D->GetZaxis()->FindBin(lowProjAxis2);
	  lastBinAxis2 = hist3D->GetZaxis()->FindBin(highProjAxis2);
	}
	cout<<"Using bins:\t"<<firstBinAxis1<<"\t"
	    <<lastBinAxis1<<"\t"
	    <<firstBinAxis2<<"\t"
	    <<lastBinAxis2<<endl;
	projName += "_px";
	h1 = hist3D->ProjectionX(projName, firstBinAxis1, lastBinAxis2, firstBinAxis1, lastBinAxis2);
      } else if(projectionType3D == "Y") {
	if(lowProjAxis1 > -0.5 && highProjAxis1 > -0.5) {
	  firstBinAxis1 = hist3D->GetXaxis()->FindBin(lowProjAxis1);
	  lastBinAxis1 = hist3D->GetXaxis()->FindBin(highProjAxis1);
	}
	if(lowProjAxis2 > -0.5 && highProjAxis2 > -0.5) {
	  firstBinAxis2 = hist3D->GetZaxis()->FindBin(lowProjAxis2);
	  lastBinAxis2 = hist3D->GetZaxis()->FindBin(highProjAxis2);
	}
	projName += "_py";
	h1 = hist3D->ProjectionY(projName, firstBinAxis1, lastBinAxis2, firstBinAxis1, lastBinAxis2);
      } else {
	cout<<"Bad projection direction input:\t"<<projectionType3D
	    <<endl;
	assert(0);
      }
    }
    if(!h1) {
      cout<<"Could not find histogram. Skipping."<<endl;
      continue;
    }
    h1->Sumw2();
    h1->SetDirectory(0);
    outDir->cd();
    h1->Write(h1->GetName(), TObject::kOverwrite);

    if(h1) delete h1; h1 = NULL;
    if(hist2D) delete hist2D; hist2D = NULL;
    if(hist3D) delete hist3D; hist3D = NULL;

    // TObjArray *arr = dynamic_cast<TList*>(obj);
    // if (arr) arr->Delete();  // alternatively arr->SetOwner(true);
  }
  // femtolist->Delete(); // alternatively  list->SetOwner(true);
  // delete femtolist;
}



void MakeProjectionsOfDirectories(vector<TString> &dirNames, TString projectionType2D, TString projectionType3D, Double_t lowProjAxis1 = -1., Double_t highProjAxis1 = -1., Double_t lowProjAxis2 = -1., Double_t highProjAxis2 = -1.)
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
      TObjArrProject(arr, outFile, projectionType2D, projectionType3D, lowProjAxis1, highProjAxis1, lowProjAxis2, highProjAxis2);
    }
  }

}


void MakeMomResProjections()
{
  vector<TString> dirNames;
  dirNames.push_back("ResolutionLL");
  dirNames.push_back("ResolutionAA");
  dirNames.push_back("ResolutionLA");
  TString projectionType2D = "Y";
  TString projectionType3D = "X";
  MakeProjectionsOfDirectories(dirNames, projectionType2D, projectionType3D);

}

void MakeAvgSepProjections()
{
  vector<TString> dirNames;
  dirNames.push_back("AvgSepNew");
  dirNames.push_back("AvgSepOld");
  TString projectionType2D = "X";
  TString projectionType3D = "X";
  Double_t lowProjAxisY = -1.; // use negative 1 for no limit
  Double_t highProjAxisY = -1.;
  Double_t lowProjAxisZ = -1.;
  Double_t highProjAxisZ = -1.;
  MakeProjectionsOfDirectories(dirNames, projectionType2D, projectionType3D, lowProjAxisY, highProjAxisY, lowProjAxisZ, highProjAxisZ);
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


void MakeCFProjectionForDataSet(TString fieldName, Bool_t isTrainResult)
{
  // Make all the numerator and denominator projections for one data set
  // and save them to a root file in their own directory.


  // Generate the file name and open the file
  TString inFileName;
  if(isTrainResult) {
    inFileName = "AnalysisResults" + fieldName + ".root";
  } else {
    inFileName = "MyOutput" + fieldName + ".root";
  }
  TFile inFile(inFileName,"read");
  if (inFile.IsZombie()) {
    cout << "Error opening file " << inFileName << endl;
    return;
  }

  // Get the output TList (or TLists in the case of train results)
  vector<TList*> inputLists;
  // TList *list;
  if(!isTrainResult) {
    // list = (TList*)inFile.Get("MyList");
    inputLists.push_back((TList*)inFile.Get("MyList"));
  } else {
    TDirectory *dir = inFile.GetDirectory("Results");

    // Iterate over contents of directory and get all the TLists
    TIter nextkey(dir->GetListOfKeys());
    TKey *key;
    while ((key = (TKey*)nextkey())){
      TList *list = dynamic_cast<TList*>(key->ReadObj());
      if(!list) continue;
      inputLists.push_back(list);
    }
  }

  for(UInt_t iList = 0; iList < inputLists.size(); iList++) {

    TString outputDataName;
    if(isTrainResult) {
      TString listName = inputLists[iList]->GetName();
      if(listName.Contains("MyList")) {
	// Remove the "MyList" characters
	TString subName = listName.Remove(0,6);
	outputDataName += subName;
      }
    }
    cout<<"Projecting out "<<outputDataName<<" for "<<fieldName<<endl;
    RunOverTList(inputLists[iList], outputDataName, fieldName);
  }
  // Now do CF stuff? Probably in separate function.  Get and save counts, make cfs
}



void MakeCFProjections(Bool_t isDataCompact, Bool_t isTrainResult)
{
  if(!isDataCompact) {
    TString fieldNamesReal[5] = {"mm1", "mm2", "mm3", "pp1", "pp2"};
    for(Int_t i = 0; i < 5; i++) {
      MakeCFProjectionForDataSet(fieldNamesReal[i], isTrainResult);
    }
  } else {
    TString fieldNamesMC[2] = {"mm", "pp"};
    for(Int_t i = 0; i < 2; i++) {
      MakeCFProjectionForDataSet(fieldNamesMC[i], isTrainResult);
    }
  }
}





