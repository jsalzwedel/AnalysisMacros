// ******** Workflow *******
// Needs to be run using Root 6
//
// (0. Make CFs using MakeCFs.C)
// 1. Run MakeCombinedCFs()
// 2. Run CombineCentralitiesForEachPairType()
// 3. Run CombineLLAA()
//
// *************************



#include <iostream>
#include <vector>
#include "TDirectory.h"
#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TVectorD.h"
#include "TKey.h"


using namespace std;


vector<TDirectory*> GetDirectories(TFile &f, vector<TString> dataSetNames, TString dirName)
{
  vector<TDirectory*> dirs;
  // if(dataSetNames.size() < 2) {
  //   cout<<"
  // }

  UInt_t nDirs = dataSetNames.size();

  for(UInt_t i = 0; i < nDirs; i++) {
    // TDirectory *dir = (TDirectory*) f.Get(dataSetNames[i]);
    TString fullDirName = dataSetNames[i] + "/" + dirName;
    TDirectory *dir = (TDirectory*) f.Get(fullDirName);
    assert(dir);
    dirs.push_back(dir);
  }
  return dirs;
}


TH1D *CombineCFs(vector<TH1D*> cfs, vector<Double_t> counts)
{
  // Take in a vector of cfs and a vector of counts, and use them to
  // make an averaged correlation function
  UInt_t nCFs = cfs.size();

  if(nCFs < 2) {
    cout<<"Not enough cfs in collection:\t"<<nCFs<<endl
	<<"Cannot combine."<<endl;
    return NULL;
  }
  assert(nCFs == counts.size());

  TH1D *combinedCF = (TH1D*)cfs[0]->Clone();
  combinedCF->Scale(counts[0]);
  Double_t totalCounts = counts[0];

  // Add together weighted cfs
  for(UInt_t i = 1; i < nCFs; i++) {
    TH1D *copyCF = (TH1D*) cfs[i]->Clone();
    Double_t thisCount = counts[i];
    copyCF->Scale(thisCount);
    combinedCF->Add(copyCF);
    totalCounts += thisCount;
    delete copyCF; copyCF = NULL;
  }
  
  // Now get average
  combinedCF->Scale(1./totalCounts);
  return combinedCF;
}


void MakeCombinedCFs()
{
  cout<<"Making combined cfs"<<endl;

  // Get the directories for the cfs and counts
  TString dataSetNamesArr[] = {"mm1", "mm2", "mm3", "pp1", "pp2"};
  vector<TString> dataSetNames(dataSetNamesArr, dataSetNamesArr+5);
  TString cfDirName = "CF";
  TString countDirName = "Count";
  TFile f("CFs.root","update");
  vector<TDirectory*> cfDirs = GetDirectories(f, dataSetNames, cfDirName);
  vector<TDirectory*> countDirs = GetDirectories(f, dataSetNames, countDirName);
  TDirectory *mergeDir = (TDirectory*)f.GetDirectory("Merged");
  if(!mergeDir) {
    mergeDir = f.mkdir("Merged");
  }

  UInt_t nDirs = cfDirs.size();
  // Merge each centrality one at a time
  // Load the cfs and counts into vectors for that centrality
  // Find all the cfs and counts that match the name of the CF
  // in the first directory
  TIter iter(cfDirs[0]->GetListOfKeys());
  TObject *obj = NULL;

  while ( (obj = iter()) ) {
    TKey *key = dynamic_cast<TKey*>(obj);
    TH1D *cf = dynamic_cast<TH1D*>(key->ReadObj());
    if(!cf) {
      cout<<"Could not find a CF hist"<<endl;
      return;
    }
    //Figure out which CF we are trying to grab
    TString cfName = cf->GetName();
    TString countName = cfName;
    countName.ReplaceAll("CF","Count");

    //Grab the corresponding CFs and counts
    vector<TH1D*> cfs;
    vector<Double_t> counts;
    Double_t totalCounts = 0;
    for(UInt_t iDir = 0; iDir < nDirs; iDir++) {
      TH1D *thisCF = (TH1D*) cfDirs[iDir]->Get(cfName);
      if(!thisCF) {
	cout<<"Could not find CF matching "<<cfName
	    <<" in "<<cfDirs[iDir]->GetName()<<endl;
      }
      // TVectorD *count = dynamic_cast<TVectorD*>(countDirs[iDir]->Get(countName));
      TVectorD *count = (TVectorD*)countDirs[iDir]->Get(countName);
      // cout<<count->ClassName()<<endl;
      Double_t myCount = count[0](0);
      
      cfs.push_back(thisCF);
      counts.push_back(myCount);
      totalCounts += myCount;
    }
    // Finally, combine the CFs
    TH1D *combinedCF = CombineCFs(cfs, counts);
    TVectorD finalCount(1);
    finalCount[0] = totalCounts;

    cout<<"Writing combined CF "<<combinedCF->GetName()
    	<<" to "<<mergeDir->GetName()<<endl;
    combinedCF->SetDirectory(0);
    mergeDir->cd();
    combinedCF->Write(combinedCF->GetName(), TObject::kOverwrite);
    finalCount.Write(countName, TObject::kOverwrite);
  }
  // cfs.push_back((TH1D*)cfsDirs[0]->Get)
  
}

void CombineCentralities(TString pairType)
{
  // Gather the cfs and counts to combine centrality bins


  vector<TString> centBins010 = {"05", "510"};
  vector<TString> centBins1030 = {"1015", "1520", "2025", "2530"};
  vector<TString> centBins3050 = {"3035", "3540", "4045", "4550"};

  vector<TString> finalCentBins = {"010", "1030", "3050"};

  vector<vector<TString> > centBins;
  centBins.push_back(centBins010);
  centBins.push_back(centBins1030);
  centBins.push_back(centBins3050);

  TFile f("CFs.root","update");
  TDirectory *mergeDir = (TDirectory*) f.Get("Merged");
  if(!mergeDir) {
    cout<<"Merge directory does not exist. Cannot merge."<<endl;
    return;
  }

  //For each merge group, get the necessary CFs and counts
  for(UInt_t iMerge = 0; iMerge < centBins.size(); iMerge++) {
    vector<TH1D*> cfs;
    vector<Double_t> counts;
    Double_t totalCounts = 0;
    for(UInt_t iCF = 0; iCF < centBins[iMerge].size(); iCF++) {
      TString cfName = "CF" + pairType + centBins[iMerge][iCF];
      TH1D *cf = (TH1D*)mergeDir->Get(cfName);
      if(!cf) {
	cout<<"Could not find CF named "<<cfName<<" in "<<mergeDir->GetName()<<endl;
	return;
      }
      cfs.push_back(cf);
      TString countName = "Count" + pairType + centBins[iMerge][iCF];
      TVectorD *count = (TVectorD*) mergeDir->Get(countName);
      totalCounts += count[0](0);
      counts.push_back(count[0](0));
    }


    // Finally, combine the CFs
    TH1D *combinedCF = CombineCFs(cfs, counts);
    TVectorD finalCount(1);
    finalCount[0] = totalCounts;

    // Set names
    TString combinedCFName = "CF" + pairType + finalCentBins[iMerge];
    TString combinedCountName = "Count" + pairType + finalCentBins[iMerge];
    combinedCF->SetName(combinedCFName);
    combinedCF->SetTitle(combinedCFName);

    // Set axis ranges
    combinedCF->SetAxisRange(0.9, 1.1, "Y");
    combinedCF->SetAxisRange(0., 1., "X");
    
    //

    cout<<"Writing combined CF "<<combinedCF->GetName()
    	<<" to "<<mergeDir->GetName()<<endl;
    combinedCF->SetDirectory(0);
    mergeDir->cd();
    combinedCF->Write(combinedCF->GetName(), TObject::kOverwrite);
    finalCount.Write(combinedCountName, TObject::kOverwrite);

    
  }
  

}


void CombineCentralitiesForEachPairType()
{
  // Run after merging data sets

  
  vector<TString> pairNames = {"LamLam", "ALamALam", "LamALam"};
  for(UInt_t i = 0; i < pairNames.size(); i++) {
    CombineCentralities(pairNames[i]);
  }
}


void CombineLLAA()
{
  // Run after merging centralities
  vector<TString> centBins = {"010", "1030", "3050"};
  vector<TString> pairTypes = {"LamLam", "ALamALam"};
  TString finalPairType = "LLAA";


  // Get data dir
  TFile f("CFs.root","update");
  TDirectory *mergeDir = (TDirectory*) f.Get("Merged");
  if(!mergeDir) {
    cout<<"Merge directory does not exist. Cannot merge."<<endl;
    return;
  }


  for(UInt_t iCent = 0; iCent < centBins.size(); iCent++) {
    vector<TH1D*> cfs;
    vector<Double_t> counts;
    Double_t totalCounts = 0;
    for(UInt_t iType = 0; iType < pairTypes.size(); iType++) {
      TString cfName = "CF" + pairTypes[iType] + centBins[iCent];
      TH1D *cf = (TH1D*)mergeDir->Get(cfName);
      if(!cf) {
	cout<<"Could not find CF named "<<cfName<<" in "<<mergeDir->GetName()<<endl;
	return;
      }
      cfs.push_back(cf);
      TString countName = "Count" + pairTypes[iType] + centBins[iCent];
      TVectorD *count = (TVectorD*) mergeDir->Get(countName);
      totalCounts += count[0](0);
      counts.push_back(count[0](0));
    }

    // Finally, combine the CFs
    TH1D *combinedCF = CombineCFs(cfs, counts);
    TVectorD finalCount(1);
    finalCount[0] = totalCounts;

    // Set names
    TString combinedCFName = "CF" + finalPairType + centBins[iCent];
    TString combinedCountName = "Count" + finalPairType + centBins[iCent];
    combinedCF->SetName(combinedCFName);
    combinedCF->SetTitle(combinedCFName);

    // Set axis ranges
    combinedCF->SetAxisRange(0.9, 1.1, "Y");
    combinedCF->SetAxisRange(0., 1., "X");
    
    //

    cout<<"Writing combined CF "<<combinedCF->GetName()
    	<<" to "<<mergeDir->GetName()<<endl;
    combinedCF->SetDirectory(0);
    mergeDir->cd();
    combinedCF->Write(combinedCF->GetName(), TObject::kOverwrite);
    finalCount.Write(combinedCountName, TObject::kOverwrite);
  
  }
}
