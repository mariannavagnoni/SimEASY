
//codice prova per leggere i livelli del 26Mg


/*
 1) include the header files
 2) create an object of the ifstream class
 3) open the file using the open method
 4) loop through the file using the getline() method and read each line into a string variable
 5)Close the file using the close() method once the end of the file is reached
 */


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "readlevels.hh"

using namespace std;


int readlevels(G4String filename, levelvec* levels, DataBin* databin){
    
    
    //ifstream file("/Users/danielamercogliano/Desktop/tools/geant4/geant4-v10.7.4/examples/Shades_MC_Easy/26Mg_v1.txt");
    ifstream file(filename.c_str());
    
    if(!file) {
        
        std::cout << "The file does not exist"<<std::endl;
        
        return 1;
    }
    
    
    if (!file.is_open()) {
        std::cerr << "Failed to open the file." << std::endl;
        return 2;
    }
    
    //vector for branching ratios
    std::vector<double> br;
    //vector for daughter energy
    std::vector<double> daughter;
    //vector for daughter energy
    std::vector<double> gamma;
    
    std::string line, firstWord, secondWord, word;
    G4double energy;
    
    if(file.is_open()){
        
        
        while(getline(file, line)){
            //skip empty lines
            if (line.empty()){
                
                continue;
                
            }
            
            //Process the first line (State Energy)
            std::stringstream ss1(line);
            ss1 >> firstWord >> secondWord;
            energy = atof(secondWord.c_str());
            
        
            getline(file,line);
            if (line.empty()){
                
                continue;
                
            }
            //Process the second line (gamma energy)
            std::stringstream ss2(line);
                
            while(ss2 >> word){
                
                daughter.push_back(atof(word.c_str()));
                gamma.push_back((energy - atof(word.c_str())));
    
                    //gamma.push_back(atof(word.c_str())*CLHEP::keV);
                    //daughter.push_back((energy - atof(word.c_str()))*CLHEP::keV);
                }
            
            ss1.clear();
            ss2.clear();
                
            getline(file,line);
            if (line.empty()){
                
                continue;
                
            }
            //get the third line
            std::stringstream ss3(line);
                
            while(ss3 >> word){
                    
                    br.push_back(atof(word.c_str()));
                
                }
            
            
            ss3.clear();
            
            //Print the data as check
            /*
            std::cout << "Energy: " << energy << std::endl;
            for (const auto &d : daughter) {
                        std::cout << "Daughter: " << d << std::endl;
                    }
            for (const auto &b : br) {
                        std::cout << "Branching Ratio: " << b << std::endl;
               
            
        }
             */
            
            
            
            
            double probability[br.size()];
            double gammas[gamma.size()];
            double daughters[daughter.size()];
            double prob[br.size()];
            
            for(unsigned long j=0 ; j < gamma.size() ; j++) gammas[j] = gamma.at(j);
            for(unsigned long j=0 ; j < daughter.size() ; j++) daughters[j] = daughter.at(j);
            for(unsigned long j=0 ; j < br.size() ; j++) prob[j] = br.at(j);
                
            G4double somma=0;
            
            for(unsigned long j=0 ; j < br.size() ; j++)
            {
                somma += prob[j];
            }
            
            for(unsigned long j=0 ; j < br.size() ; j++)
            
            {
                probability[j] = 100*prob[j]/somma;
            }
            
            levels->push_back(*(new Level((G4double) energy, daughters, probability,
                                          (int) br.size())));
            
            br.clear();
            daughter.clear();
            gamma.clear();
            
        }
        
        file.close();
    
        
    }
    
    //Ground state
    levels->push_back(*(new Level(0* CLHEP::MeV)));
    
    //Check
    
    cout <<"Number of total Levels" << levels->size() << endl;
    
    std::cout << "---------------" << std::endl;
    string reply;
    
    G4cout << "Do you want to display the Energy levels of 26Mg? Type y/n" << std::endl;
    cin >> reply;
    
    
    for(unsigned long i = 0; i < levels->size(); i++)
    {
        
        const G4double *daughterss = levels->at(i).getDaughter();
        int numDaughters = levels->at(i).getSize();
        const G4double *probss = levels->at(i).getBranching();
       
        
        if(reply=="y"){
            
            cout << "Level " << i + 1 << ":" << std::endl;
            cout << "Energy: " << levels->at(i).getEnergy() << " keV" << std::endl;
            cout << "Daughter: ";
            
            for (int j = 0; j < numDaughters; j++) {
                cout << daughterss[j] << " keV ";
            }
            cout << std::endl;
            
            
            
            cout << "Branching ratio " ;
            for (int j = 0; j < numDaughters; j++) {
                
                cout << probss[j] << " % ";
            }
            cout << std::endl;
        }
        
        if(reply=="n"){
            continue;
        }
        
    }
 
    return 0;
}
