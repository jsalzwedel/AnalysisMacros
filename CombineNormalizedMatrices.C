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
    Int_t nBinsX = hist->GetNbinsX();
    Int_t nBinsY = hist->GetNbinsY();
    TMatrix *matrix = new TMatrix(nBinsX+2, nBinsY+2, hist->GetArray(), "F"); // Not sure why,
    // but the example has +2 in it...
    return matrix;
}



TMatrix *GetCombinedMatrix(TMatrix *residualMat, TMatrix *resolutionMat)
{
    TMatrix *combinedMatrix = new TMatrix();
    combinedMatrix->Mult(residualMat, resolutionMat);

    return combinedMatrix;
}
