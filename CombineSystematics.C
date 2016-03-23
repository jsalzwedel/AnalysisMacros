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

  for(UInt_t iFit = 0; iFit < vecTF1.size(); iFit++) {
    cout<<vecTF1[iFit]->GetName()<<endl;
  }
  return vecTF1;
}


void CombineSystematics() {
  vector<TF1*> vecAll = GetAllTF1sFromFile("CFs.root");
}
