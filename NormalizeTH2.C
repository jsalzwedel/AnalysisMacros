TH2 *NormalizeTH2(TH2* hist, TString optionAxis)
{
    // optionAxis specifies whether to normalize the row or the column
    TString newName = hist->GetName();
    newName += "Norm";
    TH2* normHist = (TH2*)hist->Clone(newName);

    TAxis *normAxis = NULL;
    TAXIS *otherAxis = NULL;
    Bool_t isRowNorm = kTRUE;
    if ((optionAxis == "X") || (optionAxis == "x")) {
        normAxis = hist->GetXaxis();
        otherAxis = hist->GetYaxis();
    }
    else if ((optionAxis == "Y") || (optionAxis == "y")) {
        normAxis = hist->GetYaxis();
        otherAxis = hist->GetXaxis();
        isRowNorm = kFALSE;
    } else {
        cout << "Not a valid option. Must choose X or Y." << endl;
        assert(1 == 0);
    }

    Int_t nBinsNorm = normAxis->GetNbins();

    if (isRowNorm) {
        // Normalizing rows
        for (Int_t iRow = 1; iRow <= nBins; iRow++) {
            Double_t norm = 0.;
            for (Int_t iCol = 1; iCol < otherAxis->GetNbins(); iCol++) {
                norm += hist->GetBinContent(iRow, iCol);
            }
            if (norm < 0.01) continue; // Avoid divide by zero
            // Now normalize the row
            for (Int_t iCol = 1; iCol < otherAxis->GetNbins(); iCol++) {
                Double_t binVal = normHist->GetBinContent(iRow,iCol);
                normHist->SetBinContent(iRow, iCol, binVal/norm);
            }
        }
    } else {
        for (Int_t iCol = 1; iCol <= nBins; iCol++) {
            Double_t norm = 0.;
            for (Int_t iRow = 1; iRow < otherAxis->GetNbins(); iRow++) {
                norm += hist->GetBinContent(iRow, iCol);
            }
            if (norm < 0.01) continue; // Avoid divide by zero
            // Now normalize the row
            for (Int_t iRow = 1; iRow < otherAxis->GetNbins(); iRow++) {
                Double_t binVal = normHist->GetBinContent(iRow,iCol);
                normHist->SetBinContent(iRow, iCol, binVal/norm);
            }
        }
    }
    return normHist;
}
