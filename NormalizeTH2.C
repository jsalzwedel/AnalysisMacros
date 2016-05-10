TH2 *NormalizeTH2(TH2* hist, TString optionAxis)
{
    // optionAxis specifies whether to normalize the row or the column
    TString newName = hist->GetName();
    newName += "Norm";
    TH2* normHist = (TH2*)hist->Clone(newName);

    TAxis *normAxis = NULL;
    TAxis *otherAxis = NULL;
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
        for (Int_t iRow = 1; iRow <= nBinsNorm; iRow++) {
            Double_t norm = 0.;
            for (Int_t iCol = 1; iCol < otherAxis->GetNbins(); iCol++) {
                norm += hist->GetBinContent(iCol, iRow);
            }
            if (norm < 0.01) continue; // Avoid divide by zero
            // Now normalize the row
            for (Int_t iCol = 1; iCol <= otherAxis->GetNbins(); iCol++) {
                Double_t binVal = normHist->GetBinContent(iCol, iRow);
                normHist->SetBinContent(iCol, iRow, binVal/norm);
            }
        }
    } else {
        for (Int_t iCol = 1; iCol <= nBinsNorm; iCol++) {
            Double_t norm = 0.;
            for (Int_t iRow = 1; iRow < otherAxis->GetNbins(); iRow++) {
                norm += hist->GetBinContent(iCol, iRow);
            }
            if (norm < 0.01) continue; // Avoid divide by zero
            // Now normalize the row
            for (Int_t iRow = 1; iRow <= otherAxis->GetNbins(); iRow++) {
                Double_t binVal = normHist->GetBinContent(iCol, iRow);
                normHist->SetBinContent(iCol, iRow, binVal/norm);
            }
        }
    }
    normHist->SetDirectory(0);
    return normHist;
}


void TestNormalize(TString axis)
{
    TFile inFile("~/Analysis/lambda/AliAnalysisLambda/Results/2016-04/29-MC-ResMatrix/MomentumResolutionMatrices.root");
    TDirectory *dir = inFile.GetDirectory("Rebinned");

    TH2F *hist = (TH2F*) dir->Get("fResMatrixLAMixedAllRebin");

    TH2* normHist = NormalizeTH2(hist, axis);

    TCanvas *c1 = new TCanvas("c1", "NormMatrix");

    normHist->SetDirectory(0);
    normHist->Draw("colz");
}

void NormalizeMatricesInDir(TDirectory *inDir, TDirectory *outDir, TString axis)
{
    // Iterate over all the objects in the input. For each TH2 object,
    // normalize it and save it in the output directory

    // TKey *key;
    TIter histIter(inDir->GetListOfKeys());
    TObject *obj = NULL;
    while ((obj = histIter())) {
        TKey *histKey = dynamic_cast<TKey*>(obj);
        TObject *obj = histKey->ReadObj();
        TH2 *hist = dynamic_cast<TH2*>(histKey->ReadObj());
        if (!(hist->Class()->InheritsFrom(TH2::Class()))) {
            cout << "Object could not be cast into a TH2" << endl;
            continue;
        }
        TH2 *normHist = NormalizeTH2(hist, axis);
        outDir->cd();
        normHist->Write(normHist->GetName(), TObject::kOverwrite);
        cout << "Wrote " << normHist->GetName() << " to " << outDir->GetName()
             << endl;
    }
    cout << "All matrices in directory have been normalized." << endl;
}



void DoMatrixNormalizations(TString fileName, TString inputDirName,
                            TString outputDirName, TString axisName)
{
    TFile matrixFile(fileName,"update");

    TDirectory *inDir = matrixFile.GetDirectory(inputDirName);
    if (!inDir) {
        cout << "Could not find directory of files to rebin" << endl;
        return;
    }

    TDirectory *outDir = matrixFile.GetDirectory(outputDirName);
    if (!outDir) {
        outDir = matrixFile.mkdir(outputDirName);
    }

    NormalizeMatricesInDir(inDir, outDir, axisName);

}


void DoResMatrixNormalizations()
{
    DoMatrixNormalizations("MomentumResolutionMatrices.root", "Rebinned",
                            "Normalized", "X");
}

void DoTransformMatrixNormalizations()
{
    TString fileName = "PreparedTransformMatrices.root";
    // DoMatrixNormalizations(fileName, "",
    //                         "Normalized", "Y");


    TFile matrixFile(fileName,"read");

    // TDirectory *inDir = matrixFile.GetDirectory(inputDirName);
    // if (!inDir) {
    //     cout << "Could not find directory of files to rebin" << endl;
    //     return;
    // }

    TString outputFileName = "NormalizedTransformMatrices.root";
    TFile outFile(outputFileName, "update");
    TString outputDirName = "Normalized";
    TString axisName = "Y";

    TDirectory *outDir = outFile.GetDirectory(outputDirName);
    if (!outDir) {
        outDir = outFile.mkdir(outputDirName);
    }

    NormalizeMatricesInDir(&matrixFile, outDir, axisName);

    cout << "Normalizations finished" << endl;
}
