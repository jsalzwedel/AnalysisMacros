// Read in CFs.root
// Find all the fits with a particular name (e.g. LamALam010...)
// Add them together (in quadrature?)
// Save the new fit/histogram as the systematic uncertainty on that hist.
// Repeat for other names

// Maybe loop through the TDirectories and grab all the TF1 objects,
// add them to a vector, and then sort them out later?


void AddTF1sFromSubdirToVector(vector<TF1*> &vec, TDirectory *currentDir)
{
  currentDir->ls();

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
      TF1 *fitResult = (TF1*) currentDir->Get(key->GetName());
      vec.push_back(fitResult);
    }
  }
}

vector<TF1*> GetAllTF1sFromFile(TString fileName)
{
  TFile inputFile(fileName);

  vector<TF1*> vecTF1;
  AddTF1sFromSubdirToVector(vecTF1, &inputFile);

  cout<<"Found "<<vecTF1.size()<<" TF1s."<<endl;

  for (UInt_t iFit = 0; iFit < vecTF1.size(); iFit++) {
    cout<<vecTF1[iFit]->GetName()<<endl;
  }
  return vecTF1;
}


vector< vector<TF1*> > SortTF1s(vector<TF1*> &vecAll)
{
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
  TH1D *hist = fit->GetHistogram();
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
  assert(posVals.size() == negVals.size());

  // Grab an error vector
  for (UInt_t iErrVec = 0; iErrVec < errVectors.size(); iErrVec++) {
    vector<Double_t> &thisVec = errorVectors[iErrVec];
    assert(thisVec.size() == posVals.size());

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

  // Sqrt the results for each bin, and set negVals to be negative
  for (UInt_t iBin = 0; iBin < posVals.size(); iBin++) {
    posVals[iBin] = sqrt(posVals[iBin]);
    negVals[iBin] = sqrt(negVals[iBin]);
    negVals[iBin] *= -1.;
  } 
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
}

void CombineSystematics() {
  vector<TF1*> vecAll = GetAllTF1sFromFile("CFs.root");
  vector< vector<TF1*> > sortedVec = SortTF1s(vecAll);

  // Loop over the different cf systems
  for (UInt_t iSys = 0; iSys < sortedVec.size(); iSys++) {
    vector< vector<Double_t> > errVecs;

    // Turn all the fits into vectors of doubles
    for (UInt_t iFit = 0; iFit < sortedVecs[iSys].size(); iFit++) {
      errVecs.push_back(ConvertTF1ToVector(sortedVecs[iSys][iFit]));
    }

    vector<Double_t> posVals;
    vector<Double_t> negVals;
    CombinePosNegSeparately(errVecs, posVals, negVals);
    TGraphAsymmErrors graphAsymm = ConstructAsymmTGraphAtZero(posVals, negVals);
    
  }
}

