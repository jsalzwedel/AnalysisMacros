

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


void WriteMatrices(const vector<TH2F*> &matrices, TString dirName)
{
  // Write momentum resolution matrices to a file
  TFile outFile("MomentumResolutionMatrices.root", "update");
  TDirectory *outDir = outFile.GetDirectory(dirName);
  if (!outDir) {
    outDir = outFile.mkdir(dirName);
  }

  outDir->cd();
  for (UInt_t iMat = 0; iMat < matrices.size(); iMat++) {
    TH2F* matrix = matrices[iMat];
    matrix->Write(matrix->GetName(), TObject::kOverwrite);
  }
}

TH2F* MakeRebinnedMatrix(TH2F *matrix, Int_t rebinNumber)
{
  // Rebin both axes of a matrix by the specified quantity.
  TString newName = matrix->GetName();
  newName += "Rebin";
  TH2F* rebinnedMatrix = (TH2F*)matrix->Rebin2D(rebinNumber, rebinNumber, newName);
  return rebinnedMatrix;
}

TH2F* CombineMatrices(TH2F *mat1, TH2F *mat2, TString combinedName)
{
  // For combining the LL and AA results
  TH2F *matCombined = (TH2F*)mat1->Clone(combinedName);
  matCombined->Add(mat2);
  return matCombined;
}

void GetVectorsOfMatchingLLAAMatrices(const vector<TH2F*> &matrices,
				      vector<TH2F*> &matricesLL, vector<TH2F*> &matricesAA)
{
  // Run through the list of TH2Fs in "matrices" vector.
  // For every LL matrix, put it in matricesLL.
  // Then find the corresponding AA matrix and put it in matricesAA

  for (UInt_t iMat1 = 0; iMat1 < matrices.size(); iMat1++) {
    TH2F *matrix1 = matrices[iMat1];
    if (!matrix1) continue;
    TString name = matrix1->GetName();
    if (!name.Contains("LL")) continue;
    matricesLL.push_back(matrix1);

    // Find the equivalent AA name
    name.ReplaceAll("LL", "AA");
    // Find the associated AA matrix
    for (UInt_t iMat2 = 0; iMat2 < matrices.size(); iMat2++) {
      TH2F *matrix2 = matrices[iMat2];
      if (!matrix2) continue;
      if (matrix2->GetName() != name) continue;
      cout << name << " should match " << matrix2->GetName() << endl;
      matricesAA.push_back(matrix2);
      break;
    }
  }
}

vector<TH2F*> GetCombinedLLAAMatrices(vector<TH2F*> rebinnedMatrices)
{
  vector<TH2F*> matricesLL;
  vector<TH2F*> matricesAA;
  GetVectorsOfMatchingLLAAMatrices(rebinnedMatrices, matricesLL, matricesAA);
  assert (matricesLL.size() == matricesAA.size());

  vector<TH2F*> matricesLLAA;
  for (UInt_t iMat = 0; iMat < matricesLL.size(); iMat++) {
    TString newName = matricesLL[iMat]->GetName();
    newName.ReplaceAll("LL", "LLAA");
    TH2F* combinedMatrix = CombineMatrices(matricesLL[iMat], matricesAA[iMat], newName);
    matricesLLAA.push_back(combinedMatrix);
  }
  return matricesLLAA;
}


void ExtractResMatrices()
{
  // Open train results file and load all TH2F into a vector
  // Write them as-is into a directory
  // Rebin all matrices and write to a directory
  // Merge LL with AA matrices and write to a directory
  vector<TH2F*> matrices = GetMatricesFromTrainResults();
  WriteMatrices(matrices, "Original");

  vector<TH2F*> rebinnedMatrices;
  Int_t rebinNumber = 4;
  for (UInt_t iMat = 0; iMat < matrices.size(); iMat++) {
    TH2F *rebinMat = MakeRebinnedMatrix(matrices[iMat], rebinNumber);
    rebinnedMatrices.push_back(rebinMat);
  }
  WriteMatrices(rebinnedMatrices, "Rebinned");


  vector<TH2F*> matricesLLAA = GetCombinedLLAAMatrices(rebinnedMatrices);
  WriteMatrices(matricesLLAA, "Rebinned");

}



// TH2F* MergeResolutionAndResidualMatrix(TH2F *resolution, TH2F *residual)
// {
//   // Take the residual correlation smearing matrix and multiply it with the momentum resolution smearing matrix. 
//   ret
// }





void ResMatrices()
{
  cout << "No default actions to run with ResMatrices." << endl
       << "Instead, load in the macro and run one of the following:"
       << endl
       << "MergeAllResolutionMatrices()" << endl
       << "ProcessMatrixResults" << endl;
}
