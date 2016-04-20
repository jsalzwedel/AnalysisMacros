


TGraphAsymmErrors* CombineTwoTGraphAsymm(TGraphAsymmErrors *graph1, TGraphAsymmErrors *graph2, TH1D *baseHist)
{

  if (!graph1 || !graph2 || !baseHist) {
    cout << "Missing component. Cannot combine graphs" <<endl;
    return NULL;
  }
  
  // Initialize the TGraph from the base histogram
  TGraphAsymmErrors *combinedGraph = new TGraphAsymmErrors(baseHist);

  // Loop over the bins and combine the errors
  for (Int_t iBin = 0; iBin < combinedGraph->GetN(); iBin++) {
    Double_t errYHigh = pow(graph1->GetErrorYhigh(iBin), 2);
    errYHigh += pow(graph2->GetErrorYhigh(iBin), 2);
    errYHigh = sqrt(errYHigh);

    Double_t errYLow = pow(graph1->GetErrorYlow(iBin), 2);
    errYLow += pow(graph2->GetErrorYlow(iBin), 2);
    errYLow = sqrt(errYLow);

    combinedGraph->SetPointEYhigh(iBin, errYHigh);
    combinedGraph->SetPointEYlow(iBin, errYLow);
  }
  return combinedGraph;
}

void WriteResults(TGraphAsymmErrors *errors, TH1D *hist, TDirectory *outDir)
{
  outDir->cd();
  TString histName = hist->GetName();
  TString graphName = histName + "CombinedErrors";
  errors->Write(graphName, TObject::kOverwrite);
  hist->Write(histName, TObject::kOverwrite);

  // Save plots in all the necessary formats
  TString outputBashDir = "Plots/CombinedSys";
  if (!gSystem->OpenDirectory(outputBashDir)) {
    gSystem->mkdir(outputBashDir, kTRUE);
  }

  // Pretty up the plot
  hist->SetNdivisions(505,"xy");
  hist->SetAxisRange(0.8, 1.1, "y");
  hist->SetAxisRange(0., 0.5, "X");
  hist->SetMarkerStyle(kFullCircle);
  errors->SetLineColor(kRed);
  errors->SetFillStyle(0);
  gStyle->SetOptStat(0);

  TCanvas c1(histName + "Can", histName);
  hist->Draw();
  errors->Draw("E2");

  TString outputName = outputBashDir + "/" + histName + "CombinedSystematics";
  c1.SaveAs(outputName + ".png");
  c1.SaveAs(outputName + ".eps");
  c1.SaveAs(outputName + ".pdf");
}

void CombineTGraphsFromTwoDirectories(TDirectory *dir1, TDirectory *dir2, Int_t iBaseDir, TDirectory *outputDir)
{

  // Gather TGraphErrors from 1st directory
  vector<TGraphAsymmErrors*> graphsDir1;
  TIter nextKey(dir1->GetListOfKeys());
  TObject *obj;
  while ((obj = nextKey())) {
    TKey *thisKey = dynamic_cast<TKey*>(obj);
    TGraphAsymmErrors *graph = dynamic_cast<TGraphAsymmErrors*>(thisKey->ReadObj());
    if(!graph) continue;
    graphsDir1.push_back(graph);
    cout << "This graph name is " << graph->GetName() << endl;
  }

  // Gather equivalent TGraphErrors from 2nd directory
  vector<TGraphAsymmErrors*> graphsDir2;
  for (UInt_t iGraph = 0; iGraph < graphsDir1.size(); iGraph++) {
    TString graphName = graphsDir1[iGraph]->GetName();
    TGraphAsymmErrors *equivGraph = (TGraphAsymmErrors*)dir2->Get(graphName);
    if (!equivGraph) {
      cout << "Could not find equivalent graph in 2nd directory for "
	   << graphName << endl;
      graphsDir2.push_back(NULL); // Put in Null as a place holder
    } else {
      graphsDir2.push_back(equivGraph);
    }
  }

  // Figure out which directory to get the base histogram from
  TDirectory *baseDir;
  if (iBaseDir == 1) {
    baseDir = dir1;
  } else if (iBaseDir == 2) {
    baseDir = dir2;
  } else {
    cout << "Uh oh! Not a valid directory from which to get base histograms. Index: "
	 << iBaseDir << endl;
  }

  // Gather the base histograms
  vector<TH1D*> baseHists;
  for (UInt_t iGraph = 0; iGraph < graphsDir1.size(); iGraph++) {
    TString histName = graphsDir1[iGraph]->GetName();
    histName.ReplaceAll("AsymmErrors","");
    TH1D *hist = (TH1D*) baseDir->Get(histName);
    if(!hist) {
      cout << "Could not find base histogram with name "
	   << histName << endl;
      baseHists.push_back(NULL); // Put in NULL as placeholder
    } else {
      baseHists.push_back(hist);
    }
  }

  // Now combine each graph, save it with its corresponding histogram, and draw it
  for (UInt_t iGraph = 0; iGraph < graphsDir1.size(); iGraph++) {
    // Make sure the components exist
    if (!graphsDir1[iGraph] || !graphsDir2[iGraph] || !baseHists[iGraph]) {
      cout << "Missing component. Cannot combine graphs for index "
	   << iGraph << endl;
      continue;
    }

    TGraphAsymmErrors* combinedGraph = CombineTwoTGraphAsymm(graphsDir1[iGraph], graphsDir2[iGraph], baseHists[iGraph]);
    WriteResults(combinedGraph, baseHists[iGraph], outputDir);
  }
}


void CombineTGraphErrors()
{
  TFile file("SysErrors.root", "Update");
  TDirectory *dir1 = file.GetDirectory("TopologicalSystematics");
  TDirectory *dir2 = file.GetDirectory("AvgSepSystematics");

  Int_t indexBaseDir = 2;

  TString combinedDirName = "CombinedSystematics";
  TDirectory *outputDir = file.GetDirectory(combinedDirName);
  if(!outputDir) {
    outputDir = file.mkdir(combinedDirName);
  }
  
  CombineTGraphsFromTwoDirectories(dir1, dir2, indexBaseDir, outputDir);
}


