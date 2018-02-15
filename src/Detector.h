#ifndef DETECTOR_H
#define DETECTOR_H
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

#include <stdexcept>

#include "sls_receiver_defs.h"
#include "sls_detector_defs.h"
#include "slsDetectorUtils.h"
#include "multiSlsDetector.h"
#include "slsDetector.h"
#include "error_defs.h"

class Detector{
public:
    Detector(int i=0):det(i){
        //added default arg to cope with branch
        //removed when going back to developer


        //Disable any output from std::cout
        std::cout.setstate(std::ios_base::failbit);

    }

    //get image size as [nrow, ncols] return as a pair of ints
    std::pair<int, int> getImageSize(){
        std::pair<int, int> image_size{0,0};
        image_size.first = det.getMaxNumberOfChannelsPerDetector(slsDetectorDefs::dimension::Y);
        image_size.second = det.getMaxNumberOfChannelsPerDetector(slsDetectorDefs::dimension::X);
        return image_size;
    }

    void setImageSize(int rows, int cols){
        det.setMaxNumberOfChannelsPerDetector(slsDetectorDefs::dimension::Y, rows);
        det.setMaxNumberOfChannelsPerDetector(slsDetectorDefs::dimension::X, cols);
    }

    //blocking command, acquire set number of frames
    void acquire(){ det.acquire(); }


    //for Eiger check status of  the module
    //true active false deactivated
    bool getActive(const int det_id){
        auto d = det.getSlsDetector(det_id);
        if (d)
            return d->activate();
        else
            throw std::runtime_error("could not get detector");
    }

    //activate or deactivate a module
    void setActive(const int det_id, bool value){
        auto d = det.getSlsDetector(det_id);
        if (d)
            d->activate(value);
        else
            throw std::runtime_error("could not get detector");
    }

    int getFramesCaughtByReceiver(){
        return det.getFramesCaughtByReceiver();
    }

    void resetFramesCaught(){
        det.resetFramesCaught();
    }

    int getReceiverCurrentFrameIndex(){
        return det.getReceiverCurrentFrameIndex();
    }

    bool getThreadedProcessing(){
        return det.setThreadedProcessing();
    }
    void setThreadedProcessing(bool value){
        det.setThreadedProcessing(value);
    }

    void startReceiver(){
        det.startReceiver();
    }
    void stopReceiver(){
        det.stopReceiver();
    }

    bool getTenGigabitEthernet(){
        return det.enableTenGigabitEthernet();
    }
    void setTenGigabitEthernet(bool value){
        det.enableTenGigabitEthernet(value);
    }

    int getFileFormat(){
        return det.getFileFormat();
    }

    std::string checkOnline(){
       return det.checkOnline();
    }

    void clearErrorMask(){
        det.clearAllErrorMask();
    }

    int64_t getErrorMask(){
        return det.getErrorMask();
    }

    std::string getErrorMessage(){
        //tmp would hold the number of critical errors, is and should this be used?
        int tmp=0;
        return det.getErrorMessage(tmp);
    }

    bool getReceiverOnline(){
        return det.setReceiverOnline();
    }
    void setReceiverOnline(bool status){
        det.setReceiverOnline(status);
    }

    bool getOnline(){
        return det.setOnline();
    }
    void setOnline(bool status){
        det.setOnline(status);
    }


    bool isChipPowered(){
        return det.powerChip();
    }
    void powerChip(bool value){
        det.powerChip(value);
    }


    //read register from readout system, used for low level control
    int readRegister(int addr){
        return det.readRegister(addr);
    }

    //directly write to register in readout system
    void writeRegister(int addr, int value){
        det.writeRegister(addr, value);
    }

    //directly write to the ADC register
    void writeAdcRegister(int addr, int value){
        det.writeAdcRegister(addr, value);
    }

    bool getAcquiringFlag(){
        return det.getAcquiringFlag();
    }

    void setAcquiringFlag(bool flag){
        det.setAcquiringFlag(flag);
    }

    bool getCounterBit(){
        return det.setCounterBit();
    }
    void setCounterBit(bool b){
        det.setCounterBit(b);
    }

    slsDetectorDefs::dacIndex dacNameToEnum(std::string dac_name);

    std::pair<int, int> getDetectorGeometry(){
        std::pair<int, int> g;
        det.getNumberOfDetectors(g.first, g.second);
        return g;
    }

    int getNumberOfDetectors(){
        return det.getNumberOfDetectors();
    }

    std::string getRunStatus(){
        auto s = det.getRunStatus();
        return det.runStatusType(s);
    }


    void startAcquisition(){ det.startAcquisition(); }
    void stopAcquisition(){ det.stopAcquisition(); }

    std::string getHostname(){
        return det.getHostname();
    }

    void setHostname(std::string hostname){
        det.setHostname(hostname.c_str());
    }
    
    int getDynamicRange(){
        return det.setDynamicRange(-1);
    }
    void setDynamicRange(int dr){
        det.setDynamicRange(dr);
    }

    void pulseChip(int n){ det.pulseChip(n); }
    void pulseAllPixels(int n);
    void pulseDiagonal(int n);

    void readConfigurationFile(std::string fname){ det.readConfigurationFile(fname);}
    void readParametersFile(std::string fname){ det.retrieveDetectorSetup(fname); }
    int getReadoutClockSpeed(){ return det.setSpeed(slsDetectorDefs::CLOCK_DIVIDER, -1); }

    int getFirmwareVersion(){ return det.getId(slsDetectorDefs::DETECTOR_FIRMWARE_VERSION); }
    int getSoftwareVersion(){  return det.getId(slsDetectorDefs::DETECTOR_SOFTWARE_VERSION); }

    void setReadoutClockSpeed(int speed){
        det.setSpeed(slsDetectorDefs::CLOCK_DIVIDER, speed);
    }

    int getRxTcpport(const int i){
        int port = -1;
        auto index = slsDetectorDefs::portType::DATA_PORT;
        auto _d = det.getSlsDetector(i);

        //protect from accesing a nullptr
        if (_d)
            port = _d->setPort(index);

        return port;
    }
    void setRxTcpport(const int i, const int value){
        auto index = slsDetectorDefs::portType::DATA_PORT;
        auto _d = det.getSlsDetector(i);
        //protect from accesing a nullptr
        if (_d)
            _d->setPort(index, value);
    }

    void setRateCorrection(std::vector<double> tau){
        slsDetector* _d;
        for (int i=0; i<det.getNumberOfDetectors(); ++i){
            _d = det.getSlsDetector(i);
            _d->setRateCorrection(tau[i]);
        }

    }



    std::vector<double>getRateCorrection(){
        std::vector<double> rate_corr;
        slsDetector* _d;
        for (int i=0; i<det.getNumberOfDetectors(); ++i){
            _d = det.getSlsDetector(i);
            std::pair<int, double> r = {0,0};
            r.first = _d->getRateCorrection(r.second);
            rate_corr.push_back(r.second);
        }
        return rate_corr;
    }

    bool getFlippedDataX(const int i){
        auto _d = det.getSlsDetector(i);
        if(_d){
            return _d->getFlippedData(slsDetectorDefs::dimension::X);
        }else{
            throw std::runtime_error("Detector index does not exist");
        }
    }
    bool getFlippedDataY(const int i){
        auto _d = det.getSlsDetector(i);
        if(_d){
            return _d->getFlippedData(slsDetectorDefs::dimension::Y);
        }else{
            throw std::runtime_error("Detector index does not exist");
        }
    }

    void setFlippedDataX(const int i, const bool value){
        auto _d = det.getSlsDetector(i);
        if(_d){
            _d->setFlippedData(slsDetectorDefs::dimension::X, value);
        }else{
            throw std::runtime_error("Detector index does not exist");
        }
    }
    void setFlippedDataY(const int i, const bool value){
        auto _d = det.getSlsDetector(i);
        if(_d){
            _d->setFlippedData(slsDetectorDefs::dimension::Y, value);
        }else{
            throw std::runtime_error("Detector index does not exist");
        }
    }
    //----------------------------------------------------File
    void setFileName(std::string fname){ det.setFileName(fname); }
   
    std::string getFileName(){
        return det.getFileName();
    }

    void setFilePath(std::string path){ det.setFilePath(path); }
    
    std::string getFilePath(){
        return det.getFilePath();
    }


    void loadTrimbitFile(std::string fname, const int idet){
        det.loadSettingsFile(fname, idet);
    }

    //Eiger: set the energies where the detector is trimmed
    void setTrimEnergies(std::vector<int> energy){
        det.setTrimEn(energy.size(), energy.data());
    }
    std::vector<int> getTrimEnergies(){
        //initial call to get legth, energies defaults to NULL
        auto n_trimen = det.getTrimEn();
        std::vector<int> trim_energies(n_trimen);

        //second call to get the eneries
        det.getTrimEn(trim_energies.data());
        return trim_energies;
    }

    void setThresholdEnergy(const int eV){
        //example of checking errors
        //should now be done on the python side
        det.clearAllErrorMask();
        det.setThresholdEnergy(eV);
        if (det.getErrorMask()){
            int tmp=0;
            auto msg = det.getErrorMessage(tmp);
            det.clearAllErrorMask();
            throw std::runtime_error(msg);
        }
    }

    std::string getSettingsDir(){ return det.getSettingsDir(); }
    void setSettingsDir(std::string dir){ det.setSettingsDir(dir); }

    int getThresholdEnergy(){
        return det.getThresholdEnergy();
    }

    std::string getSettings(){
        return det.getDetectorSettings(det.getSettings());
    }

    void setSettings(std::string s){
        det.clearAllErrorMask();
        det.setSettings(det.getDetectorSettings(s));
        if (det.getErrorMask()){
            int tmp=0;
            auto msg = det.getErrorMessage(tmp);
            det.clearAllErrorMask();
            throw std::runtime_error(msg);
        }
    }

    //name to enum translation on the c++ side
    //should we instead expose the enum to Python?
    dacs_t getDac(std::string dac_name, int mod_id){
        dacs_t val = -1;
        auto dac = dacNameToEnum(dac_name);
        return det.setDAC(val, dac, 0, mod_id);
    }

    void setDac(std::string dac_name, int mod_id, dacs_t val){
        auto dac = dacNameToEnum(dac_name);
        det.setDAC(val, dac, 0, mod_id);
    }

    //Calling multi do we have a need to lock/unlock a single det?
    bool getServerLock(){ return det.lockServer(-1); }
    void setServerLock(const bool value){ det.lockServer(value); }
    bool getReceiverLock(){ return det.lockReceiver(-1); }
    void setReceiverLock(const bool value){ det.lockReceiver(value); }

    dacs_t getAdc(std::string adc_name, int mod_id){
        auto adc = dacNameToEnum(adc_name);
        return det.getADC(adc, mod_id);

    }

    std::vector<std::string> getReadoutFlags(){
        std::vector<std::string> flags;
        auto r = det.setReadOutFlags();
        if(r & slsDetectorDefs::readOutFlags::STORE_IN_RAM)
            flags.push_back("storeinram");
        if(r & slsDetectorDefs::readOutFlags::TOT_MODE)
            flags.push_back("tot");
        if(r & slsDetectorDefs::readOutFlags::CONTINOUS_RO)
            flags.push_back("continous");
        if(r & slsDetectorDefs::readOutFlags::PARALLEL)
            flags.push_back("parallel");
        if(r & slsDetectorDefs::readOutFlags::NONPARALLEL)
            flags.push_back("nonparallel");
        if(r & slsDetectorDefs::readOutFlags::SAFE)
            flags.push_back("safe");
        if(r & slsDetectorDefs::readOutFlags::DIGITAL_ONLY)
            flags.push_back("digital");
        if(r & slsDetectorDefs::readOutFlags::ANALOG_AND_DIGITAL)
            flags.push_back("analog_digital");

        return flags;
    }

    //note singular
    void setReadoutFlag(const string flag_name){
        if(flag_name == "none")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::NORMAL_READOUT);
        else if(flag_name == "storeinram")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::STORE_IN_RAM);
        else if(flag_name == "tot")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::TOT_MODE);
        else if(flag_name == "continous")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::CONTINOUS_RO);
        else if(flag_name == "parallel")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::PARALLEL);
        else if(flag_name == "nonparallel")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::NONPARALLEL);
        else if(flag_name == "safe")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::SAFE);
        else if(flag_name == "digital")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::DIGITAL_ONLY);
        else if(flag_name == "analog_digital")
            det.setReadOutFlags(slsDetectorDefs::readOutFlags::ANALOG_AND_DIGITAL);
        else
            throw std::runtime_error("Flag name not recognized");

    }

    //name to enum transltion of dac
    dacs_t getDacVthreshold(){
        dacs_t val = -1;
        auto dac = slsDetectorDefs::dacIndex::THRESHOLD;
        return det.setDAC(val, dac, 0, -1);
    }
    
    void setDacVthreshold(dacs_t val){
        auto dac = slsDetectorDefs::dacIndex::THRESHOLD;
        det.setDAC(val, dac, 0, -1);
    }


    void setFileIndex(const int i){ det.setFileIndex(i); }
    int getFileIndex(){
        return det.setFileIndex(-1);
    }

    //time in ns
    void setExposureTime(const int64_t t){
        det.setTimer(slsReceiverDefs::timerIndex::ACQUISITION_TIME, t);
    }

    //time in ns
    int64_t getExposureTime(){
        return det.setTimer(slsReceiverDefs::timerIndex::ACQUISITION_TIME, -1);
    }

    void setSubExposureTime(const int64_t t){
        det.setTimer(slsReceiverDefs::timerIndex::SUBFRAME_ACQUISITION_TIME, t);
    }

    int64_t getSubExposureTime(){
        //time in ns
        return det.setTimer(slsReceiverDefs::timerIndex::SUBFRAME_ACQUISITION_TIME, -1);
    }

    int64_t getCycles(){
        return det.setTimer(slsReceiverDefs::timerIndex::CYCLES_NUMBER, -1);
    }

    void setCycles(const int64_t n_cycles){
        det.setTimer(slsReceiverDefs::timerIndex::CYCLES_NUMBER, n_cycles);
    }

    void setNumberOfMeasurements(const int n_measurements){
        det.setTimer(slsReceiverDefs::timerIndex::MEASUREMENTS_NUMBER, n_measurements);
    }
    int getNumberOfMeasurements(){
        return det.setTimer(slsReceiverDefs::timerIndex::MEASUREMENTS_NUMBER, -1);
    }

    //time in ns
    int getNumberOfGates(){
        return det.setTimer(slsReceiverDefs::timerIndex::GATES_NUMBER, -1);
    }
    //time in ns
    void setNumberOfGates(const int t){
        det.setTimer(slsReceiverDefs::timerIndex::GATES_NUMBER, t);
    }
    //time in ns
    int64_t getDelay(){
        return det.setTimer(slsReceiverDefs::timerIndex::DELAY_AFTER_TRIGGER, -1);
    }
    //time in ns
    void setDelay(const int64_t t){
        det.setTimer(slsReceiverDefs::timerIndex::DELAY_AFTER_TRIGGER, t);
    }
    //time in ns
    int64_t getPeriod(){
        return det.setTimer(slsReceiverDefs::timerIndex::FRAME_PERIOD, -1);
    }
    //time in ns
    void setPeriod(const int64_t t){
        det.setTimer(slsReceiverDefs::timerIndex::FRAME_PERIOD, t);
    }

    int64_t getNumberOfFrames(){
        return det.setTimer(slsReceiverDefs::timerIndex::FRAME_NUMBER, -1);
    }

    void setNumberOfFrames(const int64_t nframes){
        det.setTimer(slsReceiverDefs::timerIndex::FRAME_NUMBER, nframes);
    }

    std::string getTimingMode(){
        return det.externalCommunicationType(det.setExternalCommunicationMode());
    }
    void setTimingMode(const std::string mode){
        det.setExternalCommunicationMode(det.externalCommunicationType(mode));
    }

    void freeSharedMemory(){
        det.freeSharedMemory();
    }

    std::vector<std::string> getDetectorType(){
        std::vector<std::string> detector_type;
        for (int i=0; i<det.getNumberOfDetectors(); ++i){
            detector_type.push_back(det.sgetDetectorsType(i));
        }
        return detector_type;
    }



    void setFileWrite(bool value){
        if (value == true)
            det.enableWriteToFile(1);
        else
            det.enableWriteToFile(0);
    }

    bool getFileWrite(){
        auto r = det.enableWriteToFile(-1);
        if (r==1)
            return true;
        else
            return false;
    }


    void setAllTrimbits(int tb){
        det.setAllTrimbits(tb);
    }

    int getAllTrimbits(){
        return det.setAllTrimbits(-1);
    }
    
    bool getRxDataStreamStatus(){
        //can we implicitly convert?
        return det.enableDataStreamingFromReceiver();
    }
    
    void setRxDataStreamStatus(bool state){
        det.enableDataStreamingFromReceiver(state);
    }
    
    //Get a network parameter for all detectors, looping over individual detectors
    //return a vector of strings
    std::vector<std::string> getNetworkParameter(std::string par_name){
        auto p = networkNameToEnum(par_name);
        std::vector<std::string> par;
        slsDetector* _d;
        for (int i=0; i<det.getNumberOfDetectors(); ++i){
            _d = det.getSlsDetector(i);
            par.push_back(_d->getNetworkParameter(p));
        }
        return par;
    }

    //Set network parameter for all modules if det_id == -1 otherwise the module
    //specified with det_id.
    void setNetworkParameter(std::string par_name, std::string par, const int det_id){
        auto p = networkNameToEnum(par_name);
        if (det_id == -1){
            det.setNetworkParameter(p, par);
        }else{
            auto _d = det.getSlsDetector(det_id);
            if (_d)
                _d->setNetworkParameter(p, par);
            else
                throw std::runtime_error("could not get detector");
        }

    }


    std::string getLastClientIP(){
        return det.getLastClientIP();
    }


    //get frame delay of module (det_id) in ns
    int getDelayFrame(const int det_id){
        auto _d = det.getSlsDetector(det_id);
        if (_d){
            auto r = _d->getNetworkParameter(slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_FRAME);
            return std::stoi(r);

        }else{
            throw std::runtime_error("could not get detector");
        }
    }
    //set frame delay of module (det_id) in ns
    void setDelayFrame(const int det_id, const int delay){
        auto delay_str = std::to_string(delay);
        auto _d = det.getSlsDetector(det_id);
        if (_d){
            _d->setNetworkParameter(slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_FRAME, delay_str);
        }else{
            throw std::runtime_error("could not get detector");
        }
    }

    //get delay left of module (det_id) in ns
    int getDelayLeft(const int det_id){
        auto _d = det.getSlsDetector(det_id);
        if (_d){
            auto r = _d->getNetworkParameter(slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_LEFT);
            return std::stoi(r);

        }else{
            throw std::runtime_error("could not get detector");
        }
    }
    //set delay left of module (det_id) in ns
    void setDelayLeft(const int det_id, const int delay){
        auto delay_str = std::to_string(delay);
        auto _d = det.getSlsDetector(det_id);
        if (_d){
            _d->setNetworkParameter(slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_LEFT, delay_str);
        }else{
            throw std::runtime_error("could not get detector");
        }
    }
    //get delay right of module (det_id) in ns
    int getDelayRight(const int det_id){
        auto _d = det.getSlsDetector(det_id);
        if (_d){
            auto r = _d->getNetworkParameter(slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_RIGHT);
            return std::stoi(r);

        }else{
            throw std::runtime_error("could not get detector");
        }
    }
    //set delay right of module (det_id) in ns
    void setDelayRight(const int det_id, const int delay){
        auto delay_str = std::to_string(delay);
        auto _d = det.getSlsDetector(det_id);
        if (_d){
            _d->setNetworkParameter(slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_RIGHT, delay_str);
        }else{
            throw std::runtime_error("could not get detector");
        }
    }


    //Check if detector if filling in gap pixels in module
    //return true if so, currently only in developer
    bool getGapPixels(){
        throw std::runtime_error("gap pixels only in develop");
//        return det.enableGapPixels(-1);
    }


    //Set to true to have the detector filling in gap pixels
    //false to disable, currently only in developer
    void setGapPixels(bool val){
        throw std::runtime_error("gap pixels only in develop");
//        det.enableGapPixels(val);
    }

    slsDetectorDefs::networkParameter networkNameToEnum(std::string par_name);
private:
    multiSlsDetector det;
};


//enum networkParameter {
//  DETECTOR_MAC, 	    	/**< detector MAC */
//  DETECTOR_IP,	 	    	/**< detector IP */
//  RECEIVER_HOSTNAME,  		/**< receiver IP/hostname */
//  RECEIVER_UDP_IP,			/**< receiever UDP IP */
//  RECEIVER_UDP_PORT,		/**< receiever UDP Port */
//  RECEIVER_UDP_MAC,			/**< receiever UDP MAC */
//  RECEIVER_UDP_PORT2,		/**< receiever UDP Port of second half module for eiger */
//  DETECTOR_TXN_DELAY_LEFT, 	/**< transmission delay on the (left) port for next frame */
//  DETECTOR_TXN_DELAY_RIGHT,	/**< transmission delay on the right port for next frame  */
//  DETECTOR_TXN_DELAY_FRAME, /**< transmission delay of a whole frame for all the ports */
//  FLOW_CONTROL_10G,			/**< flow control for 10GbE */
//  FLOW_CONTROL_WR_PTR,		/**< memory write pointer for flow control */
//  FLOW_CONTROL_RD_PTR,		/**< memory read pointer for flow control */
//  RECEIVER_STREAMING_PORT,	/**< receiever streaming TCP(ZMQ) port */
//  CLIENT_STREAMING_PORT,	/**< client streaming TCP(ZMQ) port */
//  RECEIVER_STREAMING_SRC_IP,/**< receiever streaming TCP(ZMQ) ip */
//  CLIENT_STREAMING_SRC_IP	/**< client streaming TCP(ZMQ) ip */
//};

slsDetectorDefs::networkParameter Detector::networkNameToEnum(std::string par_name){

    if(par_name == "detector_mac"){
        return slsDetectorDefs::networkParameter::DETECTOR_MAC;
    }
    else if(par_name == "detector_ip"){
        return slsDetectorDefs::networkParameter::DETECTOR_IP;
    }
    else if(par_name == "rx_hostname"){
        return slsDetectorDefs::networkParameter::RECEIVER_HOSTNAME;
    }
    else if(par_name == "rx_udpip"){
        return slsDetectorDefs::networkParameter::RECEIVER_UDP_IP;
    }
    else if(par_name == "rx_udpport"){
        return slsDetectorDefs::networkParameter::RECEIVER_UDP_PORT;
    }
    else if(par_name == "rx_udpmac"){
        return slsDetectorDefs::networkParameter::RECEIVER_UDP_MAC;
    }
    else if(par_name == "rx_udpport2"){
        return slsDetectorDefs::networkParameter::RECEIVER_UDP_PORT2;
    }
    else if(par_name == "delay_left"){
        return slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_LEFT;
    }
    else if(par_name == "delay_right"){
        return slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_RIGHT;
    }
    else if(par_name == "delay_frame"){
        return slsDetectorDefs::networkParameter::DETECTOR_TXN_DELAY_FRAME;
    }
    else if(par_name == "flow_control_10g"){
        return slsDetectorDefs::networkParameter::FLOW_CONTROL_10G;
    }
    else if(par_name == "client_zmqport"){
        return slsDetectorDefs::networkParameter::CLIENT_STREAMING_PORT;
    }
    else if(par_name == "rx_zmqport"){
        return slsDetectorDefs::networkParameter::RECEIVER_STREAMING_PORT;
    }
    else if(par_name == "rx_zmqip"){
        throw std::runtime_error("rx_zmqip only in developer");
//        return slsDetectorDefs::networkParameter::RECEIVER_STREAMING_SRC_IP;
    }
    
    return slsDetectorDefs::networkParameter::RECEIVER_STREAMING_PORT;
};

slsDetectorDefs::dacIndex Detector::dacNameToEnum(std::string dac_name){
    //to avoid unitialised
    slsDetectorDefs::dacIndex dac = slsDetectorDefs::dacIndex::E_SvP;
    if(dac_name == std::string("vsvp")){
        dac = slsDetectorDefs::dacIndex::E_SvP;
    }
    else if(dac_name == "vtr"){
        dac = slsDetectorDefs::dacIndex::E_Vtr;
    }
    else if(dac_name == "vthreshold"){
        dac = slsDetectorDefs::dacIndex::THRESHOLD;
    }
    else if(dac_name == "vrf"){
        dac = slsDetectorDefs::dacIndex::E_Vrf;
    }
    else if(dac_name == "vrs"){
        dac = slsDetectorDefs::dacIndex::E_Vrs;
    }
    else if(dac_name == "vsvn"){
        dac = slsDetectorDefs::dacIndex::E_SvN;
    }
    else if(dac_name == "vtgstv"){
        dac = slsDetectorDefs::dacIndex::E_Vtgstv;
    }
    else if(dac_name == "vcmp_ll"){
        dac = slsDetectorDefs::dacIndex::E_Vcmp_ll;
    }
    else if(dac_name == "vcmp_lr"){
        dac = slsDetectorDefs::dacIndex::E_Vcmp_lr;
    }
    else if(dac_name == "vcall"){
        dac = slsDetectorDefs::dacIndex::E_cal;
    }
    else if(dac_name == "vcmp_rl"){
        dac = slsDetectorDefs::dacIndex::E_Vcmp_rl;
    }
    else if(dac_name == "rxb_rb"){
        dac = slsDetectorDefs::dacIndex::E_rxb_rb;
    }
    else if(dac_name == "rxb_lb"){
        dac = slsDetectorDefs::dacIndex::E_rxb_lb;
    }
    else if(dac_name == "vcmp_rr"){
        dac = slsDetectorDefs::dacIndex::E_Vcmp_rr;
    }
    else if(dac_name == "vcp"){
        dac = slsDetectorDefs::dacIndex::E_Vcp;
    }
    else if(dac_name == "vcn"){
        dac = slsDetectorDefs::dacIndex::E_Vcn;
    }
    else if(dac_name == "vis"){
        dac = slsDetectorDefs::dacIndex::E_Vis;
    }
    else if(dac_name == "iodelay"){
        dac = slsDetectorDefs::dacIndex::IO_DELAY;
    }
    else if(dac_name == "temp_fpga"){
        dac = slsDetectorDefs::dacIndex::TEMPERATURE_FPGA;
    }
    else if(dac_name == "temp_fpgaext"){
        dac = slsDetectorDefs::dacIndex::TEMPERATURE_FPGAEXT;
    }
    else if(dac_name == "temp_10ge"){
        dac = slsDetectorDefs::dacIndex::TEMPERATURE_10GE;
    }
    else if(dac_name == "temp_dcdc"){
        dac = slsDetectorDefs::dacIndex::TEMPERATURE_DCDC;
    }
    else if(dac_name == "temp_sodl"){
        dac = slsDetectorDefs::dacIndex::TEMPERATURE_SODL;
    }
    else if(dac_name == "temp_sodr"){
        dac = slsDetectorDefs::dacIndex::TEMPERATURE_SODR;
    }
    else if(dac_name == "temp_fpgafl"){
        dac = slsDetectorDefs::dacIndex::TEMPERATURE_FPGA2;
    }
    else if(dac_name == "temp_fpgafr"){
        dac = slsDetectorDefs::dacIndex::TEMPERATURE_FPGA3;
    }
    else if(dac_name == "vhighvoltage"){
        dac = slsDetectorDefs::dacIndex::HV_NEW;
    }
    else if(dac_name == "vb_comp"){
        dac = static_cast<slsDetectorDefs::dacIndex>(0);
    }
    else if(dac_name == "vdd_prot"){
        dac = static_cast<slsDetectorDefs::dacIndex>(1);
    }
    else if(dac_name == "vin_com"){
        dac = static_cast<slsDetectorDefs::dacIndex>(2);
    }
    else if(dac_name == "vref_prech"){
        dac = static_cast<slsDetectorDefs::dacIndex>(3);
    }
    else if(dac_name == "vb_pixbuff"){
        dac = static_cast<slsDetectorDefs::dacIndex>(4);
    }
    else if(dac_name == "vb_ds"){
        dac = static_cast<slsDetectorDefs::dacIndex>(5);
    }
    else if(dac_name == "vref_ds"){
        dac = static_cast<slsDetectorDefs::dacIndex>(6);
    }
    else if(dac_name == "vref_comp"){
        dac = static_cast<slsDetectorDefs::dacIndex>(7);
    }


    return dac;

}


void Detector::pulseAllPixels(int n){
//  int pulsePixelNMove(int n=0,int x=0,int y=0);
//  int pulsePixel(int n=0,int x=0,int y=0);

    for (int j=0; j<8; ++j){
        det.pulsePixel(0, -255+j, 0);
        for (int i=0; i<256; ++i){
            det.pulsePixelNMove(n, 0, 1);
        }
    }
    return;
}
void Detector::pulseDiagonal(int n){
//  int pulsePixelNMove(int n=0,int x=0,int y=0);
//  int pulsePixel(int n=0,int x=0,int y=0);

    for (int j=20; j<232; j+=16){
        det.pulsePixel(0, -255, j);
        for (int i=0; i<8; ++i){
            det.pulsePixelNMove(n, 1, 1);
        }
    }
    return;
}


#endif // DETECTOR_H
