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
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TPaveLabel.h"
#include "TStyle.h"

void scalebylumi(TGraphErrors *g, double min=0., string scalefile="/afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv"); 
//old /afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv
//new /afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv
double scalerunbylumi(int run, double min=0., string scalefile="/afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv");


void MuSigma_plotter(string type="MB"){
	gROOT->SetBatch();

	vector<int> pixelupdateruns {314881, 316758, 317527, 318228, 320377};

	vector<string> structures { "BPIX", "FPIX", "BPIX_y", "FPIX_y", "TIB", "TOB" };
	//v7
	//vector<string> geometries {"GT", "SG", "MPpixLBLstr", "HipPypix", "MP2pixHMSstrFIX", "MP2pixMLstrFIX", "MP2pixHMSnoFPIXzstrFIX", "MP2pixMLnoFPIXzatHLstrFIX", "MP2pixMLnoFPIXzstrFIX"};
	//vector<Color_t> colours { kBlack, kRed, kGreen+2, kViolet, kBlue, kOrange, kCyan+2, kYellow+2, kSpring};
	//v8 
	//vector<string> geometries {"GT", "SG", "fullMLwoSD", "fullMLwSD"};
	//vector<Color_t> colours { kBlack, kRed, kBlue, kViolet}; 
	//v9
	vector<string> geometries {"GT", "SG", "weight10xZmumu+cosmics", "weight5xZmumu+cosmics", "weight20xZmumu+cosmics"};
	vector<Color_t> colours { kBlack, kRed, kBlue, kViolet, kOrange}; 
	for (string structure: structures) {
	  	int i=0;
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
			g->SetFillColor(*colour);
			g->SetFillStyle(3350+i);
			g->SetMarkerColor(*colour);
			g->SetMarkerStyle(20);
			scalebylumi(g);
			//g->Draw(first ? "A3L" : "3Lsame");
			mg->Add(g,"3L");
			first = false;
			cout << g->GetHistogram()->GetMaximum()<< endl;
			cout << g->GetHistogram()->GetMinimum()<< endl;
			if(g->GetHistogram()->GetMaximum() > max) max = g->GetHistogram()->GetMaximum();
			if(g->GetHistogram()->GetMinimum() < min) min = g->GetHistogram()->GetMinimum();
			++colour;
			++i;
		}
		mg->Draw("a3");
		double range=max-min;

		//mg->SetMaximum(max+range*0.1);
		//mg->SetMinimum(min-range*0.3);
		mg->SetMaximum(2.1);
		mg->SetMinimum(-3.1);
		mg->GetYaxis()->SetTitle("#mu [#mum]");

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
		legend->SetNColumns(5);
		//legend->SetTextSize(0.05);
		string structtitle="structure: "+structure;
		legend->AddEntry((TObject*)0,structtitle.c_str(),"h");
		TLegendEntry *str = (TLegendEntry*)legend->GetListOfPrimitives()->Last();
		str->SetTextSize(.03);
		cout << "pad max " << gPad->GetUymax() << " pad min " << gPad->GetUymin() << endl;
		cout << "graph max " << max << " graph min " << min << endl;

		int Nupdates = pixelupdateruns.size();
		double lastlumi=0.;
		vector<TPaveText*> labels;
		for(int iline=0; iline<Nupdates;iline++){
		  double lumi=0.;
		  int run=pixelupdateruns.at(iline);
		  lumi=scalerunbylumi(pixelupdateruns.at(iline));
		  TLine *line = new TLine (lumi,c->GetUymin(),lumi,c->GetUymax());
		  line->SetLineColor(kRed);
		  line->SetLineWidth(2);
		  line->Draw();

		  int ix1;
		  int ix2;
		  int iw = gPad->GetWw();
		  int ih = gPad->GetWh();
		  double x1p,y1p,x2p,y2p;
		  gPad->GetPadPar(x1p,y1p,x2p,y2p);

		  ix1 = (Int_t)(iw*x1p);
		  ix2 = (Int_t)(iw*x2p);
		  double wndc  = TMath::Min(1.,(double)iw/(double)ih);
		  double rw    = wndc/(double)iw;
		  double x1ndc = (double)ix1*rw;
		  double x2ndc = (double)ix2*rw;
		  double rx1,ry1,rx2,ry2;
		  gPad->GetRange(rx1,ry1,rx2,ry2);
		  double rx = (x2ndc-x1ndc)/(rx2-rx1);
		  double _sx;
		  _sx = rx*(lumi-rx1)+x1ndc;
		  bool tooclose = false;
		  if((lumi-lastlumi)<5&&iline!=0)tooclose=true;
		  TPaveText *box= new TPaveText(_sx+0.001,0.85-tooclose*0.05,_sx+0.08,0.89-tooclose*0.05,"blNDC");
		  TText *textRun = box->AddText(Form("%i",int(pixelupdateruns.at(iline))));
		  textRun->SetTextSize(0.025);
		  labels.push_back(box);
		  lastlumi=lumi;
		}
		for(int iline=0; iline<Nupdates;iline++){
		  labels.at(iline)->Draw("same");
		}
		legend->Draw();

		//TPaveLabel *title = new TPaveLabel(.11,.91,.89,0.99,typetitle,"brNDC");
		//c->cd();
		//title->Draw();
		c->Update();
		string pngfile="V9-"+type+"_musigma_"+structure+ ".png";
		string pdffile="V9-"+type+"_musigma_"+structure+ ".pdf";
		string epsfile="V9-"+type+"_musigma_"+structure+ ".eps";
		c->Print(pdffile.c_str());
		c->Print(pngfile.c_str());
		c->Print(epsfile.c_str());
	}

	
}
double scalerunbylumi(int run, double min, string scalefile){
  int unitscale=pow(10,3);


  TGraph * scale = new TGraph(scalefile.c_str());
  int Nscale=scale->GetN();
  double *xscale=scale->GetX();
  double *yscale=scale->GetY();


  int i=0;
  double lumi=min;
  int index=-1;
  for(int j=0;j<Nscale;j++){
    lumi+=yscale[j];
    if(run>=xscale[j]){
      index=j;
      continue;
    }
  }
  lumi=min;  
  for(int j=0;j<=index;j++)lumi+=yscale[j]/unitscale;
   
 
  return lumi;

}

void scalebylumi(TGraphErrors *g, double min, string scalefile){ 
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
