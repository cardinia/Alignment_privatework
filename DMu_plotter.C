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
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TPaveLabel.h"
#include "TStyle.h"

void scalebylumi(TGraph *g, double min=0., string scalefile="/afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv"); 


void DMu_plotter(string type="MB"){
	gROOT->SetBatch();
	vector<string> structures { "BPIX", "FPIX", "BPIX_y", "FPIX_y", "TIB", "TOB" };
	vector<string> variables { "mu", "dmu", "sigma_mu" };
	vector<string> YaxisNames { "#mu [#mum]", "#Delta#mu [#mum]", "#sigma_{#mu} [#mum]" };
	//v7
	//vector<string> geometries {"GT", "SG", "MPpixLBLstr", "HipPypix", "MP2pixHMSstrFIX", "MP2pixMLstrFIX", "MP2pixHMSnoFPIXzstrFIX", "MP2pixMLnoFPIXzatHLstrFIX", "MP2pixMLnoFPIXzstrFIX"};
	//vector<Color_t> colours { kBlack, kRed, kGreen+2, kViolet, kBlue, kOrange, kCyan+2, kYellow+2, kSpring}; 
	
	//v8
	//vector<string> geometries {"GT", "SG", "fullMLwoSD", "fullMLwSD"};
	//vector<Color_t> colours { kBlack, kRed, kBlue, kViolet}; 
	
	//v9
	vector<string> geometries {"GT", "SG", "weight10xZmumu+cosmics", "weight5xZmumu+cosmics", "weight20xZmumu+cosmics"};
	vector<Color_t> colours { kBlack, kRed, kViolet, kOrange}; 
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

		mg->SetMaximum(max+range*0.1);
		mg->SetMinimum(min-range*0.3);
		if(variable=="sigma_mu"&&range>=3){
		mg->SetMaximum(2);
		mg->SetMinimum(-1);
		}else{
		mg->SetMinimum(min-range*0.5);
		}
		char* Ytitle= (char *)YaxisNames.at(i).c_str();
		mg->GetYaxis()->SetTitle(Ytitle);

		mg->GetXaxis()->SetTitle("processed luminosity [1/fb]");
		mg->GetYaxis()->SetTitleOffset(.8);
		mg->GetYaxis()->SetTitleSize(.05);
		mg->GetXaxis()->SetTitleSize(.04);
		gStyle->SetOptTitle(0); // TODO
		c->SetLeftMargin(0.11);
		char* typetitle=(char *)"";
		if(type=="MB")typetitle=(char *)"Minimum Bias";
		if(type=="SM")typetitle=(char *)"Single Muon";

		TLegend *legend = c->BuildLegend();
		legend->SetHeader(typetitle);
		TLegendEntry *header = (TLegendEntry*)legend->GetListOfPrimitives()->First();
		header->SetTextSize(.04);
		legend->SetNColumns(4);
		//legend->SetTextSize(0.05);
		string structtitle="structure: "+structure;
		legend->AddEntry((TObject*)0,structtitle.c_str(),"h");
		TLegendEntry *str = (TLegendEntry*)legend->GetListOfPrimitives()->Last();
		str->SetTextSize(.03);
     		cout << "pad max " << gPad->GetUymax() << " pad min " << gPad->GetUymin() << endl;
		cout << "graph max " << max << " graph min " << min << endl;
		c->Update();
		string pngfile="V9-"+type+"_"+variable+"_"+structure+ ".png";
		string pdffile="V9-"+type+"_"+variable+"_"+structure+ ".pdf";
		string epsfile="V9-"+type+"_"+variable+"_"+structure+ ".eps";
		c->Print(pdffile.c_str());
		c->Print(pngfile.c_str());
		c->Print(epsfile.c_str());
	  }
	  ++i;

	}
}

void scalebylumi(TGraph *g, double min, string scalefile){ 
  int N=g->GetN();
  double *x=g->GetX();
  int unitscale=pow(10,3);


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
