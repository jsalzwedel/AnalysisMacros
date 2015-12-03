void MakeOutputProjections()
{
  MakeMomResProjections();
  MakeAvgSepProjections();
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

void MakeProjectionsOfDirectories(vector<TString> &dirNames, TString projectionType)
{
  TString inFileName = "MyOutput.root";
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
  while( obj = next() ) {
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
