#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <ctype.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using namespace std;

// void LinuxParser::getstatus(int pid){
//   string rawdata;
//   ifstream
// }

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel,version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}


// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(),::isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string key,value;
  float total,memtotal,memfree;
  string rawdataline;//to be proccessed.
  ifstream filedata(kProcDirectory+kMeminfoFilename);
  if(filedata.is_open()){
      while (std::getline(filedata,rawdataline))
      {
        istringstream reader(rawdataline);
        while(reader>>key>>value){
          if(key=="MemTotal:"){
            memtotal=stof(value);
          }
          else if(key=="MemFree:"){
            memfree=stof(value);
          }
          
        }

        
      }
    total=memtotal-memfree;

    return total/memtotal;
      

  }
  
  return 0.0; }

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string uptime1,uptime2;
  string line;
  ifstream fileopener(kProcDirectory+kUptimeFilename);
  if (fileopener.is_open()){
    getline(fileopener,line);
    istringstream x(line);
    x>>uptime1>>uptime2;
  }
  return stoi(uptime1); }
long LinuxParser::Jiffies() { return UpTime() * sysconf(_SC_CLK_TCK); }

long LinuxParser::ActiveJiffies(int pid) {
  string line, token;
  vector<string> values;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> token) {
      values.push_back(token);
    }
  }
  long jiffies{0};
  if (values.size() > 21) {
    long user = stol(values[13]);
    long kernel = stol(values[14]);
    long children_user = stol(values[15]);
    long children_kernel = stol(values[16]);
    jiffies = user + kernel + children_user + children_kernel;
  }
  return jiffies;
}
vector<string> LinuxParser::CpuUtilization(){
  string dummy;
  vector<string> values;
  ifstream cpu(kProcDirectory+kStatFilename);
    if(cpu.is_open()){

        getline(cpu,dummy);
        istringstream data(dummy);
        data>>dummy>>values[CPUStates::kUser_]>>values[CPUStates::kNice_]>>values[CPUStates::kSystem_]>>values[CPUStates::kIdle_]>>values[CPUStates::kIOwait_]>>values[CPUStates::kIRQ_]>>values[CPUStates::kSoftIRQ_]>>values[CPUStates::kSteal_]>>values[CPUStates::kGuest_]>>values[CPUStates::kGuestNice_];
        
    }
  return values;     

}
long LinuxParser::ActiveJiffies() {
  vector<string> time = CpuUtilization();
  return (stol(time[CPUStates::kUser_]) + stol(time[CPUStates::kNice_]) +
          stol(time[CPUStates::kSystem_]) + stol(time[CPUStates::kIRQ_]) +
          stol(time[CPUStates::kSoftIRQ_]) + stol(time[CPUStates::kSteal_]) +
          stol(time[CPUStates::kGuest_]) + stol(time[CPUStates::kGuestNice_]));
}

long LinuxParser::IdleJiffies() {
  vector<string> time = CpuUtilization();
  return (stol(time[CPUStates::kIdle_]) + stol(time[CPUStates::kIOwait_]));
}

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) {
  ifstream myfile("/proc/uptime");
  std::map<int,long> stats_processor;
  float uptime,utime,stime,cutime,cstime,starttime;
  string line,dummy;
  if(myfile.is_open()){
    getline(myfile,line);
    istringstream ss(line);
    ss>>dummy;
    uptime=stof(dummy);
    myfile.close();
    ss.clear();
  }
  int x=1;
  ifstream myfile2("/proc/"+to_string(pid)+"/stat");
  if(myfile2.is_open()){
      
    while (getline(myfile2,line))
    {
      /* code */
      istringstream my(line);
      while (my>>dummy)
      {
        if(x==14||x==15||x==16||x==17||x==22){
        stats_processor[x]=stol(dummy);
        
        }
        x++;
      }
      
      my.clear();
    }
    myfile2.close();
      utime=stats_processor.at(14);
      stime=stats_processor.at(15);
      cutime=stats_processor.at(16);
      cstime=stats_processor.at(17);
      starttime=stats_processor.at(22);
    float total_time=utime+stime;
    
    total_time=total_time+cutime+cstime;
    float seconds=uptime-(starttime/sysconf(_SC_CLK_TCK));
  
    float cpu_usage =((total_time / sysconf(_SC_CLK_TCK))/seconds);
    return cpu_usage;
  }
  
  
  
   return 0; }

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key ,value;
  int no_of_process;
  string rawdata;
  ifstream fileopener(kProcDirectory+kStatFilename);
  if (fileopener.is_open()){
    while(getline(fileopener,rawdata)){
      istringstream reader(rawdata);
      while(reader>>key>>value){
        if(key=="processes"){
          no_of_process=stoi(value);
          return no_of_process;
        }
      }
    }
  }
   return 0; }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string key ,value,x;
  int no_of_process;
  string rawdata;
  ifstream fileopener(kProcDirectory+kStatFilename);
  if (fileopener.is_open()){
    while(getline(fileopener,rawdata)){
      istringstream reader(rawdata);
      reader>>key>>value;
        if(key=="procs_running"){
          no_of_process=stoi(value);
          return no_of_process;
        }
      }
    }
  return no_of_process;

 }

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  
  ifstream myfile("/proc/"+to_string(pid)+"/cmdline");
  string line;
  if(myfile.is_open()){

      getline(myfile,line);
      myfile.close();

  }
  
  return line; }

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  
  ifstream myfile("/proc/"+to_string(pid)+"/status");
  string line,key,value;
  if(myfile.is_open()){

      while(getline(myfile,line)){
        istringstream mydata(line);
        mydata>>key>>value;
        if(key=="VmSize:")
        {
          mydata.clear();
          return to_string(stoi(value)/1024);
        }

      }
      myfile.close();
      

  }
  return string(); }

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  ifstream pfiles("/proc/"+to_string(pid)+"/status");
  string user,name,line;
  if(pfiles.is_open()){
    while(getline(pfiles,line)){
      istringstream reader(line);
      reader>>user>>name;
      if(user=="Uid:"){
        pfiles.close();
        reader.clear();
        break;
      }
    }
  }
  
   return name; }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  
  
  ifstream ufiles("/etc/passwd");
  string uname,variable,identifier,line;
  if(ufiles.is_open()){
      while(getline(ufiles,line)){
        std::replace(line.begin(),line.end(),':',' ');
        istringstream reader(line);
        reader>>uname>>variable>>identifier;
        if(identifier==LinuxParser::Uid(pid)){
          
          return uname;
        }
      }
  }
  
  
   return "string()"; }

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  ifstream myfile("/proc/uptime");
  std::map<int,long> stats_processor;
  long uptime,utime,stime,cutime,cstime,starttime;
  string line,dummy;
  if(myfile.is_open()){
    getline(myfile,line);
    istringstream ss(line);
    ss>>dummy;
    uptime=stol(dummy);
    myfile.close();
    ss.clear();
  }
  int x=1;
  ifstream myfile2("/proc/"+to_string(pid)+"/stat");
  if(myfile2.is_open()){
      
    while (getline(myfile2,line))
    {
      /* code */
      istringstream my(line);
      while (my>>dummy)
      {
        if(x==14||x==15||x==16||x==17||x==22){
        stats_processor[x]=stol(dummy);
        
        }
        x++;
      }
      
      my.clear();
    }
    myfile2.close();
      utime=stats_processor.at(14);
      stime=stats_processor.at(15);
      cutime=stats_processor.at(16);
      cstime=stats_processor.at(17);
      starttime=stats_processor.at(22);
    long total_time=utime+stime;
    
    total_time=total_time+cutime+cstime;
    long seconds=uptime-(starttime/sysconf(_SC_CLK_TCK));
  
    return seconds;
  }
  return 0;
 }