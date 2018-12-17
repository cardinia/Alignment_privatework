#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <experimental/filesystem>
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
#include "TRegexp.h"
#include "TPaveLabel.h"
#include "TPaveText.h"
#include "TStyle.h"
#include "TLine.h"

using namespace std;
namespace fs = std::experimental::filesystem;

/*!
 * \def Dummy value in case a DMR would fail for instance
 */
#define DUMMY -999.
/*!
 * \def Scale factor value to have mean and sigmas expressed in micrometers
 */
#define SFactor 1000.

/*! \struct Point
 *  \brief Structure Point
 *         Contains parameters of Gaussian fits to DMRs
 *  
 * @param run run number (IOV boundary)
 * @param mu mu/mean from Gaussian fit to DMR
 * @param sigma sigma/standard deviation from Gaussian fit to DMR
 * @param muplus mu/mean for the inward pointing modules
 * @param muminus mu/mean for outward pointing modules
 * @param sigmaplus sigma/standard for inward pointing modules 
 * @param sigmaminus; //!< sigma/standard for outward pointing modules
 */
struct Point {
    float run, mu, sigma, muplus, muminus, sigmaplus, sigmaminus; 

    /*! \fn Point
     *  \brief Constructor of structure Point, initialising all members one by one
     */
    Point (float Run = DUMMY, float y1 = DUMMY,float y2 = DUMMY, float y3 = DUMMY, float y4 = DUMMY, float y5 = DUMMY, float y6 = DUMMY) :
        run(Run), mu(y1), sigma(y2), muplus(y3), muminus(y5), sigmaplus(y4), sigmaminus(y6)
    {}

    /*! \fn Point
     *  \brief Constructor of structure Point, initialising all members from DMRs directly (with split)
     */
    Point (float Run, TH1 * histo, TH1 * histoplus, TH1 * histominus) :
        Point(Run, histo->GetMean(), histo->GetMeanError(),
                histoplus->GetMean(), histoplus->GetMeanError(),
                histominus->GetMean(), histominus->GetMeanError())
    {}

    /*! \fn Point
     *  \brief Constructor of structure Point, initialising all members from DMRs directly (without split)
     */
    Point (float Run, TH1 * histo) :
        Point(Run, histo->GetMean(), histo->GetMeanError())
    {}

    Point& operator=(const Point &p){ run = p.run; mu = p.mu;  muplus = p.muplus;  muminus = p.muminus; sigma = p.sigma;  sigmaplus = p.sigmaplus;  sigmaminus = p.sigmaminus; return *this;}

    float GetRun          () const { return run       ; } 
    float GetMu           () const { return SFactor*mu        ; } 
    float GetMuPlus       () const { return SFactor*muplus    ; } 
    float GetMuMinus      () const { return SFactor*muminus   ; } 
    float GetSigma        () const { return SFactor*sigma     ; } 
    float GetSigmaPlus    () const { return SFactor*sigmaplus ; } 
    float GetSigmaMinus   () const { return SFactor*sigmaminus; } 
    float GetDeltaMu      () const { if(muplus==DUMMY&&muminus==DUMMY) return DUMMY;
        else return SFactor*(muplus - muminus); }
        float GetSigmaDeltaMu () const { if(sigmaplus==DUMMY&&sigmaminus==DUMMY) return DUMMY;
            else return SFactor*hypot(sigmaplus,sigmaminus); };
};

/*! \function getName
 *  \brief Function used to get a string containing information on the high level structure, the layer/disc and the geometry.
 */

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


void scalebylumi(TGraphErrors *g, double min=0., string scalefile="/afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv"); 
double scalerunbylumi(int run, double min=0., string scalefile="/afs/cern.ch/work/h/hpeterse/public/lumiPerRun80.csv");
void PlotDMRTrends(string label="_v11", string type="MB", const vector<int> IOVfirstrun={315257, 315488, 315489, 315506, 315640, 315689, 315690, 315713, 315790, 315800, 315973, 316058, 316060, 316082, 316187, 316200, 316216, 316218, 316239, 316271, 316361, 316363, 316378, 316456, 316470, 316505, 316559, 316569, 316665, 316758, 317080, 317182, 317212, 317295, 317339, 317382, 317438, 317527, 317661, 318228, 318712, 319337, 319460, 320377, 320569, 320688, 320712, 320809, 320821, 320823, 320838, 320853, 320856, 320917, 320933, 320936, 320980, 321004, 321009, 321051, 321067, 321119}, vector<string> geometries={"MP pix LBL","PIX HLS+ML STR fix","GT","SG"}, vector<Color_t> colours={kGreen, kBlue, kBlack, kRed});

/*! \class Geometry
 *  \brief Class Geometry
 *         Contains vector for fit parameters (mean, sigma, etc.) obtained from multiple IOVs
 *         See Structure Point for description of the parameters.
 */

class Geometry {
    public:
        vector<Point> points;
    private:
        //template<typename T> vector<T> GetQuantity (T (Point::*getter)() const) const {
        vector<float> GetQuantity (float (Point::*getter)() const) const {
            vector<float> v;
            for (Point point: points) {
                float value = (point.*getter)();
                v.push_back(value);
            }
            return v;
        }
    public:
        TString title;
        Geometry() : title ("") {}
        Geometry(TString Title) : title(Title) {}
        Geometry& operator=(const Geometry &geom){ title = geom.title; points = geom.points; return *this;}
        void SetTitle(TString Title){ title = Title;}
        TString GetTitle() { return title; }
        vector<float> Run             () const { return GetQuantity( &Point::GetRun         ); }
        vector<float> Mu              () const { return GetQuantity( &Point::GetMu          ); }
        vector<float> MuPlus          () const { return GetQuantity( &Point::GetMuPlus      ); }
        vector<float> MuMinus         () const { return GetQuantity( &Point::GetMuMinus     ); }
        vector<float> Sigma           () const { return GetQuantity( &Point::GetSigma       ); }
        vector<float> SigmaPlus       () const { return GetQuantity( &Point::GetSigmaPlus   ); }
        vector<float> SigmaMinus      () const { return GetQuantity( &Point::GetSigmaMinus  ); }
        vector<float> DeltaMu         () const { return GetQuantity( &Point::GetDeltaMu     ); }
        vector<float> SigmaDeltaMu    () const { return GetQuantity( &Point::GetSigmaDeltaMu); }
        //vector<float> Graph (string variable) const {
        // };
};

/// DEPRECATED
//struct Layer {
//    map<string,Geometry> geometries;
//};
//
//struct HLS {
//    vector<Layer> layers;
//    map<string,Geometry> geometries;
//};


/*! \function DMRtrends()
 *  \brief    Function DMRtrends()
 *            Used to create a file where DMR trends are stored in the form of TGraph.
 */


void DMRtrends(string label="_v11", string myValidation="/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/results/acardini/DMRs/", vector<string> geometries={"MP pix LBL","PIX HLS+ML STR fix","GT","SG"}, string type="MB"){
    gROOT->SetBatch();
    vector<int>RunNumbers;
    vector<TString> filenames;
    TRegexp regexp("[0-9][0-9][0-9][0-9][0-9][0-9]");
    for (const auto & entry : fs::recursive_directory_iterator(myValidation)){
      if (entry.path().string().find("ExtendedOfflineValidation_Images/OfflineValidationSummary.root")!=std::string::npos){
	//std::cout << entry.path().string() << std::endl;
	if(fs::is_empty(entry.path())) cout << "ERROR: Empty file " << entry.path() << endl;
	else{
	  TString filename(entry.path().string());
	  filenames.push_back(filename);
	}
      }
    }

    const vector<int> IOVfirstrun={315257, 315488, 315489, 315506, 315640, 315689, 315690, 315713, 315790, 315800, 315973, 316058, 316060, 316082, 316187, 316200, 316216, 316218, 316239, 316271, 316361, 316363, 316378, 316456, 316470, 316505, 316559, 316569, 316665, 316758, 317080, 317182, 317212, 317295, 317339, 317382, 317438, 317527, 317661, 318228, 318712, 319337, 319460, 320377, 320569, 320688, 320712, 320809, 320821, 320823, 320838, 320853, 320856, 320917, 320933, 320936, 320980, 321004, 321009, 321051, 321067, 321119};


    vector<string> types {"MB", "SM"};

    vector<int> pixelupdateruns {314881, 316758, 317527, 318228, 320377};

    vector<TString> structures { "BPIX", "BPIX_y", "FPIX", "FPIX_y", "TIB", "TID", "TOB", "TEC"};

    const map<TString,int> nlayers{ {"BPIX", 4}, {"FPIX", 3}, {"TIB", 4}, {"TID", 3}, {"TOB", 6}, {"TEC", 9} };


    map<pair<pair<TString,int>,TString>,Geometry> mappoints; // pair = (structure, layer), geometry
    //Points defaultvalue;
    //Map[""]=defaultvalue;
    //string will be a combination of stucture, layer, and geometry
    for (TString filename: filenames){
        int runN;
        TString runstring(filename(regexp));
        if(runstring.IsFloat()){
	  runN=runstring.Atoi();
	  RunNumbers.push_back(runN);
	}
        else{
	  cout << "ERROR: run number not retrieved for file " << filename << endl;
	  continue;
	}
        //const char * filename = Form("fwhewe%deorbveurvev", 23);
        //TString filename="/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/results/acardini/DMRs/DMR_";	
        //filename+=type; filename+="_"; filename+=runN; filename+=label; filename+="/ExtendedOfflineValidation_Images/OfflineValidationSummary.root";

        TFile * f = new TFile(filename, "READ");

        for (TString& structure: structures) {
            TString structname = structure;
            structname.ReplaceAll("_y", "");
            size_t layersnumber=nlayers.at(structname);
            for (size_t layer=0; layer<=layersnumber;layer++){
                for (string geometry: geometries) {

                    TString name = getName(structure, layer, geometry);
                    TH1F *histo      = dynamic_cast<TH1F*>(f->Get( name));
                    //Geometry *geom =nullptr;
                    Point * point = nullptr;
                    if(!histo){
                        cout << "Run" << runN << " Histogram: " << name << " not found" << endl;
                        point= new Point(runN);
                        continue;
                    }
                    if(structure!="TID"&&structure!="TEC"){

                        TH1F *histoplus  = dynamic_cast<TH1F*>(f->Get((name+"_plus")));

                        TH1F *histominus = dynamic_cast<TH1F*>(f->Get((name+"_minus")));						  
                        if(!histoplus||!histominus){
                            cout << "Run" << runN << " Histogram: " << name << " plus or minus not found" << endl;
                            point= new Point(runN, histo);
                            continue;
                        }
                        point= new Point(runN, histo, histoplus, histominus);

                    }else point= new Point(runN, histo);
                    mappoints[make_pair(make_pair(structure,layer),geometry)].points.push_back(*point);
                }
            }
        }
        f->Close();
    }
    TString outname="/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/results/acardini/DMRs/DMRtrends_";
    outname+=type.c_str(); outname+=".root";	
    TFile * fout = TFile::Open(outname, "RECREATE");
    for (TString& structure: structures) {
        TString structname = structure;
        structname.ReplaceAll("_y", "");
        size_t layersnumber=nlayers.at(structname);
        for (size_t layer=0; layer<=layersnumber;layer++){
            for (string geometry : geometries) {
                TString name = getName(structure, layer, geometry);
                Geometry geom = mappoints[make_pair(make_pair(structure,layer),geometry)];
                using Trend = vector<float> (Geometry::*)() const;
                vector<Trend> trends {&Geometry::Mu, &Geometry::Sigma, &Geometry::MuPlus, &Geometry::SigmaPlus, 
                    &Geometry::MuMinus, &Geometry::SigmaMinus, &Geometry::DeltaMu, &Geometry::SigmaDeltaMu };
                vector<TString> variables {"mu", "sigma", "muplus", "sigmaplus", "muminus", "sigmaminus", "deltamu", "sigmadeltamu" };
                vector<float> runs = geom.Run();
                size_t n = runs.size();
                for(size_t iVar=0; iVar < variables.size(); iVar++){
                    Trend trend=trends.at(iVar);
                    TGraph *g = new TGraph(n, runs.data(), (geom.*trend)().data());
                    g->SetTitle(geometry.c_str());
                    g->Write(name+"_"+variables.at(iVar));
                }
                vector<pair<Trend,Trend>> trendspair {make_pair(&Geometry::Mu, &Geometry::Sigma), make_pair(&Geometry::MuPlus, &Geometry::SigmaPlus), 
                    make_pair(&Geometry::MuMinus, &Geometry::SigmaMinus), make_pair(&Geometry::DeltaMu, &Geometry::SigmaDeltaMu) };
                vector<pair<TString,TString>> variablepairs {make_pair("mu", "sigma"), make_pair("muplus", "sigmaplus"), make_pair("muminus", "sigmaminus"), make_pair("deltamu", "sigmadeltamu") };
                vector<float> emptyvec;
                for(size_t i=0; i < n; i++)emptyvec.push_back(0.);
                for(size_t iVar=0; iVar < variablepairs.size(); iVar++){
                    Trend meantrend=trendspair.at(iVar).first;
                    Trend sigmatrend=trendspair.at(iVar).second;
                    TGraphErrors *g = new TGraphErrors(n, runs.data(), (geom.*meantrend)().data(), emptyvec.data(), (geom.*sigmatrend)().data());
                    g->SetTitle(geometry.c_str());
                    TString graphname = name+"_"+variablepairs.at(iVar).first;
                    graphname+=variablepairs.at(iVar).second;
                    g->Write(graphname);
                }
            }
        }
    }
    fout->Close();

    PlotDMRTrends(label, type, RunNumbers, geometries);
}



double scalerunbylumi(int run, double min, string scalefile){
    int unitscale=pow(10,3);


    TGraph * scale = new TGraph(scalefile.c_str());
    int Nscale=scale->GetN();
    double *xscale=scale->GetX();
    double *yscale=scale->GetY();


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
    for(int j=0;j<Nscale;j++){
        lumi+=yscale[j]/unitscale;

    }
    cout << "total lumi: " << lumi << endl;
    g->GetHistogram()->Delete(); 
    g->SetHistogram(0); 
}
/*! \function PlotDMRTrends()
 *  \brief    Function PlotDMRTrends()
 *            Used to plot the DMR trends.
 */

void PlotDMRTrends(string label, string type, const vector<int> IOVfirstrun, vector<string> geometries, vector<Color_t> colours){

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
            vector<TString> variables {"mu", "sigma", "muplus", "sigmaplus", "muminus", "sigmaminus", "deltamu", "sigmadeltamu", "musigma", "muplussigmaplus", "muminussigmaminus", "deltamusigmadeltamu"};
            vector<string> YaxisNames { "#mu [#mum]", "#sigma_{#mu} [#mum]", "#mu outward [#mum]", "#sigma_{#mu outward} [#mum]", "#mu inward [#mum]", "#sigma_{#mu inward} [#mum]", "#Delta#mu [#mum]", "#sigma_{#Delta#mu} [#mum]", "#mu [#mum]", "#mu outward [#mum]", "#mu inward [#mum]", "#Delta#mu [#mum]",}; 
            for(size_t i=0; i < variables.size(); i++){
                TString variable= variables.at(i);
                double max=-999;
                double min=+999;
                TCanvas * c = new TCanvas;
                vector<Color_t>::iterator colour = colours.begin();

                TMultiGraph *mg = new TMultiGraph(structure,structure);

                for (string geometry: geometries) {
                    TString name = getName(structure, layer, geometry);
                    TGraphErrors *g = (TGraphErrors*) in->Get(name+"_"+variables.at(i));
                    /*if (i >=8) {
                      g->Delete();
                      TGraphErrors *g = (TGraphErrors*) in->Get(name+"_"+variables.at(i));
                      }*/ 
                    g->SetLineColor(*colour);
                    g->SetMarkerColor(*colour);
                    g->SetMarkerStyle(20);
                    if(i>=8){
                        g->SetLineWidth(2);
                        g->SetFillColor(*colour);
                        g->SetFillStyle(3350+i-8);
                    }
                    if(i<8) mg->Add(g,"PL");
                    else mg->Add(g,"3L");
                    if(g->GetHistogram()->GetMaximum() > max) max = g->GetHistogram()->GetMaximum();
                    if(g->GetHistogram()->GetMinimum() < min) min = g->GetHistogram()->GetMinimum();
                    ++colour;

                }
                if(i<8) mg->Draw("a");
                else mg->Draw("a3");
                max=0.4;
                min=-0.4;
                double range=max-min;
                if(((variable=="sigma"||variable=="sigmaplus"||variable=="sigmaminus"||variable=="sigmadeltamu")&&range>=2)){
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
                //cout << "pad max " << gPad->GetUymax() << " pad min " << gPad->GetUymin() << endl;
                //cout << "graph max " << max << " graph min " << min << endl;
                legend->Draw();
                c->Update();
                TString structandlayer = getName(structure,layer,"");
		///TO DO: select output directory for the printing the DMR trends
                TString printfile="/afs/cern.ch/cms/CAF/CMSALCA/ALCA_TRACKERALIGN/data/commonValidation/alignmentObjects/acardini/DMRsTrends/V11-"+type+"_"+variable+structandlayer;
                c->SaveAs(printfile+".pdf");
                c->SaveAs(printfile+".eps");
                c->SaveAs(printfile+".png");

            }

        }

    }
    in->Close();
}

int main () { DMRtrends(); return 0; }
