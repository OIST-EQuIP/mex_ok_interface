#include <math.h>
#include <matrix.h>
#include <mex.h>
#include <iostream>
#include <string.h>
#include "okFrontPanelDLL.h"

okCFrontPanel *xem = NULL;
OpalKelly::FrontPanelDevices devices;
OpalKelly::FrontPanelPtr devptr;
bool isopen = false;
bool file_exists(const char * filename);


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]){
    /* commands
     *  1, open ... mex_ok_interface('open') 
     *  2, configure ... mex_ok_interface('configure', <bitfilename>)
     *  3, setWireIn or swi ... mex_ok_interface('setWireIn', <ep>, <value>)
     *  4, updateWireIns or uwi ... mex_ok_interface('updateWireIns')
     *  5, getWireOutValue or gwo... mex_ok_interface('getWireOutValue', <ep>)
     *  6, updateWireOuts or uwo... mex_ok_interface('updateWireOuts')
     *  7, activateTriggerIn or ati ... mex_ok_interface('activateTriggerIn', <ep>, <bit>)
     */
    char cmd[100];
    mxGetString(prhs[0], cmd, 100); //read the first input argument
    printf("command = %s\n", cmd);
    if(strcmpi(cmd, "open") == 0){
        printf("Opening FPGA.\n");
        int count;
        std::string serial;
        std::string devID;
        
        count =  devices.GetCount();
        printf("Number of devices found: %d\n", count);
        devptr = devices.Open();
        xem = devptr.get();
        if (!xem) {
            printf("Opening failed.\n");
            return;
        }
        okTDeviceInfo  m_devInfo;
        xem->GetDeviceInfo(&m_devInfo);
        printf("Found a device: %s\n", m_devInfo.productName);
        printf("Device firmware version: %d.%d\n", m_devInfo.deviceMajorVersion, m_devInfo.deviceMinorVersion);
        printf("Device serial number: %s\n", m_devInfo.serialNumber);
        printf("Device device ID: %d\n", m_devInfo.productID);
        xem->LoadDefaultPLLConfiguration();
        isopen = true;
        
    } else if (strcmpi(cmd, "configure") == 0){
        printf("Configuring FPGA.\n");
        if (!isopen){
            printf("FPGA is not open!\n");
            return;
        }
        if (!xem){
            printf("Null pointer!\n");
            return;
        }
        char bitfile[100];
        mxGetString(prhs[1], bitfile, 100);
        // check if file exists
        if (file_exists(bitfile)){
            std::string filename(bitfile);
            if (okCFrontPanel::NoError != xem->ConfigureFPGA(filename)) {
                printf("Loading failed.\n");
                return;
            } else {
                printf("Bit file %s loaded.\n", bitfile);
            }
        } else {
            printf("Bit file %s does not exist.\n", bitfile);
            return;
        }
        
    } else if (strcmpi(cmd, "setWireIn") == 0 || strcmpi(cmd, "SWI") == 0){ 
        if (!isopen) {
            printf("FPGA is not open!\n");
            return;
        }
        int ep = mxGetScalar(prhs[1]);
        int val = mxGetScalar(prhs[2]);
        if (okCFrontPanel::NoError != xem->SetWireInValue(ep, val)) {
            printf("Wrinting to ep %d failed!\n", ep);
            return;
        }
        
    } else if (strcmpi(cmd, "updateWireIns") == 0 || strcmpi(cmd, "UWI") == 0){
        if (!isopen) {
            printf("FPGA is not open!\n");
            return;
        }
        xem->UpdateWireIns();
        
    } else if (strcmpi(cmd, "getWireOutValue") == 0 || strcmpi(cmd, "GWO") == 0){
        if (!isopen) {
            printf("FPGA is not open!\n");
            return;
        }
        int ep = mxGetScalar(prhs[1]);
        int val = xem->GetWireOutValue(ep);
        plhs[0] = mxCreateDoubleScalar(val);
        
    } else if (strcmpi(cmd, "updateWireOuts") == 0 || strcmpi(cmd, "UWO") == 0){
        if (!isopen) {
            printf("FPGA is not open!\n");
            return;
        }
        xem->UpdateWireOuts();
        
    } else if (strcmpi(cmd, "activateTriggerIn") == 0 || strcmpi(cmd, "ATI") == 0) {
        if (!isopen) {
            printf("FPGA is not open!\n");
            return;
        }
        int ep = mxGetScalar(prhs[1]);
        int bit = mxGetScalar(prhs[2]);
        
        if (okCFrontPanel::NoError != xem->ActivateTriggerIn(ep, bit)) {
            printf("Activating trigger bit %d at ep %d failed!\n", bit, ep);
            return;
        }
    
    } else if (strcmpi(cmd, "close") == 0){
        if (!isopen) {
            printf("FPGA is not open!\n");
            return;
        }
        printf("Closing FPGA.\n");
        xem->Close();
        isopen = false;
        
    } else {        
        printf("Unknown command: %s.\n", cmd);
    }
    
	return;
	
}

bool file_exists(const char * filename){
    if (FILE *file = fopen(filename, "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }
}