// read_bkg_us.cpp
// Author: Daniela Mercogliano
// Updated: Thomas Chillery 29/05/2025
////  Cleaned code, made it compilable instead of an interpreted macro (which can miss/substitute things), changed bad generator TRandom -> improved TRandom3    

// Usage: root -l 'read_bkg_us.cpp++(scale_factor)'
// This opens root, compiles the code, then executes it with user input "scale_factor"

// STL
#include <iostream>

// Headers
#include "TROOT.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TChain.h"
#include "TH1F.h"
#include "TF1.h"
#include "TVirtualFitter.h"
#include "TRandom3.h"
#include "TArrayS.h"
#include "TLegend.h"

Int_t read_bkg_us(Float_t scaleFact)
{	
    //re-initialize ROOT parameters
    gROOT->Reset();
    gROOT->SetStyle("Plain");
    //gStyle->SetOptStat(111111);
    gStyle->SetOptStat(0);
    gStyle->SetOptFit(1111);
    gStyle->SetPalette(1);
    gStyle->SetOptTitle(0);
    gStyle->SetHistFillColor(kWhite);

    auto c1 = new TCanvas("c1","c1",900,600);
    c1->cd();

    ///////////////////////////////
    //
    // Measured data
    //
    //////////////////////////////

    bool useListData = false; // Set to true to use energy branch, otherwise plot from CoMPASS Histogram
    
    TH1F* scaledHist;
    TH1F* histoCal;
    TH1F* TotHist;
    TH1F* TotBkg;
    TH1F* TOTBKG;
    TH1F* histoBG_un;

    //test 2 è senza shielding di Pb di 15 cm

    TFile* fMeas = new TFile("../testdata/run102.root","READ");
    int realtime = 251742; // seconds
    int nEvent;

	TChain* aChain = new TChain("Data_R");
    if(useListData) aChain->Add("../testdata/run102.root");

	ULong64_t Timestamp;
	UShort_t Energy;
	UShort_t Channel;
	UInt_t f;
	TArrayS *w = new TArrayS();

    ULong64_t time;
    int nbinMeas;

    if(useListData){
	    aChain->SetBranchAddress("Timestamp", &Timestamp);
	    //aChain->SetBranchAddress("Samples", &(w));
	    aChain->SetBranchAddress("Energy", &Energy);
	    aChain->SetBranchAddress("Flags",&f);
	    aChain->SetBranchAddress("Channel",&Channel);
	    nEvent = aChain->GetEntries();
	    histoBG_un = new TH1F("histoBG_un", "Uncalibrated histo bkg", 1400, 0., 14E3);   

	    ULong64_t timeMin =0;
	    ULong64_t timeMax=0;

	    for(int i=0; i<nEvent+1; i++)
	    {
	    	aChain->GetEntry(i);
	    	histoBG_un->Fill(Energy);

            //Determine minimum and maximum timestamps
            if(i==0) timeMin=Timestamp;
            if(Timestamp < timeMin) timeMin=Timestamp;
            if(Timestamp > timeMax) timeMax=Timestamp;
	    }
        ULong64_t time_check = timeMax - timeMin ;

        // time difference of first and last events in the tree
        aChain->GetEntry(nEvent);
        ULong64_t TMax = Timestamp ;
        aChain->GetEntry(0);
        ULong64_t Tmin = Timestamp ;
        time = TMax - Tmin ;

        std::cout << "Time in h " << (time*1E-12)/3600. << std::endl;
        std::cout << "Time check in h " << (time_check*1E-12)/3600.<< std::endl;
    }
    else{
        histoBG_un = (TH1F*)fMeas->Get("Energy/_R_EnergyCH1@V1724_1075");
        nEvent = histoBG_un->GetEntries();
        time = realtime;
        nbinMeas = histoBG_un->GetNbinsX();
        std::cout << "Time in h " << time/3600. << std::endl;
    }
    
    if(useListData) histoBG_un->Rebin(10);
    else histoBG_un->Rebin(3); // 16383 divisible by 3
    // Energy calibration pars
    double a = 0.56292;
    double b = -37.267;
    double y = 0.;
    
    //calibrate the histogram
    if(useListData){ 
        histoCal = new TH1F("calibrated_ug","Single calibrated underground unshielded",1400, 0., 14E3);
        for(int i=0; i<nEvent+1; i++)
        {
            aChain->GetEntry(i);
            y=0;
            if(Energy<10000)
            {
                y=a*Energy+b;
                histoCal->Fill(y);
            }
            else {
                histoCal->Fill(y);
            }
        }
    }
    else{
        // Take channel == energy
        histoCal = (TH1F*)histoBG_un->Clone();
    }
    //histoCal->Draw("hist");

    
    //Extend the background
    /*
    double nbins=histoCal->GetNbinsX();
    
    double xmin = 4000;
    double xmax = 5000;
    double lastbin=histoCal->GetNbinsX();
    double xmean = xmin + (xmax-xmin)/2;
    double binmin = histoCal->GetXaxis()->FindBin(xmin);
    double binmean = histoCal->GetXaxis()->FindBin(xmean);
    double binmax = histoCal->GetXaxis()->FindBin(xmax);
    
    double intercept = histoCal->GetBinContent(binmin);
    
    std::cout << binmin << " " << binmax << " " << intercept << std::endl;
    
    //linear fit on the background to retrieve the parameter
    TF1* linear_fit = new TF1("linear", "[0]+[1]*x", xmin, xmax);
    linear_fit->SetParameter(0,intercept);
    linear_fit->SetParameter(1,0.);
    
    linear_fit->Draw();
    
    histoCal->Fit("linear","WL","", xmin, xmax);
    
    TVirtualFitter *fit = TVirtualFitter::GetFitter();
    fit->PrintResults(2,0.);
    //histoBG->GetYaxis()->SetRangeUser(0.,100);
    //histoBG->GetXaxis()->SetRangeUser(xmin,xmax);
    histoCal->Draw("hist");
    
    double slope = fit->GetParameter(1);
    double q = fit->GetParameter(0);
    
    linear_fit->SetParameter(1,slope);
    linear_fit->SetParameter(0,q);
    linear_fit->Draw("l same");

    while (histoCal->GetBinContent(lastbin)==0 & nbins>0)
    {
        lastbin--;
    }
    
    TRandom3 randomGenerator;
    randomGenerator.SetSeed(0);
    
    TH1F* histoBG = (TH1F*) (histoCal->Clone());
    
    for(int j=lastbin; j<=nbins;j++)
    {
        double x = histoBG->GetXaxis()->GetBinCenter(j);
        double entry = (slope*x)+q;
        double sigma = sqrt(entry);
        
        double noise = randomGenerator.Poisson(entry);
        
        //entry=r.Poisson(entry);
        histoBG->SetBinContent(j,noise);
        std::cout << noise << std::endl;
    }
    
    histoBG->Draw("hist");
    
    */

    //c1->SetLogy();
    
    //float_t ratioSim = 1 /simHist->Integral() ;
    float_t ratioBG; 
    int runtime;
    if(useListData) {
        ratioBG = float(1./((time*1E-12)/3600.));
        std::cout << "ratioBG " << 1/((time*1E-12)/3600.) << std::endl;
        runtime = int((time*1E-12)/3600. + 0.5); // rounded up
    }
    else{ 
        ratioBG = float(1./(time/3600.));
        std::cout << "ratioBG " << 1/(time/3600.) << std::endl;
        runtime = int(time/3600. + 0.5); // rounded up
    }
    
    //simHist->Scale(ratioSim * scaleFact) ;
    
    //TH1F* scaledHist = dynamic_cast<TH1F*>(histoBG->Clone());
    scaledHist = (TH1F*)histoCal->Clone();
    std::cout << "Scaling histoCal by "<< float(ratioBG * scaleFact) << std::endl;
    scaledHist->Scale(float(ratioBG * scaleFact));
    //TH1F* TotHist = new TH1F("TotHist", "", 1400, 0., 14E3);
    //TotHist = (TH1F*) histoBG;
    scaledHist->SetLineColor(kBlue);
    //scaledHist->GetXaxis()->SetRange(0,6000);
    scaledHist->Draw("hist");
    
    /*
    histoBG->SetLineColor(kRed);
    histoBG->Draw("hist same");
    auto legend1 = new TLegend(0.1,0.7,0.48,0.9);
    legend1->SetHeader("Background for one scintillator with lead shielding");
    //legend->AddEntry(hist_sum,"Sum of the invidual energy deposits","l");
    legend1->AddEntry(histoCal,Form("%i hours backgorund run",runtime),"l");
    legend1->AddEntry(scaledHist,"120 hours backgorund run","l");
    legend1->Draw();
    c1->Update();
    */
    // Histogramma scalato per 1 solo rivelatore underground
    TFile* fout = new TFile("Bkg.root", "RECREATE");
    scaledHist->Write();
    
    ///////////////////////////
    //
    // Plot the simulated energy spectrum
    //
    ///////////////////////////

    // open the simulation file
    // per la simulazione a Ecm=706keV gli eventi sono calcolati per 5 giorni di misura
    
    // nel file con 0 finale ci sono 1.68M di eventi (5days), mentre nel file con 1 1.4
    // TFile* f1 = new TFile(" /Users/danielamercogliano/Desktop/tools/geant4/geant4-v10.7.4/examples/Shades_MC_Easy/buildNaI/test_6NaI_15cm_11329keV_newBR.root", "READ");

    TFile* f1 = new TFile("/mnt/c/Users/tchil/Documents/LNGS/EASY/SimEASY/SimEASY/build/test.root", "READ");
    TH1F* simHist = (TH1F*)f1->Get("Tot_energy");
    
    // energy resolution
    double resolution = 0.07; // TC: 7% at 662 keV ?
    double res_peak = 662; //MeV TC: keV ???

    double FWHM = resolution * res_peak;
    double sigma = FWHM/2.35;

    int nbin = scaledHist->GetNbinsX(); // Set binning of simulated hist to same as measured hist
    
    TH1F* simRes= new TH1F("Ene_res","Energy deposit with exp energy resol", nbin,  0., 16383);
   
    // Smear the simulated histogram
    for(int j=1;j<=nbin;j++)
    {
        double x1 = 1000*simHist->GetXaxis()->GetBinCenter(j); // Energy in keV
        double y1 = simHist->GetBinContent(j); // counts at that bin
        double sigma_red = sigma*TMath::Sqrt(x1/res_peak); // Linearly scale resolution: sigma(E) = sigma_662keV * sqrt(E/662keV)
        
        TF1* gauss = new TF1("gauss", "TMath::Gaus(x,[1],[2], true)", x1-(4*sigma_red), x1+(4*sigma_red)); // Gaus(x,mean,sigma,true) = normalised
        gauss->SetParameter(1, x1); // Gaus mean
        gauss->SetParameter(2, sigma_red); // Gaus sigma
        
        for(int i=0; i<y1; i++) simRes->Fill(gauss->GetRandom()); // smear the bin 
           
    }

    auto c2 = new TCanvas("c2","c2",900,600);
    c2->cd();
    c2->SetLogy();
    
    // Tot = signal + bkg
    if(useListData) TotHist = new TH1F("TotHist","",nbin, 0.,14E3);
    // Bkg for all 6 scintillators
    if(useListData) TotBkg = new TH1F("TotBkg","", nbin, 0.,14E3);
    TotBkg = (TH1F*)scaledHist->Clone();
    for(int i=1; i<6; i++) TotBkg->Add(scaledHist);
    
    double nbins = TotBkg->GetNbinsX();
    
    double xmin = 4000;
    double xmax = 5000;
    double lastbin=TotBkg->GetNbinsX();
    double xmean = xmin + (xmax-xmin)/2;
    double binmin = TotBkg->GetXaxis()->FindBin(xmin);
    double binmean = TotBkg->GetXaxis()->FindBin(xmean);
    double binmax = TotBkg->GetXaxis()->FindBin(xmax);
    
    double intercept = TotBkg->GetBinContent(binmin);
    
    std::cout << "binmin: " << binmin << ", binmax: " << binmax << ", initial intercept: " << intercept << std::endl;
    
    //linear fit on the background to retrieve the parameter
    TF1* linear_fit = new TF1("linear", "[0]+[1]*x", xmin, xmax);
    linear_fit->SetParameter(0,intercept);
    linear_fit->SetParameter(1,0.);
    
    linear_fit->Draw();
    
    //histoCal->Fit("linear","WL","", xmin, xmax);
    TotBkg->Fit("linear","WL","", xmin, xmax);
    std::cout << "Fitted intercept: " << linear_fit->GetParameter(0) << std::endl;
    
    TVirtualFitter *fit = TVirtualFitter::GetFitter();
    fit->PrintResults(2,0.);
    //histoBG->GetYaxis()->SetRangeUser(0.,100);
    //histoBG->GetXaxis()->SetRangeUser(xmin,xmax);
    TotBkg->Draw("hist");
    
    double slope = fit->GetParameter(1);
    double q = fit->GetParameter(0);
    
    linear_fit->SetParameter(1,slope);
    linear_fit->SetParameter(0,q);
    linear_fit->Draw("l same");

    // Find the last filled bin
    while (TotBkg->GetBinContent(lastbin)==0 & nbins>0)
    {
        lastbin--;
    }
    
    TRandom3 randomGenerator;
    randomGenerator.SetSeed(0);
    
    TOTBKG = (TH1F*)(TotBkg->Clone());
    
    for(int j=lastbin; j<=nbins;j++)
    {
        double x = TOTBKG->GetXaxis()->GetBinCenter(j);
        double entry = (slope*x)+q;
        double sigma = sqrt(entry);
        
        double noise = randomGenerator.Poisson(entry);
        
        //entry=r.Poisson(entry);
        TOTBKG->SetBinContent(j,noise);
        std::cout << "noise: " << noise << std::endl;
    }
    
    // Add the simulated spectrum to total measured background
    
    TotHist = (TH1F*) TOTBKG->Clone();
    TotHist->Add(simRes);
  
    //TotHist->SetLineStyle(0);
    
    if(useListData) TotHist->SetTitle("MC signal + experimental Background ;Energy [keV]; Counts per 10 keV");
    else TotHist->SetTitle("MC signal + experimental Background ;Energy [keV]; Counts per 3 keV");
    
    //TotHist->GetYaxis()->SetRangeUser(100,1E5);
    //TotHist->GetXaxis()->SetRangeUser(1000,6000);
    TotHist->SetLineColor(kBlack);
    TotHist->SetLineWidth(8);
    TotHist->SetFillColor(0);

    TOTBKG->SetLineColor(38);
    TOTBKG->SetLineWidth(3);
    TOTBKG->SetFillColor(0);
    TotHist->Draw("hist");
    TOTBKG->Draw("hist same");

    simRes->SetLineColor(2);
    simRes->SetLineWidth(3);
    simRes->SetFillColor(0);
    simRes->Draw("hist same");
    
    auto legend = new TLegend(0.5,0.7,0.9,0.9);
    //legend->SetHeader("shielded","C");
    legend->AddEntry(TotHist,"Simulated Signal + Experimental Background","l");
    legend->AddEntry(simRes,"Simulated Signal","l");
    legend->AddEntry(TOTBKG,"Experimental Background","l");
    legend->SetTextSize(0.02);
    legend->Draw();
    
    c2->Update();

    return 0;
}
