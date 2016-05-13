// Read in each transform TH2
// Read in the LLAAMixed and LAMixed momentum resolution TH2s
// Convert each one to a TMatrixT
// Transpose the transform TH2Fs to get k*_XX on x-axis and k*_LL on y-axis
// Do matrix multiplication
// Convert matrix back into TH2F?
// Save to file

// Save LLAAMixed and LAMixed to file as smearing for primary particles

TMatrix ConvertTH2FtoMatrix(TH2F* hist, Bool_t shouldTranspose)
{
    cout << "ConvertTH2toMatrix Begin" << endl;
    if (!hist) {
        cout << "No hist found. Cannot convert to TMatrix!" << endl;
        assert(hist);
    }
    Int_t nBinsX = hist->GetNbinsX();
    Int_t nBinsY = hist->GetNbinsY();
    TMatrix matrix(nBinsX, nBinsY);

    // Manually set all the matrix elements
    for (Int_t iX = 0; iX < nBinsX; iX++) {
      for (Int_t iY = 0; iY < nBinsY; iY++) {
	matrix(iX, iY) = hist->GetBinContent(iX + 1, iY + 1);
	// cout << iX << "\t" << iY << "\t" << matrix(iX, iY) << endl;
      }
    }

    
    
    cout << "Determinant:\t" << matrix.Determinant() << endl;
    if (shouldTranspose) {

        matrix.Transpose(matrix);
    }
    cout << "ConvertTH2toMatrix End" << endl;
    return matrix;
}



TMatrix GetCombinedMatrix(TMatrix &residualMat, TMatrix &resolutionMat)
{
    cout << "GetCombinedMatrix Begin" << endl;
    TMatrix combinedMatrix(residualMat.GetNrows(), residualMat.GetNcols());
    combinedMatrix.Mult(resolutionMat, residualMat);
    cout << "GetCombinedMatrix End" << endl;
    return combinedMatrix;
}

// Need to get and make a combined smeared matrix for each residual pair type,
// both for LLAA and LA.

vector<TString> GetResidualHistNames()
{
    vector<TString> histNames = {"SigmaLambda", "SigmaSigma", "Xi0Lambda",
                                 "SigmaXi0", "SigmaXiC", "XiCLambda", "XiCXi0"};
    for (UInt_t iName = 0; iName < histNames.size(); iName++) {
        histNames[iName].Prepend("TransformMatrix");
        histNames[iName].Append("Norm");
    }
    return histNames;
}

vector<TMatrix> GetResidualMatrices(vector<TString> histNames)
{
    cout << "GetResidualMatrices Begin" <<endl;
    TString filePath = "$RESULTS/AnalysisResults/NormalizedTransformMatrices.root";
    TFile inFile(filePath);
    TDirectory *dir = inFile.GetDirectory("Normalized");

    vector<TMatrix> residualMatrices;
    for (UInt_t iName = 0; iName < histNames.size(); iName++) {
        TH2F* hist = (TH2F*) dir->Get(histNames[iName]);
	if (!hist) {
	  cout << "Could not find histogram with name " << histNames[iName] << endl;
	  continue;
	}
        Bool_t shouldTranspose = kTRUE;
        TMatrix residualMatrix = ConvertTH2FtoMatrix(hist, shouldTranspose);
        residualMatrices.push_back(residualMatrix);
    }

    cout << "GetResidualMatrices End" <<endl;

    return residualMatrices;
}


vector<TString> GetSmearMatrixNames(vector<TString> smearNames, Bool_t isLLAA)
{
    cout << "GetSmearMatrixNames Begin" <<endl;
    for (UInt_t iName = 0; iName < smearNames.size(); iName++) {
        smearNames[iName].ReplaceAll("Transform", "Smear");
        if (isLLAA) smearNames[iName] += "LLAA";
        else smearNames[iName] += "LA";
    }
    cout << "GetSmearMatrixNames End" <<endl;

    return smearNames;
}

vector<TMatrix> MakeCombinedMatricesForPairType(TMatrix &resolutionMatrix,
                                                 vector<TMatrix> &residualMatrices)
{
    cout << "MakeCombinedMatricesForPairType Begin" << endl;
    // For each residual correlation matrix, multiply it with the common
    // resolution matrix to get a combined smeared matrix. Designed to handle
    // LLAA and LA matrices separately
    vector<TMatrix> smearedMatrices;
    for (UInt_t iMat = 0; iMat < residualMatrices.size(); iMat++) {
        TMatrix combinedMat = GetCombinedMatrix(residualMatrices[iMat],
						resolutionMatrix);
        smearedMatrices.push_back(combinedMat);
    }
    cout << "MakeCombinedMatricesForPairType End" << endl;
    return smearedMatrices;
}

vector<TH2F*> ConvertTMatricesToTH2Fs(vector<TMatrix> &matrices,
                                      vector<TString> smearNames)
{
    cout << "Convert Matrices to TH2Fs Begin" << endl;
    assert(matrices.size() == smearNames.size());

    vector<TH2F*> hists;
    for (UInt_t iMat = 0; iMat < matrices.size(); iMat++) {
        TH2F* histogram = new TH2F(matrices[iMat]);
        histogram->SetName(smearNames[iMat]);
        histogram->SetTitle(smearNames[iMat]);
	histogram->SetBins(200, 0., 2., 200, 0., 2.); 
        hists.push_back(histogram);
    }

    cout << "Convert Matrices to TH2Fs end" << endl;
    return hists;
}

void CombineNormalizedMatrices()
{
    cout << "Begin macro" << endl;
    // Get the momentum resolution histograms and convert them into matrices
    TFile resolutionFile("MomentumResolutionMatrices.root","read");
    TH2F *resolutionLLAAHist = (TH2F*)resolutionFile.Get("Normalized/fResMatrixLLAAMixedAllRebinNorm");
    TMatrix resolutionLLAAMatrix = ConvertTH2FtoMatrix(resolutionLLAAHist, kFALSE);
    TH2F *resolutionLAHist = (TH2F*)resolutionFile.Get("Normalized/fResMatrixLAMixedAllRebinNorm");
    TMatrix resolutionLAMatrix = ConvertTH2FtoMatrix(resolutionLAHist, kFALSE);

    // Get the residual correlation histograms and convert them into matrices
    TFile residualFile("NormalizedTransformMatrices.root");
    vector<TString> histNames = GetResidualHistNames();
    vector<TMatrix> residualMatrices = GetResidualMatrices(histNames);

    // Create names for the final smear histograms
    vector<TString> smearNamesLLAA = GetSmearMatrixNames(histNames, kTRUE);
    vector<TString> smearNamesLA = GetSmearMatrixNames(histNames, kFALSE);

    // Make combined smear matrices
    vector<TMatrix> smearMatricesLLAA = MakeCombinedMatricesForPairType(resolutionLLAAMatrix,
                                                                         residualMatrices);
    vector<TMatrix> smearMatricesLA = MakeCombinedMatricesForPairType(resolutionLAMatrix,
                                                                       residualMatrices);
    // Convert the smear matrices back into histograms
    vector<TH2F*> smearHistsLLAA = ConvertTMatricesToTH2Fs(smearMatricesLLAA, smearNamesLLAA);
    vector<TH2F*> smearHistsLA = ConvertTMatricesToTH2Fs(smearMatricesLA, smearNamesLA);

    // Save the histograms
    TFile outFile("SmearHistograms.root","Update");
    for (UInt_t iHist = 0; iHist < smearHistsLLAA.size(); iHist++) {
        outFile.cd();
	smearHistsLLAA[iHist]->GetYaxis()->SetTitle("k*^{recon}_{#Lambda#Lambda}");
	smearHistsLLAA[iHist]->GetXaxis()->SetTitle("k*^{true}");
        smearHistsLLAA[iHist]->Write(smearHistsLLAA[iHist]->GetName(),
                                     TObject::kOverwrite);
    }
    for (UInt_t iHist = 0; iHist < smearHistsLA.size(); iHist++) {
        outFile.cd();
	smearHistsLA[iHist]->GetYaxis()->SetTitle("k*^{recon}_{#Lambda#bar{#Lambda}}");
	smearHistsLA[iHist]->GetXaxis()->SetTitle("k*^{true}");
        smearHistsLA[iHist]->Write(smearHistsLA[iHist]->GetName(),
                                   TObject::kOverwrite);
    }

    // Save smear histograms for primary LL and LA.
    // No residual transform, so just resave the resolution histogram
    TString smearPrimaryLambdaNameLLAA = "SmearMatrixLambdaLambdaNormLLAA";
    resolutionLLAAHist->SetTitle(smearPrimaryLambdaNameLLAA);
    resolutionLLAAHist->GetYaxis()->SetTitle("k*^{recon}_{#Lambda#Lambda}");
    resolutionLLAAHist->GetXaxis()->SetTitle("k*^{true}");
    resolutionLLAAHist->Write(smearPrimaryLambdaNameLLAA, TObject::kOverwrite);

    TString smearPrimaryLambdaNameLA = "SmearMatrixLambdaLambdaNormLA";
    resolutionLAHist->SetTitle(smearPrimaryLambdaNameLA);
    resolutionLAHist->GetYaxis()->SetTitle("k*^{recon}_{#Lambda#bar{#Lambda}}");
    resolutionLAHist->GetXaxis()->SetTitle("k*^{true}");
    resolutionLAHist->Write(smearPrimaryLambdaNameLA, TObject::kOverwrite);
    cout << "End macro" << endl;

}
