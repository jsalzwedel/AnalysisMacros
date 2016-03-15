// Get data directories contained within CFs.root.  Used in MakeCFs and
// MakeCombinedCfs.

#ifndef GET_DATA_DIR_C
#define GET_DATA_DIR_C

vector<TDirectory*> GetDataDirectories(TFile& inputFile, Bool_t isTrainResult)
{
  vector<TDirectory*> dataDirs;
  if(isTrainResult) {
    // Get all the the directories and find their subdirectories
    TList *dirList = inputFile.GetListOfKeys();
    TIter dirIter(dirList);
    TObject *dirObj = NULL;

    // Loop over all variable cut directories
    // and cut bin directories and add them to dataDirs
    while((dirObj = dirIter())) { // var cut directories
      TKey *dirKey = dynamic_cast<TKey*>(dirObj);
      TDirectory *currentDir = dynamic_cast<TDirectory*>(dirKey->ReadObj());
      if(!currentDir) continue;
      TString currentDirName = currentDir->GetName();
      assert(currentDirName.Contains("Var"));
      // This dir exists.  Now grab all cut bin directories inside
      TList *cutList = currentDir->GetListOfKeys();
      TIter cutIter(cutList);
      TObject *cutDirObj = NULL;
      while((cutDirObj = cutIter())) { // cut bin directories
	TKey *cutKey = dynamic_cast<TKey*>(cutDirObj);
	TDirectory *cutDir = dynamic_cast<TDirectory*>(cutKey->ReadObj());
	if(!cutDir) continue;
	TString cutDirName = cutDir->GetName();
	assert(cutDirName.Contains("Cut"));

	//If we got this far, the directory exists. Add it to the vector
	dataDirs.push_back(cutDir);
      } // End cut bin directory while loop
    } // End var cut directory while loop 
  } else {
    // The file itself is the only data directory.
    dataDirs.push_back(&inputFile);
  }
  return dataDirs;
}

#endif
