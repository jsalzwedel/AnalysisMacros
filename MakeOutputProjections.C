


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
