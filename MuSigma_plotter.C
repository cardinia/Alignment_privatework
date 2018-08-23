#include <iostream>
#include <vector>
#include <map>
#include <iomanip>
#include "TPad.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TH1.h"
#include "TROOT.h"
#include "TColor.h"
#include "TMath.h"
#include "TPaveLabel.h"
#include "TStyle.h"

void scalebylumi(TGraphErrors *g, double min=0., string scalefile="/afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv"); 
//old /afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv
//new /afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv
void MuSigma_plotter(string type="MB"){
	gROOT->SetBatch();
	vector<string> structures { "BPIX", "FPIX", "BPIX_y", "FPIX_y", "TIB", "TOB" };
	vector<string> geometries {"GT" ,"SG" ,"MPpixLBL" ,"MPpixLBLstr" ,"MP2pixLBLHGIOV" ,"MP2pixHMSHGIOV" ,"MP2pixHMSstrHGIOV" ,"HipPypix"};
	vector<Color_t> colours { kBlack, kRed, kBlue, kGreen+2, kOrange, kCyan+2, kYellow+2, kViolet}; 
	int i=0;
	for (string structure: structures) {
	        double max=-999;
		double min=+999;
		string canvasname = structure;
		TCanvas * c = new TCanvas;
		bool first = true;
		vector<Color_t>::iterator colour = colours.begin();
		TMultiGraph *mg = new TMultiGraph(structure.c_str(),structure.c_str());
		for (string geometry: geometries) {
  			string filename = "MuDMR_"+type+"/muandsigma_"+type+"_" + structure + "_" + geometry + ".dat";
 			cout << filename << '\t' << *colour << endl;
			TGraphErrors * g = new TGraphErrors(filename.c_str(),"%lg %lg %lg");
			if(g==NULL)continue;
			g->SetTitle(geometry.c_str());
			g->SetLineColor(*colour);
			g->SetMarkerColor(*colour);
			g->SetMarkerStyle(20);
			scalebylumi(g);
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

		//mg->SetMaximum(max+range*0.1);
		//mg->SetMinimum(min-range*0.3);
		mg->SetMaximum(4);
		mg->SetMinimum(-4);
		mg->GetYaxis()->SetTitle("#mu");

		mg->GetXaxis()->SetTitle("processed luminosity [fb^{-1}]");
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
		string pngfile=type+"_musigma_"+structure+ ".png";
		string pdffile=type+"_musigma_"+structure+ ".pdf";
		string epsfile=type+"_musigma_"+structure+ ".eps";
		c->Print(pdffile.c_str());
		c->Print(pngfile.c_str());
		c->Print(epsfile.c_str());
	}

	
}

void scalebylumi(TGraphErrors *g, double min, string scalefile){ 
  int N=g->GetN();
  double *x=g->GetX();
  int unitscale=pow(10,9);


  TGraph * scale = new TGraph(scalefile.c_str());
  int Nscale=scale->GetN();
  double *xscale=scale->GetX();
  double *yscale=scale->GetY();


  int i=0;
  while(i<N){
    double lumi=min;
    int index=-1;
    for(int j=0;j<Nscale;j++){
      lumi+=yscale[j];
      if(x[i]>=xscale[j]){
	index=j;
	continue;
      }
    }
    if(yscale[index]==0){
      N=N-1;
      g->RemovePoint(i);
    }else{
      x[i]=min;
      for(int j=0;j<=index;j++)x[i]+=yscale[j]/unitscale;
      //x[i]=lumi/unitscale;
      i=i+1;
    }
    
  } 
  double lumi=min;
  int index=-1;
  for(int j=0;j<Nscale;j++){
    lumi+=yscale[j]/unitscale;
    
  }
  cout << "total lumi: " << lumi << endl;
  g->GetHistogram()->Delete(); 
  g->SetHistogram(0); 
}
