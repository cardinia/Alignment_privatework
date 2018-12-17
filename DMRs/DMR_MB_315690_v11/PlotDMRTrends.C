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
#include "TFile.h"
#include "TColor.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMath.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TLine.h"

using namespace std;


TString getName (TString structure, int layer, TString geometry){
    geometry.ReplaceAll(" ","_");
    TString name=geometry+"_"+structure;
    if(layer!=0){
        if(structure=="TID"||structure=="TEC")name+="_disc";
        else name+="_layer";
        name+=layer;
    }

    return name;
};


void PlotDMRTrends(string label="_v11", const vector<int> IOVfirstrun={315257, 315488, 315489, 315506, 315640, 315689, 315690, 315713, 315790, 315800, 315973, 316058, 316060, 316082, 316187, 316200, 316216, 316218, 316239, 316271, 316361, 316363, 316378, 316456, 316470, 316505, 316559, 316569, 316665, 316758, 317080, 317182, 317212, 317295, 317339, 317382, 317438, 317527, 317661, 318228, 318712, 319337, 319460, 320377, 320569, 320688, 320712, 320809, 320821, 320823, 320838, 320853, 320856, 320917, 320933, 320936, 320980, 321004, 321009, 321051, 321067, 321119}, vector<string> geometries={"MP pix LBL","PIX HLS+ML STR fix","GT","SG"}, vector<Color_t> colours={kGreen, kBlue, kBlack, kRed}, string type="MB"){

    vector<int> pixelupdateruns {314881, 316758, 317527, 318228, 320377};

    vector<TString> structures { "BPIX", "BPIX_y", "FPIX", "FPIX_y", "TIB", "TID", "TOB", "TEC"};

    const map<TString,int> nlayers{ {"BPIX", 4}, {"FPIX", 3}, {"TIB", 4}, {"TID", 3}, {"TOB", 6}, {"TEC", 9} };


    //	vector<string> geometries {"GT", "PIXHLS+MLSTRfix"};
    //	vector<Color_t> colours { kBlack, kRed}; 
    TString filename="/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/results/acardini/DMRs/DMRtrends_"+type+".root";
    TFile *in= new TFile(filename);
    for (TString& structure: structures) {
        TString structname = structure;
        structname.ReplaceAll("_y", "");
        int layersnumber=nlayers.at(structname);
        for (int layer=0; layer<=layersnumber;layer++){
            cout << "HI"<<endl;
            vector<TString> variables {"mu", "sigma", "muplus", "sigmaplus", "muminus", "sigmaminus", "deltamu", "sigmadeltamu", "musigma", "muplussigmaplus", "muminussigmaminus", "deltamusigmadeltamu"};
            vector<string> YaxisNames { "#mu [#mum]", "#sigma_{#mu} [#mum]", "#mu outward [#mum]", "#sigma_{#mu outward} [#mum]", "#mu inward [#mum]", "#sigma_{#mu inward} [#mum]", "#Delta#mu [#mum]", "#sigma_{#Delta#mu} [#mum]", "#mu [#mum]", "#mu outward [#mum]", "#mu inward [#mum]", "#Delta#mu [#mum]",}; 
            for(int i=0; i < variables.size(); i++){
                TString variable= variables.at(i);
                cout<< i << "  " << variables.at(i) <<endl;
                double max=-999;
                double min=+999;
                TCanvas * c = new TCanvas;
                bool first = true;
                vector<Color_t>::iterator colour = colours.begin();

                TMultiGraph *mg = new TMultiGraph(structure,structure);

                for (string geometry: geometries) {
                    TString name = getName(structure, layer, geometry);
                    cout << name << endl;
                    TGraphErrors *g = (TGraphErrors*) in->Get(name+"_"+variables.at(i));
                    /*if (i >=8) {
                      g->Delete();
                      TGraphErrors *g = (TGraphErrors*) in->Get(name+"_"+variables.at(i));
                      }*/ 
                    cout << g << endl;
                    cout << i << " " << (*colour) <<endl;
                    g->SetLineColor(*colour);
                    cout<< "HI" <<endl;
                    g->SetMarkerColor(*colour);
                    g->SetMarkerStyle(20);
                    if(i>=8){
                        g->SetLineWidth(2);
                        g->SetFillColor(*colour);
                        g->SetFillStyle(3350+i-8);
                    }
                    if(i<8) mg->Add(g,"PL");
                    else mg->Add(g,"3L");
                    cout << g->GetHistogram()->GetMaximum()<< endl;
                    cout << g->GetHistogram()->GetMinimum()<< endl;
                    if(g->GetHistogram()->GetMaximum() > max) max = g->GetHistogram()->GetMaximum();
                    if(g->GetHistogram()->GetMinimum() < min) min = g->GetHistogram()->GetMinimum();
                    ++colour;

                }
                if(i<8) mg->Draw("a");
                else mg->Draw("a3");
                max=0.4;
                min=-0.4;
                double range=max-min;
                if((variable=="sigma"||variable=="sigmaplus"||variable=="sigmaminus"||variable=="sigmadeltamu"&&range>=2)){
                    mg->SetMaximum(0.5);
                    mg->SetMinimum(-0.5);
                }else{
                    mg->SetMaximum(max+range*0.1);
                    mg->SetMinimum(min-range*0.3);
                }
                char* Ytitle= (char *)YaxisNames.at(i).c_str();
                mg->GetYaxis()->SetTitle(Ytitle);

                mg->GetXaxis()->SetTitle("IOV number");
                mg->GetYaxis()->SetTitleOffset(.8);
                mg->GetYaxis()->SetTitleSize(.05);
                mg->GetXaxis()->SetTitleSize(.04);
                gStyle->SetOptTitle(0); // TODO
                c->SetLeftMargin(0.11);
                char* typetitle=(char *)"";
                if(type=="MB")typetitle=(char *)"Minimum Bias";
                if(type=="SM")typetitle=(char *)"Single Muon";

                c->Update();


                TLegend *legend = c->BuildLegend();
                legend->SetHeader(typetitle);
                TLegendEntry *header = (TLegendEntry*)legend->GetListOfPrimitives()->First();
                header->SetTextSize(.04);
                legend->SetNColumns(2);
                //legend->SetTextSize(0.05);
                TString structtitle = "structure: " +structure;
                if(layer!=0){
                    if(structure=="TID"||structure=="TEC")structtitle+="_disc";
                    else structtitle+="_layer";
                    structtitle+=layer;
                }
                legend->AddEntry((TObject*)0,structtitle.Data(),"h");
                TLegendEntry *str = (TLegendEntry*)legend->GetListOfPrimitives()->Last();
                str->SetTextSize(.03);
                cout << "pad max " << gPad->GetUymax() << " pad min " << gPad->GetUymin() << endl;
                cout << "graph max " << max << " graph min " << min << endl;
                legend->Draw();
                c->Update();
                TString structandlayer = getName(structure,layer,"");
                TString printfile="/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/alignmentObjects/acardini/DMRsTrends/V11-"+type+"_"+variable+structandlayer;
                c->SaveAs(printfile+".pdf");
                c->SaveAs(printfile+".eps");
                c->SaveAs(printfile+".png");

            }

        }

    }
    in->Close();
}
/*
   int i=0;
   double max=-999;
   double min=+999;
   string canvasname = structure;
   TCanvas * c = new TCanvas;
   bool first = true;
   vector<Color_t>::iterator colour = colours.begin();
   TMultiGraph *mg = new TMultiGraph(structure.c_str(),structure.c_str());
   string filename = "MuDMR_"+type+"/muandsigma_"+type+"_" + structure + "_" + geometry + ".dat";
   cout << filename << '\t' << *colour << endl;
   TGraphErrors * g = new TGraphErrors(filename.c_str(),"%lg %lg %lg");
   if(g==NULL)continue;
   g->SetTitle(geometry.c_str());
   if(geometry=="PIXHLS+MLSTRfix")g->SetTitle("ReReco");
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
string typetitle=(string)"";
if(type=="MB")typetitle="Minimum Bias";
if(type=="SM")typetitle="Single Muon";

TLegend *legend = c->BuildLegend();
legend->SetHeader(typetitle.c_str());
TLegendEntry *header = (TLegendEntry*)legend->GetListOfPrimitives()->First();
header->SetTextSize(.04);
legend->SetNColumns(2);
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
string pngfile="V11-"+type+"_musigma_"+structure+ ".png";
string pdffile="V11-"+type+"_musigma_"+structure+ ".pdf";
string epsfile="V11-"+type+"_musigma_"+structure+ ".eps";
c->Print(pdffile.c_str());
c->Print(pngfile.c_str());
c->Print(epsfile.c_str());
*/



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
