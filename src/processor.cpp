#include "processor.h"
#include "linux_parser.h"

using namespace LinuxParser;
using namespace std;
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    
     std::string user,nice,system,idle,iowait,irq,softirq,steal,guest,guestnice,dummy;
     float totaltime,totalidealtime;
    ifstream cpu(kProcDirectory+kStatFilename);
    if(cpu.is_open()){

        getline(cpu,dummy);
        istringstream data(dummy);
        data>>dummy>>user>>nice>>system>>idle>>iowait>>irq>>softirq>>steal>>guest>>guestnice;
        totalidealtime=stof(idle)+stof(iowait);
        totaltime=totalidealtime+stof(user)+stof(nice)+stof(system)+stof(irq)+stof(softirq)+stof(guest)+stof(guestnice);

        
    
    }
    
     return (totaltime-totalidealtime)/totalidealtime; }