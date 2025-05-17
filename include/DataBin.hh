//DataBin.hh

#ifndef DataBin_h
#define DataBin_h 1


#include "globals.hh"
#include "TH1.h"

class DataBin
    {
    private:
		TH1F* hist;
        
    public:
        DataBin(){
			hist = new TH1F("spec", "spec", 8192, 0, 2000);
		};
        ~DataBin();
		
        TH1F* getHist(){ return hist;};
    };
#endif
