// Read in CFs.root
// Find all the fits with a particular name (e.g. LamALam010...)
// Add them together (in quadrature?)
// Save the new fit/histogram as the systematic uncertainty on that hist.
// Repeat for other names

// Maybe loop through the TDirectories and grab all the TF1 objects,
// add them to a vector, and then sort them out later?

#include "DefineEnums.C"

void AddTF1sFromSubdirToVector(vector<TF1*> &vec, TDirectory *currentDir)
{
  // currentDir->ls();

  // Loop over all entries of this directory
  TKey *key;
  TIter nextKey(currentDir->GetListOfKeys());
  while ((key = (TKey*)nextKey())) {
    TString className = key->GetClassName();
    TClass *cl = gROOT->GetClass(className);
    if (!cl) continue;
    if (cl->InheritsFrom(TDirectory::Class())) {
      // If object is a TDirectory, repeat this process inside it
      // before continuing through this loop.
      TDirectory *subdir = currentDir->GetDirectory(key->GetName());
      AddTF1sFromSubdirToVector(vec, subdir);
    } else if (cl->InheritsFrom(TF1::Class())) {
      // Add all TF1s to the vector
      TString fitName = key->GetName();
      if (!fitName.Contains("Fail")) continue;
      TF1 *fitResult = (TF1*) currentDir->Get(fitName);
      vec.push_back(fitResult);
    }
  }
}

vector<TF1*> GetAllTF1sFromFile(TString fileName)
{
  TFile inputFile(fileName);

  vector<TF1*> vecTF1;
  AddTF1sFromSubdirToVector(vecTF1, &inputFile);

  cout << "Found " << vecTF1.size() << " TF1s in "
       << fileName << endl;

  for (UInt_t iFit = 0; iFit < vecTF1.size(); iFit++) {
    cout<<vecTF1[iFit]->GetName()<<endl;
  }
  return vecTF1;
}





vector< vector<TF1*> > SortTF1s(vector<TF1*> &vecAll)
{
  // Make a different vector for each system, and
  // add the respective TF1s to those vectors
  cout << "Sorting error TF1s" << endl;
  vector<TString> names = {"CFLamALam010", "CFLamALam1030", "CFLamALam3050",
			   "CFLLAA010", "CFLLAA1030", "CFLLAA3050"};

  vector< vector<TF1*> > sortedVec(names.size());
  for (UInt_t iFit = 0; iFit < vecAll.size(); iFit++) {
    TString thisName = vecAll[iFit]->GetName();
    for (UInt_t iName = 0; iName < names.size(); iName++) {
      if (thisName.Contains(names[iName])) {
	sortedVec[iName].push_back(vecAll[iFit]);
	break;
      }
    }
  }

  for (UInt_t iName = 0; iName < names.size(); iName++) {
    cout<<"For "<<names[iName]<<", found "<<sortedVec[iName].size()<<endl;
  }
  return sortedVec;
}

vector<Double_t> ConvertTF1ToVector(TF1* fit) {
  // Take a fit, convert it into a TH1,
  // then use that to make a vector of doubles

  // Get the histogram equivalent of the fit.
  TH1 *hist = fit->GetHistogram();
  Int_t nBins = hist->GetNbinsX();
  vector<Double_t> valVec;
  for (Int_t iBin = 1; iBin < nBins + 1; iBin++) {
    valVec.push_back(hist->GetBinContent(iBin));
  }
  return valVec;
}

void CombinePosNegSeparately(vector <vector <Double_t> > errorVectors,
			     vector<Double_t> &posVals,
			     vector<Double_t> &negVals)
{
  // Get all the error vectors.  Combine all the postive
  // values in quadrature, and combine all the negative values
  // in quadrature
  // cout << "Combining error vectors" << endl;
  
  if (errorVectors.size() < 1) {
    return;
  }
  
  posVals.resize(errorVectors[0].size(), 0.);
  negVals.resize(errorVectors[0].size(), 0.);

  // Grab an error vector
  for (UInt_t iErrVec = 0; iErrVec < errorVectors.size(); iErrVec++) {
    vector<Double_t> &thisVec = errorVectors[iErrVec];

    // Loop over each bin in the vector
    for (UInt_t iBin = 0; iBin < thisVec.size(); iBin++) {
      Double_t errVal = thisVec[iBin];

      // Decide whether to add to posVec or negVec,
      // then add in quadrature
      if(errVal > 0) {
	posVals[iBin] += pow(errVal,2.);
      } else {
	negVals[iBin] += pow(errVal,2.);
      }
    }
  }

  // Sqrt the results for each bin
  // TGraphAsymmErrors wants negVals to be positive
  for (UInt_t iBin = 0; iBin < posVals.size(); iBin++) {
    posVals[iBin] = sqrt(posVals[iBin]);
    negVals[iBin] = sqrt(negVals[iBin]);
    // negVals[iBin] *= -1.;
  } 
}

void FindMaximumPosNegValues(vector <vector <Double_t> > errorVectors,
			     vector<Double_t> &posVals,
			     vector<Double_t> &negVals)
{
  // Take all the error vectors. For each bin,
  // find and save the maximum values (pos and neg).
  
  if (errorVectors.size() < 1) {
    return;
  }
  
  posVals.resize(errorVectors[0].size(), 0.);
  negVals.resize(errorVectors[0].size(), 0.);
  // Grab an error vector
  for (UInt_t iErrVec = 0; iErrVec < errorVectors.size(); iErrVec++) {
    vector<Double_t> &thisVec = errorVectors[iErrVec];
    // Loop over each bin in the vector
    for (UInt_t iBin = 0; iBin < thisVec.size(); iBin++) {
      Double_t errVal = thisVec[iBin];

      // Decide whether to add to posVec or negVec,
      // then add in quadrature
      if (errVal > 0) {
	if (posVals[iBin] < fabs(errVal)) {
	  posVals[iBin] = fabs(errVal);
	}
      } else {
	if (negVals[iBin] < fabs(errVal)) {
	  negVals[iBin] = fabs(errVal);
	}
      }
    }
  }
}

TGraphAsymmErrors *ConstructAsymmTGraph(const TH1D* baseHist, const vector<Double_t> posVals, const vector<Double_t> negVals)
{
  cout << "Constructing TGraph" << endl;
  TGraphAsymmErrors *graph = new TGraphAsymmErrors(baseHist);
  if(!graph) {
    cout<<"Graph not custructed!"<<endl;
    assert(graph);
  }

  // Set all errors to zero
  for(Int_t iBin = 0; iBin < graph->GetN(); iBin++) {
    // graph->SetPointEXlow(iBin, 0);
    // graph->SetPointEXhigh(iBin, 0);
    graph->SetPointEYlow(iBin, 0);
    graph->SetPointEYhigh(iBin, 0);
  }
  
  for (UInt_t iBin = 0; iBin < posVals.size(); iBin++) {
    cout<<"Bin: "<<iBin
	<<"\tErrLow: "<<negVals[iBin]
	<<"\tErrHigh: "<<posVals[iBin]
	<<endl;
    graph->SetPointEYlow(iBin, negVals[iBin]);
    graph->SetPointEYhigh(iBin, posVals[iBin]);
  }

  return graph;
}

TF1* AddConstantTF1sInQuadrature(vector<TF1*> &vec, Bool_t isPosErrors) {
  // Add positive errors in quadrature (or negative if !isPosErrors)
  // This will not work as desired if the TF1s have different ranges.
  // *** Only works for TF1s in the form f(x) = [0] ***
  Double_t quadPar = 0.;
  for (UInt_t iFit = 0; iFit < vec.size(); iFit++) {
    TF1 *fit = vec[iFit];
    Double_t par = fit->GetParameter(0);
    if (isPosErrors && par < 0.) continue;
    if (!isPosErrors && par > 0.) continue;
    quadPar += pow(par,2);
  }

  quadPar = sqrt(quadPar);
  if(!isPosErrors) quadPar *= -1.;

  TF1 *combinedTF1 = (TF1*) vec[0]->Clone();
  combinedTF1->SetParameter(0, quadPar);
  return combinedTF1;
}


TString GetBaseName(Int_t nameIndex)
{
  // cout<<"Getting base name"<<endl;
  vector<TString> names = {"CFLamALam010", "CFLamALam1030", "CFLamALam3050",
			   "CFLLAA010", "CFLLAA1030", "CFLLAA3050"};

  if (nameIndex > names.size()) {
    cout<<"No name for index value "<< nameIndex << endl;
    return "NoNameFound";
  } else {
    return names[nameIndex];
  }
}


TH1D *GetBaseHistogram(Int_t iSys, TString filePath, StudyType sysStudyType)
{
  // cout<<"Getting base histogram"<<endl;
  // Grab the correlation function corresponding to the index.
  // This is the default CF that uses all nominal cut values.

  TFile inputFile(filePath, "read");
  TString dirPath;
  if (kTopStudy == sysStudyType) {
    dirPath = "Var0/Cut1/Merged";
  } else if (kAvgSepStudy == sysStudyType) {
    dirPath = "Study0Var0/Cut1/Merged";
  } else {
    cout << "No systematics for this SysStudyType" << endl;
    return NULL;
  }
  
  TDirectory *dir = inputFile.GetDirectory(dirPath);
  if(!dir) {
    cout << "could not find base histogram directory at "
	 << dirPath << endl;
    return NULL;
  }
  // get the right histogram
  TString histName = GetBaseName(iSys);
  TH1D *baseHist = (TH1D*)dir->Get(histName);
  assert(baseHist);

  baseHist->SetDirectory(0);
  // cout<<"We have found the histogram"<<endl;
  return baseHist;
}

void CombineSystematicsForStudy(TString filePath, StudyType sysStudyType, Bool_t shouldAddInQuad) {
  filePath += "/CFs.root";

  // Get and sort all the error histograms
  vector<TF1*> vecAll = GetAllTF1sFromFile(filePath);
  vector< vector<TF1*> > sortedVec = SortTF1s(vecAll);

  // Setup the output directory
  TFile outputFile("SysErrors.root", "update");
  TString sysStudyDir;
  if (kTopStudy == sysStudyType) {
    sysStudyDir = "TopologicalSystematics";
  } else if (kAvgSepStudy == sysStudyType) {
    sysStudyDir = "AvgSepSystematics";
  }
  TDirectory *outputDir = outputFile.GetDirectory(sysStudyDir);
  if(!outputDir) {
    outputDir = outputFile.mkdir(sysStudyDir);
  }
  
  // Loop over each cf system and combine
  // the errors for that system
  for (UInt_t iSys = 0; iSys < sortedVec.size(); iSys++) {
    vector< vector<Double_t> > errVecs;
    
    // Turn all the fits into vectors of doubles
    UInt_t vecSize = 0;
    for (UInt_t iFit = 0; iFit < sortedVec[iSys].size(); iFit++) {
      errVecs.push_back(ConvertTF1ToVector(sortedVec[iSys][iFit]));

      // Make sure all the vectors are the same length
      const UInt_t thisSize = errVecs.back().size();
      if (iFit > 0) {
	assert(thisSize == vecSize);
      }
      vecSize = thisSize;
    }

    // Combine the errors and store them into posVals and negVals
    vector<Double_t> posVals;
    vector<Double_t> negVals;
    if (shouldAddInQuad) {
      CombinePosNegSeparately(errVecs, posVals, negVals);
    } else {
      FindMaximumPosNegValues(errVecs, posVals, negVals);
    }
    // Make and output the TGraphAsymmErrors
    TH1D* baseHist = GetBaseHistogram(iSys, filePath, sysStudyType);
    TGraphAsymmErrors *graphAsymm = ConstructAsymmTGraph(baseHist, posVals, negVals);

    cout << "Writing TGraph ";
    TString graphName = GetBaseName(iSys);
    cout << "with name " << graphName << endl;
    graphName += "AsymmErrors";
    graphAsymm->SetName(graphName);
    graphAsymm->SetTitle(graphName);
    outputDir->cd();
    graphAsymm->Write(graphName, TObject::kOverwrite);
    baseHist->Write(baseHist->GetName(), TObject::kOverwrite);
    // graphAsymm->SetDirectory(0);
    baseHist->SetDirectory(0);
    cout << "Finished writing for system " << iSys << endl;
  }
  cout << "All done combining systematics for this study" << endl;
}

void CombineSystematics(Bool_t shouldAddInQuad) {

  TString filePathTop = "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-03/04-Train-SysCutChecks";
  CombineSystematicsForStudy(filePathTop, kTopStudy, shouldAddInQuad);

  TString filePathAvgSep = "/home/jai/Analysis/lambda/AliAnalysisLambda/Results/2016-04/08-Train-TTCSys";
  CombineSystematicsForStudy(filePathAvgSep, kAvgSepStudy, shouldAddInQuad);

}
