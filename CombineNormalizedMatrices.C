// Read in each transform TH2
// Read in the LLAAMixed and LAMixed momentum resolution TH2s
// Convert each one to a TMatrixT
// Transpose the transform TH2Fs to get k*_XX on x-axis and k*_LL on y-axis
// Do matrix multiplication
// Convert matrix back into TH2F?
// Save to file

// Save LLAAMixed and LAMixed to file as smearing for primary particles

TMatrix *ConvertTH2toMatrix(TH2* hist, Bool_t shouldTranspose)
{
    cout << "ConvertTH2toMatrix Begin" << endl;
    Int_t nBinsX = hist->GetNbinsX();
    Int_t nBinsY = hist->GetNbinsY();
    TMatrix *matrix = new TMatrix(nBinsX+2, nBinsY+2, hist->GetArray(), "F");
    // Not sure why,
    // but the example has bins+2 in it...

    if (shouldTranspose) {
        matrix = matrix->Transpose(/*matrix*/);
    }
    TString matrixName = hist->GetName() + "Matrix"''
    matrix->SetName(matrixName);

    cout << "ConvertTH2toMatrix End" << endl;

    return matrix;
}



TMatrix *GetCombinedMatrix(TMatrix *residualMat, TMatrix *resolutionMat)
{
    cout << "GetCombinedMatrix Begin" << endl;
    TMatrix *combinedMatrix = new TMatrix();
    combinedMatrix->Mult(resolutionMat, residualMat);

    cout << "GetCombinedMatrix End" << endl;
    return combinedMatrix;
}

// Need to get and make a combined smeared matrix for each residual pair type,
// both for LLAA and LA.



vector<TMatrix*> GetResidualMatrices()
{
    cout << "GetResidualMatrices Begin" <<endl;
    TString filePath = "$RESULTS/AnalysisResults/NormalizedTransformMatrices.root";
    TFile inFile(filePath);
    TDirectory *dir = inFile.GetDirectory("Normalized");

    vector<TMatrix*> residualMatrices;
    vector<TString> histNames = {"SigmaLambda", "SigmaSigma", "Xi0Lambda",
                                 "SigmaXi0", "SigmaXiC", "XiCLambda", "XiCXi0"};
    for (UInt_t iName = 0; iName < histNames.size(); iName++) {
        histNames[iName].Prepend("TransformMatrix");
        histNames[iName].Append("Norm");
        TH2* hist = (TH2*) dir->Get(histNames[iName]);
        Bool_t shouldTranspose = kTRUE;
        TMatrix *residualMatrix = ConvertTH2toMatrix(hist, shouldTranspose);
        residualMatrices.push_back(residualMatrix);
    }

    cout << "GetResidualMatrices End" <<endl;

    return residualMatrices;
}

TString MakeSmearMatrixName(TMatrix *matrix, Bool_t isLLAA)
{
    cout << "MakeSmearMatrixName Begin" << endl;
    TString smearName = matrix->GetName();
    cout << "The current matrix name is " << smearName << endl;

    smearName.ReplaceAll("Transform", "Smear");
    if (isLLAA) smearName += "LLAA";
    else smearName += "LA";

    cout << "MakeSmearMatrixName End" << endl;
    return smearName;
}

vector<TString> GetSmearMatrixNames(vector<TMatrix*> matrices, Bool_t isLLAA)
{
    cout << "GetSmearMatrixNames Begin" <<endl;
    vector<TString> smearNames;
    for (UInt_t iMat = 0; iMat < matrices.size(); iMat++) {
        TString name = MakeSmearMatrixName(matrices[iMat], isLLAA);
        smearNames.push_back(name);
    }
    cout << "GetSmearMatrixNames End" <<endl;

    return smearNames;
}

vector<TMatrix*> MakeCombinedMatricesForPairType(TMatrix *resolutionMatrix,
                                                 vector<TMatrix*> residualMatrices,
                                                 vector<TString> finalNames)
{
    cout << "MakeCombinedMatricesForPairType" << endl;
    // For each residual correlation matrix, multiply it with the common
    // resolution matrix to get a combined smeared matrix. Designed to handle
    // LLAA and LA matrices separately
    assert(residualMatrices.size() == finalNames.size());
    vector<TMatrix*> smearedMatrices;
    for (UInt_t iMat = 0; iMat < residualMatrices.size(); iMat++) {
        TMatrix *combinedMat = GetCombinedMatrix(residualMatrices[iMat],
                                                 resolutionMatrix);
        combinedMat->SetName(finalNames[iMat]);
        smearedMatrices.push_back(combinedMat);
    }
    cout << "MakeCombinedMatricesForPairType End" << endl;
    return smearedMatrices;
}

vector<TH2F*> ConvertTMatricesToTH2Fs(vector<TMatrix*> matrices)
{
    cout << "Convert Matrices to TH2Fs Begin" << endl;
    vector<TH2F*> hists;
    for (UInt_t iMat = 0; iMat < matrices.size(); iMat++) {
        TH2F* histogram = new TH2F(matrices[iMat]);
        histogram->SetName(matrices[iMat]->GetName());
        histogram->SetTitle(matrices[iMat]->GetName());
        hists.push_back(histogram);
    }

    cout << "Convert Matrices to TH2Fs end" << endl;
    return hists;
}

void CombineNormalizedMatrices()
{
    cout << "Begin macro" >> endl;
    // Get the momentum resolution histograms and convert them into matrices
    TFile resolutionFile("MomentumResolutionMatrices.root","read");
    TH2 *resolutionLLAAHist = (TH2*)resolutionFile->Get("Noramalized/fResMatrixLLAAMixedAllRebinNorm");
    TMatrix *resolutionLLAAMatrix = ConvertTH2toMatrix(resolutionLLAAHist, kFALSE);
    TH2 *resolutionLAHist = (TH2*)resolutionFile->Get("Noramalized/fResMatrixLAMixedAllRebinNorm");
    TMatrix *resolutionLAMatrix = ConvertTH2toMatrix(resolutionLAHist, kFALSE);

    // Get the residual correlation histograms and convert them into matrices
    TFile residualFile("NormalizedTransformMatrices.root");
    vector<TMatrix*> residualMatrices = GetResidualMatrices();

    // Get names for the final smear histograms
    TString smearNamesLLAA = GetSmearMatrixNames(residualMatrices, kTRUE);
    TString smearNamesLA = GetSmearMatrixNames(residualMatrices, kFALSE);

    // Make combined smear matrices
    vector<TMatrix*> smearMatricesLLAA = MakeCombinedMatricesForPairType(resolutionLLAAMatrix,
                                                                         residualMatrices,
                                                                         smearNamesLLAA);
    vector<TMatrix*> smearMatricesLA = MakeCombinedMatricesForPairType(resolutionLAMatrix,
                                                                       residualMatrices,
                                                                       smearNamesLA);
    // Convert the smear matrices back into histograms
    vector<TH2F*> smearHistsLLAA = ConvertTMatricesToTH2Fs(smearMatricesLLAA);
    vector<TH2F*> smearHistsLA = ConvertTMatricesToTH2Fs(smearMatricesLA);

    // Save the histograms

    TFile outFile("SmearHistograms.root","Update");
    for (UInt_t iHist = 0; iHist < smearHistsLLAA.size(); iHist++) {
        outFile.cd();
        smearHistsLLAA[iHist]->Write(smearHistsLLAA[iHist]->GetName(),
                                     TObject::kOverwrite);
    }
    for (UInt_t iHist = 0; iHist < smearHistsLA.size(); iHist++) {
        outFile.cd();
        smearHistsLA[iHist]->Write(smearHistsLA[iHist]->GetName(),
                                   TObject::kOverwrite);
    }
    cout << "End macro" >> endl;

}
