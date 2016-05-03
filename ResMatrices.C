

vector<TH2F*> GetMatricesFromTrainResults()
{
  // Gather all the matrices (TH2F) from the ResolutionMatrices TObjArray
  
  TFile inFile("AnalysisResults.root","READ");
  TList *myList = (TList*)inFile.Get("Results/MyListStudy0Var0");
  TObjArray *matrixArr = (TObjArray*)myList->FindObject("ResolutionMatrices");


  vector<TH2F*> matrices;
  TIter iter(matrixArr);
  TObject *obj = NULL;

  while( (obj = iter()) ) {
    TH2F *hist2D = dynamic_cast<TH2F*>(obj);
    if (!hist2D) {
      cout << "This object is not a TH2F" << endl;
      continue;
    }
    cout << "Found hist named " << hist2D->GetName() << endl;
    hist2D->SetDirectory(0);
    matrices.push_back(hist2D);
  }

  return matrices;
}


void WriteMatrices(vector<TH2F*> matrices, TString dirName)
{
  // Write momentum resolution matrices to a file
  TFile outFile("MomentumResolutionMatrices.root", "update");
  TDirectory *outDir = outFile.GetDirectory(dirName);
  if (!outDir) {
    outDir = outFile.mkdir(dirName);
  }

  outDir.cd();
  for (UInt_t iMat = 0; iMat < matrices.size(); iMat++) {
    TH2F* matrix = matrices[iMat];
    matrix->Write(matrix->GetName(), TObject::kOverwrite());
  }
}

TH2F* CombineMatrices(TH2F *mat1, TH2F *mat2, TString combinedName)
{
  // For combining the LL and AA results
  TH2F *matCombined = (TH2F*)mat1->Clone(combinedName);
  matCombined->Add(mat2);
  return matCombined;
}

TH2F* MakeRebinnedMatrix(TH2F *matrix, Int_t rebinNumber)
{
  // Rebin both axes of a matrix by the specified quantity.
  TString newName = matrix->GetName() + "Rebin";
  TH2F* rebinnedMatrix = (TH2F*)matrix->Clone(newName);
  rebinnedMatrix->GetXaxis()->Rebin(rebinNumber);
  rebinnedMatrix->GetYaxis()->Rebin(rebinNumber);
  return rebinnedMatrix;
}



TH2F* MergeResolutionAndResidualMatrix(TH2F *resolution, TH2F *residual)
{
  // Take the residual correlation smearing matrix and multiply it with the momentum resolution smearing matrix. 
  
}





void ResMatrices()
{
  cout << "No default actions to run with ResMatrices." << endl
       << "Instead, load in the macro and run one of the following:"
       << endl
       << "MergeAllResolutionMatrices()" << endl
       << "ProcessMatrixResults" << endl;
}
