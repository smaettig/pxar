#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

#include <cstdlib>
#include <string.h>
#include <stdio.h>

#include <TString.h>

#include "log.h"
#include "ConfigParameters.hh"

using namespace std;
using namespace pxar;


ConfigParameters * ConfigParameters::fInstance = 0;

// ----------------------------------------------------------------------
bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == ' '); }

// ----------------------------------------------------------------------
void replaceAll(string& str, const string& from, const string& to) {
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

// ----------------------------------------------------------------------
ConfigParameters::ConfigParameters() {
  initialize();
}


// ----------------------------------------------------------------------
ConfigParameters::ConfigParameters(string filename) {
  initialize();
  readConfigParameterFile(filename);
}


// ----------------------------------------------------------------------
void ConfigParameters::initialize() {
  fReadTbParameters = fReadTbmParameters = fReadDacParameters = fReadRocPixelConfig = false; 
  fnCol = 52; 
  fnRow = 80; 
  fnRocs = 16;
  fnTbms = 1; 
  fnModules = 1;
  fHubId = 31;
  
  fCustomModule = 0;

  fHvOn = true;
  fTbmEnable = true;
  fTbmEmulator = false;
  fKeithleyRemote = false;
  fGuiMode = false;
  fTbmChannel = 0;
  fHalfModule = 0;

  fEmptyReadoutLength = 54;
  fEmptyReadoutLengthADC = 64;
  fEmptyReadoutLengthADCDual = 40;

  fDACParametersFileName  = "defaultDACParameters.dat";
  fTbmParametersFileName  = "defaultTBMParameters.dat";
  fTBParametersFileName   = "defaultTBParameters.dat";
  fTrimParametersFileName = "defaultTrimParameters.dat";
  fTestParametersFileName = "defaultTestParameters.dat";
  fMaskFileName           = "defaultMaskFile.dat";
  fLogFileName            = "log.txt";
  fDebugFileName          = "debug.log";
  fRootFileName           = "expert.root";

  ia = -1.; 
  id = -1.;
  va = -1.;
  vd = -1.;

  rocZeroAnalogCurrent = 0.0;
  fRocType = "psi46v2";
  fTbmType = ""; 
}


// ----------------------------------------------------------------------
ConfigParameters*  ConfigParameters::Singleton() {
  if (fInstance == 0) {fInstance = new ConfigParameters();}
  return fInstance;
}


// ----------------------------------------------------------------------
bool ConfigParameters::readConfigParameterFile(string file) {
  ifstream _input(file.c_str());
  if (!_input.is_open())
    {
      LOG(logINFO) << "Can not open file '"  << file << "' to read Config Parameters.";

      return false;
    }

  // Read file by lines
  for (string _line; _input.good();)
    {
      getline(_input, _line);

      // Skip Empty Lines and Comments (starting from # or - )
      if (!_line.length()
	  || '#' == _line[0]
	  || '-' == _line[0]) continue;

      istringstream _istring(_line);
      string _name;
      string _value;

      _istring >> _name >> _value;

      // Skip line in case any errors occured while reading parameters
      if (_istring.fail() || !_name.length()) continue;

      int _ivalue = atoi(_value.c_str());
      float dvalue = atof(_value.c_str());
      
      if (0 == _name.compare("testboardName")) { fTBName = _value; }
      else if (0 == _name.compare("directory")) { fDirectory =  _value; }

      else if (0 == _name.compare("tbParameters")) { setTBParameterFileName(_value); }
      else if (0 == _name.compare("tbmParameters")) { setTbmParameterFileName(_value); }
      else if (0 == _name.compare("testParameters")) { setTestParameterFileName(_value); }
      else if (0 == _name.compare("dacParameters")) { setDACParameterFileName(_value); }
      else if (0 == _name.compare("rootFileName")) { setRootFileName(_value); }
      else if (0 == _name.compare("trimParameters")) { setTrimParameterFileName(_value); }
      else if (0 == _name.compare("maskFile")) { setMaskFileName(_value); }

      else if (0 == _name.compare("nModules")) { fnModules                  = _ivalue; }
      else if (0 == _name.compare("nRocs")) { fnRocs                     = _ivalue; }
      else if (0 == _name.compare("nTbms")) { fnTbms                     = _ivalue; }
      else if (0 == _name.compare("hubId")) { fHubId                     = _ivalue; }
      else if (0 == _name.compare("customModule")) { fCustomModule              = _ivalue; }
      else if (0 == _name.compare("halfModule")) { fHalfModule                = _ivalue; }
      else if (0 == _name.compare("dataTriggerLevel")) { fDataTriggerLevel          = _ivalue; }
      else if (0 == _name.compare("emptyReadoutLength")) { fEmptyReadoutLength        = _ivalue; }
      else if (0 == _name.compare("emptyReadoutLengthADC")) { fEmptyReadoutLengthADC     = _ivalue; }
      else if (0 == _name.compare("emptyReadoutLengthADCDual")) { fEmptyReadoutLengthADCDual = _ivalue; }
      else if (0 == _name.compare("hvOn")) { fHvOn                      = _ivalue; }
      else if (0 == _name.compare("keithleyRemote")) { fKeithleyRemote            = _ivalue; }
      else if (0 == _name.compare("tbmEnable")) { fTbmEnable                 = _ivalue; }
      else if (0 == _name.compare("tbmEmulator")) { fTbmEmulator             = _ivalue; }
      else if (0 == _name.compare("tbmChannel")) { fTbmChannel                = _ivalue; }

      else if (0 == _name.compare("ia")) { cout << "ia dvalue: " << dvalue << " _value: " << _value << endl; ia = (dvalue > 1000.?.001:1.) * dvalue; }
      else if (0 == _name.compare("id")) { id = (dvalue > 1000.?.001:1.) * dvalue; }
      else if (0 == _name.compare("va")) { va = (dvalue > 1000.?.001:1.) * dvalue; }
      else if (0 == _name.compare("vd")) { vd = (dvalue > 1000.?.001:1.) * dvalue; }

      else if (0 == _name.compare("rocZeroAnalogCurrent")) { rocZeroAnalogCurrent = .001 * _ivalue; }

      else if (0 == _name.compare("rocType")) { fRocType = _value; }
      else if (0 == _name.compare("tbmType")) { fTbmType = _value; }

      else { LOG(logINFO) << "Did not understand '" << _name << "'."; }
    }

  _input.close();

  return true;
}


// ----------------------------------------------------------------------
vector<pair<string, uint8_t> > ConfigParameters::readDacFile(string fname) {
  vector<pair<string, uint8_t> > rocDacs; 

  // -- read in file
  vector<string> lines; 
  char  buffer[5000];
  LOG(logINFO) << "  readDacFile  reading " << fname;
  ifstream is(fname.c_str());
  while (is.getline(buffer, 200, '\n')) {
    lines.push_back(string(buffer));
  }
  is.close();

  // -- parse lines
  unsigned int ival(0); 
  uint8_t uval(0); 
  //  unsigned char uval(0); 
  
  string::size_type s1, s2; 
  string str1, str2, str3;
  for (unsigned int i = 0; i < lines.size(); ++i) {
    //    cout << lines[i] << endl;   
    // -- remove tabs, adjacent spaces, leading and trailing spaces
    replaceAll(lines[i], "\t", " "); 
    string::iterator new_end = unique(lines[i].begin(), lines[i].end(), BothAreSpaces);
    lines[i].erase(new_end, lines[i].end()); 
    if (0 == lines[i].length()) continue;
    if (lines[i].substr(0, 1) == string(" ")) lines[i].erase(0, 1); 
    if (lines[i].substr(lines[i].length()-1, 1) == string(" ")) lines[i].erase(lines[i].length()-1, 1); 
    s1 = lines[i].find(" "); 
    s2 = lines[i].rfind(" "); 
    if (s1 != s2) {
      str1 = lines[i].substr(0, s1); 
      str2 = lines[i].substr(s1+1, s2-s1-1); 
      str3 = lines[i].substr(s2+1); 
    } else {
      LOG(logINFO) << "could not read line -->" << lines[i] << "<--";
    }
    
    // -- why, oh why?!
    ival = atoi(str3.c_str()); 
    uval = ival;
    // const unsigned char* us =  reinterpret_cast<const unsigned char*>(str3.c_str());
    // const uint8_t *us2 =  reinterpret_cast<const uint8_t*>(str3.data());
    // uval = ival;  does not work at all          
    // uval = *us;   strips off trailing digits   
    // uval = *us2;  strips off trailing digits   
    //    uval = translate(ival); 
    //    cout << "    str3 = " << str3 << " ival = " << ival << " uval = " << uval << " int(uval) = " << int(uval) << endl;
    rocDacs.push_back(make_pair(str2, uval)); 

  }

  return rocDacs; 
}

// ----------------------------------------------------------------------
vector<pair<string, uint8_t> >  ConfigParameters::getTbParameters() {
  string filename = Form("%s", fTBParametersFileName.c_str()); 
  fTbParameters = readDacFile(filename); 
  fReadTbParameters = true; 
  return fTbParameters;
}

// ----------------------------------------------------------------------
vector<pair<string, double> >  ConfigParameters::getTbPowerSettings() {
  vector<pair<string, double> > v; 
  if (ia < 0.) {
    LOG(logINFO) << "Read config files first!" << endl;    
    return v; 
  }
  v.push_back(make_pair("ia", ia));   
  v.push_back(make_pair("id", id)); 
  v.push_back(make_pair("va", va));   
  v.push_back(make_pair("vd", vd)); 
  return v; 
}

// ----------------------------------------------------------------------
vector<pair<string, uint8_t> >  ConfigParameters::getTbSigDelays() {
  vector<pair<string, uint8_t> > a;

  vector<string> sigdelays; 
  sigdelays.push_back("clk");
  sigdelays.push_back("ctr");
  sigdelays.push_back("sda");
  sigdelays.push_back("tin");
  sigdelays.push_back("deser160phase");

  if (!fReadTbParameters) getTbParameters();
  for (unsigned int i = 0; i < fTbParameters.size(); ++i) {
    for (unsigned int j = 0; j < sigdelays.size(); ++j) {
      if (0 == fTbParameters[i].first.compare(sigdelays[j])) a.push_back(make_pair(sigdelays[j], fTbParameters[i].second));
    }
  }

  return a;
}

// ----------------------------------------------------------------------
vector<pair<uint16_t, uint8_t> >  ConfigParameters::getTbPgSettings() {
  vector<pair<uint16_t, uint8_t> > a;

  if (fnTbms < 1) {
    a.push_back(make_pair(0x0800,25));    // PG_RESR
    a.push_back(make_pair(0x0400,101+5)); // PG_CAL
    a.push_back(make_pair(0x0200,16));    // PG_TRG
    a.push_back(make_pair(0x0100,0));     // PG_TOK
  } else {
    a.push_back(std::make_pair(0x1000,15)); // PG_REST
    a.push_back(std::make_pair(0x0400,50)); // PG_CAL
    a.push_back(std::make_pair(0x2200,0));  // PG_TRG PG_SYNC
  }

  return a;
}


// ----------------------------------------------------------------------
vector<vector<pxar::pixelConfig> > ConfigParameters::getRocPixelConfig() {
  string filename; 

  // -- read one mask file containing entire DUT mask
  filename = Form("%s", fMaskFileName.c_str()); 
  vector<bool> rocmasked; 
  for (unsigned int i = 0; i < fnRocs; ++i) rocmasked.push_back(false); 

  vector<vector<pair<int, int> > > vmask = readMaskFile(filename); 
  for (unsigned int i = 0; i < vmask.size(); ++i) {
    vector<pair<int, int> > v = vmask[i]; 
    if (v.size() > 0) {
      rocmasked[i] = true; 
      for (unsigned int j = 0; j < v.size(); ++j) {
	LOG(logINFO) << "MASKED Roc " << i << " col/row: " << v[j].first << " " << v[j].second;
      }
    }
  }

  // -- read all trim files and creat pixelconfig vector
  for (int i = 0; i < fnRocs; ++i) {
    vector<pxar::pixelConfig> v;
    for (int ic = 0; ic < fnCol; ++ic) {
      for (int ir = 0; ir < fnRow; ++ir) {
	pxar::pixelConfig a; 
	a.column = ic; 
	a.row = ir; 
	a.trim = 0; 
	if (rocmasked[i]) {
	  vector<pair<int, int> > v = vmask[i]; 
	  for (unsigned int j = 0; j < v.size(); ++j) {
	    if (v[j].first == ic && v[j].second == ir) {
	      LOG(logINFO) << "  masking Roc " << i << " col/row: " << v[j].first << " " << v[j].second;
	      a.mask = true;
	    }
	  }
	} else {
	  a.mask = false;
	}
	a.enable = true;
	v.push_back(a); 
      }
    }
    filename = Form("%s_C%d.dat", fTrimParametersFileName.c_str(), i); 
    readTrimFile(filename, v); 
    fRocPixelConfigs.push_back(v); 
  }

  fReadRocPixelConfig = true; 
  return fRocPixelConfigs;

}


// ----------------------------------------------------------------------
void ConfigParameters::readTrimFile(string fname, vector<pxar::pixelConfig> &v) {

  // -- read in file
  vector<string> lines; 
  char  buffer[5000];
  LOG(logINFO) << "  readTrimFile reading " << fname;
  ifstream is(fname.c_str());
  while (is.getline(buffer, 200, '\n')) {
    lines.push_back(string(buffer));
  }
  is.close();
  
  // -- parse lines
  unsigned int ival(0), irow(0), icol(0); 
  uint8_t uval(0), urow(0), ucol(0); 
  
  string::size_type s1, s2; 
  string str1, str2, str3;
  for (unsigned int i = 0; i < lines.size(); ++i) {
    //    cout << lines[i] << endl;   
    // -- remove tabs, adjacent spaces, leading and trailing spaces
    replaceAll(lines[i], "\t", " "); 
    replaceAll(lines[i], "Pix", " "); 
    string::iterator new_end = unique(lines[i].begin(), lines[i].end(), BothAreSpaces);
    lines[i].erase(new_end, lines[i].end()); 
    if (0 == lines[i].length()) continue;
    if (lines[i].substr(0, 1) == string(" ")) lines[i].erase(0, 1); 
    if (lines[i].substr(lines[i].length()-1, 1) == string(" ")) lines[i].erase(lines[i].length()-1, 1); 
    s1 = lines[i].find(" "); 
    s2 = lines[i].rfind(" "); 
    if (s1 != s2) {
      str1 = lines[i].substr(0, s1); 
      str2 = lines[i].substr(s1+1, s2-s1); 
      str3 = lines[i].substr(s2+1); 
    } else {
      LOG(logINFO) << "could not read line -->" << lines[i] << "<--";
    }
    
    // -- why, oh why?!
    ival = atoi(str1.c_str()); 
    icol = atoi(str2.c_str()); 
    irow = atoi(str3.c_str()); 
    uval = ival;
    ucol = icol;
    urow = irow;
    unsigned int index = icol*80+irow; 
    if (index <= v.size()) {
      v[index].trim = uval; 
    } else {
      LOG(logINFO) << " not matching entry in trim vector found for row/col = " << irow << "/" << icol;
    }
    //     cout << "col/row = " << icol << "/" << irow << " trim = " << ival 
    // 	 << " pixelConfig: " << int(v[index].column) << "/" << int(v[index].row)
    // 	 << endl;
  }


}


// ----------------------------------------------------------------------
vector<vector<pair<int, int> > > ConfigParameters::readMaskFile(string fname) {

  vector<vector<pair<int, int> > > v; 
  vector<pair<int, int> > a; 
  for (unsigned int i = 0; i < fnRocs; ++i) {
    v.push_back(a); 
  }

  // -- read in file
  vector<string> lines; 
  char  buffer[5000];
  LOG(logINFO) << "  readMaskFile reading " << fname;
  ifstream is(fname.c_str());
  while (is.getline(buffer, 200, '\n')) {
    lines.push_back(string(buffer));
  }
  is.close();
  
  // -- parse lines
  unsigned int iroc(0), irow(0), icol(0); 
  uint8_t uval(0), urow(0), ucol(0); 
  
  string::size_type s1, s2, s3; 
  string str1, str2, str3;
  for (unsigned int i = 0; i < lines.size(); ++i) {
    //    cout << lines[i] << endl;   
    if (lines[i].substr(0, 1) == string("#")) continue;
    // -- remove tabs, adjacent spaces, leading and trailing spaces
    replaceAll(lines[i], "\t", " "); 
    replaceAll(lines[i], "Pix", " "); 
    string::iterator new_end = unique(lines[i].begin(), lines[i].end(), BothAreSpaces);
    lines[i].erase(new_end, lines[i].end()); 
    if (lines[i].substr(0, 1) == string(" ")) lines[i].erase(0, 1); 
    if (0 == lines[i].length()) continue;
    if (lines[i].substr(lines[i].length()-1, 1) == string(" ")) lines[i].erase(lines[i].length()-1, 1); 

    s1 = lines[i].find("roc"); 
    if (string::npos != s1) {
      str3 = lines[i].substr(s1+1); 
      iroc = atoi(str3.c_str()); 
      //      cout << "masking all pixels for ROC " << iroc << endl;
      if (iroc >= 0 && iroc < fnRocs) {
	for (unsigned int ic = 0; ic < fnCol; ++ic) {
	  for (unsigned int ir = 0; ir < fnRow; ++ir) {
	    v[iroc].push_back(make_pair(ic, ir)); 
	  }
	}  
      } else {
	LOG(logINFO) << "illegal ROC coordinates in line " << i << ": " << lines[i];
      }
      continue;
    }
    
    s1 = lines[i].find("row"); 
    if (string::npos != s1) {
      s1 += 4; 
      s2 = lines[i].find(" ", s1) + 1; 
      iroc = atoi(lines[i].substr(s1, s2-s1-1).c_str()); 
      irow = atoi(lines[i].substr(s2).c_str()); 
      //      cout << "-> MASKING ROC " << iroc << " row " << irow << endl;
      if (iroc >= 0 && iroc < fnRocs && irow > 0 && irow < fnRow) {
	for (unsigned int ic = 0; ic < fnCol; ++ic) {
	  v[iroc].push_back(make_pair(ic, irow)); 
	}  
      } else {
	LOG(logINFO) << "illegal ROC/row coordinates in line " << i << ": " << lines[i];
      }
      continue;
    }

    s1 = lines[i].find("col"); 
    if (string::npos != s1) {
      s1 += 4; 
      s2 = lines[i].find(" ", s1) + 1; 
      iroc = atoi(lines[i].substr(s1, s2-s1-1).c_str()); 
      icol = atoi(lines[i].substr(s2).c_str()); 
      //      cout << "-> MASKING ROC " << iroc << " col " << icol << endl;
      if (iroc >= 0 && iroc < fnRocs && icol > 0 && icol < fnCol) {
	for (unsigned int ir = 0; ir < fnRow; ++ir) {
	  v[iroc].push_back(make_pair(icol, ir)); 
	}  
      } else {
	LOG(logINFO) << "illegal ROC/col coordinates in line " << i << ": " << lines[i];
      }
      continue;
    }

    s1 = lines[i].find("pix"); 
    if (string::npos != s1) {
      s1 += 4; 
      s2 = lines[i].find(" ", s1) + 1; 
      s3 = lines[i].find(" ", s2) + 1; 
      iroc = atoi(lines[i].substr(s1, s2-s1-1).c_str()); 
      icol = atoi(lines[i].substr(s2, s3-s2-1).c_str()); 
      irow = atoi(lines[i].substr(s3).c_str()); 
      //      cout << "-> MASKING ROC " << iroc << " col " << icol << " row " << irow << endl;
      if (iroc >= 0 && iroc < fnRocs && icol > 0 && icol < fnCol && irow > 0 && irow < fnRow) {
	v[iroc].push_back(make_pair(icol, irow)); 
      } else {
	LOG(logINFO) << "illegal ROC/row/col coordinates in line " << i << ": " << lines[i];
      }
      continue;
    }
  }

  return v; 
}



// ----------------------------------------------------------------------
vector<vector<pair<string, uint8_t> > > ConfigParameters::getRocDacs() {
  string filename; 
  for (int i = 0; i < fnRocs; ++i) {
    filename = Form("%s_C%d.dat", fDACParametersFileName.c_str(), i); 
    vector<pair<string, uint8_t> > rocDacs = readDacFile(filename); 
    fDacParameters.push_back(rocDacs); 
  }
  fReadDacParameters = true; 
  return fDacParameters; 
}


// ----------------------------------------------------------------------
vector<vector<pair<string, uint8_t> > > ConfigParameters::getTbmDacs() {
  string filename; 
  for (int i = 0; i < fnTbms; ++i) {
    filename = Form("%s", fTbmParametersFileName.c_str()); 
    vector<pair<string, uint8_t> > rocDacs = readDacFile(filename); 
    fTbmParameters.push_back(rocDacs); 
  }
  fReadTbmParameters = true; 
  return fTbmParameters; 
}


// ----------------------------------------------------------------------
void ConfigParameters::setTBParameterFileName(const string &file) {
  fTBParametersFileName.assign(fDirectory).append("/").append(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setTbmParameterFileName(const string &file) {
  fTbmParametersFileName.assign(fDirectory).append("/").append(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setDACParameterFileName(const string &file) {
  fDACParametersFileName.assign(fDirectory).append("/").append(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setTrimParameterFileName(const string &file) {
  fTrimParametersFileName.assign(fDirectory).append("/").append(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setTestParameterFileName(const string &file) {
  fTestParametersFileName.assign(fDirectory).append("/").append(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setRootFileName(const string &file) {
  fRootFileName.assign(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setLogFileName(const string &file) {
  fLogFileName.assign(fDirectory).append("/").append(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setDebugFileName(const string &file) {
  fDebugFileName.assign(fDirectory).append("/").append(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setMaskFileName(const string &file) {
  fMaskFileName.assign(fDirectory).append("/").append(file);
}


// ----------------------------------------------------------------------
void ConfigParameters::setDirectory(string &d) {
  fDirectory = d;
}

// ----------------------------------------------------------------------
bool ConfigParameters::writeConfigParameterFile() {
  char filename[1000];
  sprintf(filename, "%s/configParameters.dat", fDirectory.c_str());
  FILE * file = fopen(filename, "w");
  if (!file)
    {
      LOG(logINFO) << "Can not open file '" << filename << "' to write configParameters.";
      return false;
    }

  LOG(logINFO) << "Writing Config-Parameters to '" << filename << "'.";

  fprintf(file, "testboardName %s\n\n", fTBName.c_str());

  fprintf(file, "-- parameter files\n\n");

  fprintf(file, "tbParameters %s\n",   fTBParametersFileName.c_str());
  fprintf(file, "dacParameters %s\n",  fDACParametersFileName.c_str());
  fprintf(file, "tbmParameters %s\n",  fTbmParametersFileName.c_str());
  fprintf(file, "trimParameters %s\n", fTrimParametersFileName.c_str());
  fprintf(file, "testParameters %s\n", fTestParametersFileName.c_str());
  fprintf(file, "rootFileName %s\n\n", fRootFileName.c_str());

  fprintf(file, "-- configuration\n\n");

  if (fCustomModule) fprintf(file, "customModule %i\n", fCustomModule);

  fprintf(file, "nModules %i\n", fnModules);
  fprintf(file, "nRocs %i\n", fnRocs);
  fprintf(file, "hubId %i\n", fHubId);
  fprintf(file, "tbmEnable %i\n", fTbmEnable);
  fprintf(file, "tbmEmulator %i\n", fTbmEmulator);
  fprintf(file, "hvOn %i\n", fHvOn);
  fprintf(file, "tbmChannel %i\n\n", fTbmChannel);
  fprintf(file, "halfModule %i\n\n", fHalfModule);
  fprintf(file, "rocType %s\n\n", fRocType.c_str());
  fprintf(file, "tbmType %s\n\n", fTbmType.c_str());

  fprintf(file, "-- voltages and current limits\n\n");

  fprintf(file, "ia %i\n"  , static_cast<int>(ia * 1000));
  fprintf(file, "id %i\n"  , static_cast<int>(id * 1000));
  fprintf(file, "va %i\n"  , static_cast<int>(va * 1000));
  fprintf(file, "vd %i\n\n", static_cast<int>(vd * 1000));

  fprintf(file, "rocZeroAnalogCurrent %i\n\n", static_cast<int>(rocZeroAnalogCurrent * 1000));

  fclose(file);
  return true;
}


// ----------------------------------------------------------------------
bool ConfigParameters::setTbParameter(std::string var, uint8_t val) {
  for (unsigned int i = 0; i < fTbParameters.size(); ++i) {
    if (!fTbParameters[i].first.compare(var)) {
      fTbParameters[i].second = val;
      cout << "ConfigParameters::setTbParameter> Updated " << fTbParameters[i].first << " to " << fTbParameters[i].second << endl;
      return true; 
    }
  }
  return false; 
}

// ----------------------------------------------------------------------
bool ConfigParameters::setTbPowerSettings(std::string var, double val) {
  for (unsigned int i = 0; i < fTbPowerSettings.size(); ++i) {
    if (!fTbPowerSettings[i].first.compare(var)) {
      fTbPowerSettings[i].second = val;
      cout << "ConfigParameters::setTbParameter> Updated " << fTbPowerSettings[i].first << " to " << fTbPowerSettings[i].second << endl;
      return true; 
    }
  }
  return false; 

}
