#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include "TPad.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TMultiGraph.h"
#include "TH1.h"
#include "TROOT.h"
#include "TColor.h"
#include "TMath.h"
#include "TPaveLabel.h"
#include "TStyle.h"


void DMu_plotter(string type="MB"){
	gROOT->SetBatch();
	vector<string> structures { "BPIX", "FPIX", "BPIX_y", "FPIX_y", "TIB", "TOB" };
	vector<string> variables { "mu", "dmu", "sigma", "sigma_mu" };
	vector<string> YaxisNames { "#mu", "#Delta#mu", "#sigma", "#sigma_{#mu}" };
	vector<string> geometries {"GT" ,"SG" ,"MPpixLBL" ,"MPpixLBLstr" ,"MP2pixLBLHGIOV" ,"MP2pixHMSHGIOV" ,"MP2pixHMSstrHGIOV" ,"HipPypix"};
	vector<Color_t> colours { kBlack, kRed, kBlue, kGreen+2, kOrange, kCyan+2, kYellow+2, kViolet}; 
	int i=0;
	for (string variable: variables) {
	  for (string structure: structures) {
	        double max=-999;
		double min=+999;
		string canvasname = structure;
		TCanvas * c = new TCanvas;
		bool first = true;
		vector<Color_t>::iterator colour = colours.begin();
		TMultiGraph *mg = new TMultiGraph(structure.c_str(),structure.c_str());
		for (string geometry: geometries) {
  			string filename = "dMuDMR_"+type+"/"+variable+"_"+type+"_" + structure + "_" + geometry + ".dat";
 			cout << filename << '\t' << *colour << endl;
			TGraph * g = new TGraph(filename.c_str());
			if(g==NULL)continue;
			g->SetTitle(geometry.c_str());
			g->SetLineColor(*colour);
			g->SetMarkerColor(*colour);
			g->SetMarkerStyle(20);
			//g->Draw(first ? "APL" : "PLsame");
			mg->Add(g,"PL");
			first = false;
			cout << g->GetHistogram()->GetMaximum()<< endl;
			cout << g->GetHistogram()->GetMinimum()<< endl;
			if(g->GetHistogram()->GetMaximum() > max) max = g->GetHistogram()->GetMaximum();
			if(g->GetHistogram()->GetMinimum() < min) min = g->GetHistogram()->GetMinimum();
			++colour;
		}
		mg->Draw("a");
		double range=max-min;

		mg->SetMaximum(max+range*0.1);
		mg->SetMinimum(min-range*0.3);
		char* Ytitle= (char *)YaxisNames.at(i).c_str();
		mg->GetYaxis()->SetTitle(Ytitle);

		mg->GetXaxis()->SetTitle("run number");
		mg->GetYaxis()->SetTitleOffset(.8);
		mg->GetYaxis()->SetTitleSize(.05);
		mg->GetXaxis()->SetTitleSize(.04);
		gStyle->SetOptTitle(0); // TODO
		c->SetLeftMargin(0.11);
		c->BuildLegend();
		cout << "pad max " << gPad->GetUymax() << " pad min " << gPad->GetUymin() << endl;
		cout << "graph max " << max << " graph min " << min << endl;
		char* typetitle=(char *)"";
		if(type=="MB")typetitle=(char *)"Minimum Bias";
		if(type=="SM")typetitle=(char *)"Single Muon";
		TPaveLabel *title = new TPaveLabel(.11,.91,.89,0.99,typetitle,"brNDC");
		c->cd();
		title->Draw();
		c->Update();
		string pngfile=type+"_"+variable+"_"+structure+ ".png";
		string pdffile=type+"_"+variable+"_"+structure+ ".pdf";
		string epsfile=type+"_"+variable+"_"+structure+ ".eps";
		c->Print(pdffile.c_str());
		c->Print(pngfile.c_str());
		c->Print(epsfile.c_str());
	  }
	  ++i;

	}
}
