# include "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/macros/GetDate.C"


TGraphAsymmErrors* CombineTwoTGraphAsymm(TGraphAsymmErrors *graph1, TGraphAsymmErrors *graph2, TH1D *baseHist, Bool_t shouldAddQuadrature)
{

  if (!graph1 || !graph2 || !baseHist) {
    cout << "Missing component. Cannot combine graphs" <<endl;
    return NULL;
  }
  
  // Initialize the TGraph from the base histogram
  TGraphAsymmErrors *combinedGraph = new TGraphAsymmErrors(baseHist);

  // Loop over the bins and combine the errors
  for (Int_t iBin = 0; iBin < combinedGraph->GetN(); iBin++) {
    Double_t errYHigh = 0;
    Double_t errYLow = 0;
    if (shouldAddQuadrature) {
      // Add the errors in quadrature
      errYHigh = pow(graph1->GetErrorYhigh(iBin), 2);
      errYHigh += pow(graph2->GetErrorYhigh(iBin), 2);
      errYHigh = sqrt(errYHigh);

      errYLow = pow(graph1->GetErrorYlow(iBin), 2);
      errYLow += pow(graph2->GetErrorYlow(iBin), 2);
      errYLow = sqrt(errYLow);
    } else {
      // Just take the largest value for the bin
      if (errYHigh < graph1->GetErrorYhigh(iBin)) {
	errYHigh = graph1->GetErrorYhigh(iBin);
      }
      if (errYHigh < graph2->GetErrorYhigh(iBin)) {
	errYHigh = graph2->GetErrorYhigh(iBin);
      }
      if (errYLow < graph1->GetErrorYlow(iBin)) {
	errYLow = graph1->GetErrorYlow(iBin);
      }
      if (errYLow < graph2->GetErrorYlow(iBin)) {
	errYLow = graph2->GetErrorYlow(iBin);
      }
    }

    combinedGraph->SetPointEYhigh(iBin, errYHigh);
    combinedGraph->SetPointEYlow(iBin, errYLow);
  }
  return combinedGraph;
}

void WriteResults(TGraphAsymmErrors *errors, TH1D *hist, TDirectory *outDir, Bool_t shouldAddQuadrature)
{
  outDir->cd();
  TString histName = hist->GetName();
  TString graphName = histName + "CombinedErrors";

  TString errorCombinationType;
  if (shouldAddQuadrature) {
    errorCombinationType += "Quadrature";
  } else {
    errorCombinationType += "Maximum";
  }
  graphName += errorCombinationType;

  // Pretty up the plot
  hist->SetNdivisions(505,"xy");
  hist->SetAxisRange(0.8, 1.1, "y");
  hist->SetAxisRange(0., 0.5, "X");
  hist->GetXaxis()->SetTitleOffset(0.8);
  hist->SetMarkerStyle(kFullCircle);
  hist->GetYaxis()->SetTitle("C(k*)");
  hist->SetTitleSize(0.06, "y");
  hist->GetYaxis()->SetTitleOffset(0.7);
  hist->GetYaxis()->CenterTitle();
  errors->SetLineColor(kRed);
  errors->SetFillStyle(0);
  gStyle->SetOptStat(0);

  // Give the plot a nicer title
  TString system;
  TString centrality;
  TString oldTitle = hist->GetTitle();
  if (oldTitle.Contains("LamALam")) system = "#Lambda#bar{#Lambda}";
  else if (oldTitle.Contains("LLAA")) system = "#Lambda#Lambda";
  else {
    cout << "Could not parse histogram title for system name"
	 << endl;
    system = "BadSystemName";
  }

  if (oldTitle.Contains("010")) centrality = "0-10%";
  else if (oldTitle.Contains("1030")) centrality = "10-30%";
  else if (oldTitle.Contains("3050")) centrality = "30-50%";
  else {
    cout << "Could not parse histogram title for centrality"
	 << endl;
    centrality = "BadCentrality";
  }

  TString newTitle = system + " " + centrality;
  hist->SetTitle(newTitle);
   
  errors->Write(graphName, TObject::kOverwrite);
  hist->Write(histName, TObject::kOverwrite);

  // Save plots in all the necessary formats
  TString outputBashDir = "Plots/CombinedSys";
  outputBashDir += errorCombinationType;
  if (!gSystem->OpenDirectory(outputBashDir)) {
    gSystem->mkdir(outputBashDir, kTRUE);
  }



  TCanvas c1(histName + "Can", histName);
  hist->Draw();
  errors->Draw("E2");

  TString date = GetDate(kTRUE);
  
  TString outputName = outputBashDir + "/" + date + histName + "CombinedSystematics";
  outputName += errorCombinationType;
  c1.SaveAs(outputName + ".png");
  c1.SaveAs(outputName + ".eps");
  c1.SaveAs(outputName + ".pdf");
}

void CombineTGraphsFromTwoDirectories(TDirectory *dir1, TDirectory *dir2, Int_t iBaseDir, TDirectory *outputDir, Bool_t shouldAddQuadrature)
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

    TGraphAsymmErrors* combinedGraph = CombineTwoTGraphAsymm(graphsDir1[iGraph], graphsDir2[iGraph], baseHists[iGraph], shouldAddQuadrature);
    WriteResults(combinedGraph, baseHists[iGraph], outputDir, shouldAddQuadrature);
  }
}


void CombineTGraphErrors(Bool_t shouldAddQuadrature)
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
  
  CombineTGraphsFromTwoDirectories(dir1, dir2, indexBaseDir, outputDir, shouldAddQuadrature);
}


