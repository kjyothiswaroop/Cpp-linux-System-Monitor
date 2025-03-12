#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string os, kernel, version;
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
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() 
{ 
  std::string line, key;
  float value,memTotal, memFree;
  std::ifstream memoryFile(kProcDirectory + kMeminfoFilename);
  if(memoryFile.is_open())
  {
    while(std::getline(memoryFile, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "MemTotal:")
      {
         memTotal = value;
      }
      else if(key == "MemFree:")
      {
          memFree = value;
      }
    }
    return (memTotal - memFree) / memTotal;
  }
  return 0.0; 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  std::string line;
  long value = 0;
  std::ifstream uptimeFile(kProcDirectory + kUptimeFilename);
  if(uptimeFile.is_open())
  {
    std::getline(uptimeFile, line);
    std::istringstream linestream(line);
    linestream >> value;
    
  }
  return value; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{ 
  std::string line, key;
  std::vector< string > arr;
  std::ifstream cpuFile(kProcDirectory + kStatFilename);
  if(cpuFile.is_open())
  {
    std::getline(cpuFile, line);
    std::istringstream linestream(line);
    linestream >> key;
    while(linestream >> key)
    {
      arr.push_back(key);
    }
  }
  long totalJiffies = 0;
  for(std::string i : arr)
  {
    totalJiffies += std::stol(i);
  }
  return totalJiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) 
{ 
  std::string line, value;
  long utime, stime, cutime, cstime;
  std::ifstream statFile(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(statFile.is_open())
  {
    std::getline(statFile, line);
    std::istringstream linestream(line);
    for(int i = 0; i < 13; i++)
    {
      linestream >> value;
    }
    linestream >> utime >> stime >> cutime >> cstime;
    return utime + stime + cutime + cstime;
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() 
{ 
  std::string line, key;
  std::vector< string > arr;
  std::ifstream cpuFile(kProcDirectory + kStatFilename);
  if(cpuFile.is_open())
  {
    std::getline(cpuFile, line);
    std::istringstream linestream(line);
    linestream >> key;
    for(int i = 0; i < 10; i++)
    {
      linestream >> key;
      arr.push_back(key);
    }
  }
  long jiffies{0};
  for(std::string a:arr)
  {
    jiffies += std::stol(a);
  }
  long idlejiffies = std::stol(arr[3]) + std::stol(arr[4]);
  long  activejiffies = jiffies - idlejiffies;
  return activejiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{ 
  return Jiffies() - ActiveJiffies();
}

// TODO: Read and return CPU utilization
float LinuxParser::CpuUtilization() 
{ 
  return (float(LinuxParser::ActiveJiffies())/float(LinuxParser::Jiffies()));
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{
  std::string line,key;
  int value{0};
  std::ifstream processFile(kProcDirectory + kStatFilename);
  if(processFile.is_open())
  {
    while(std::getline(processFile, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "processes")
      {
        return value;
      }
    }
  }
  return 0; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{
  std::string line,key;
  int value{0};
  std::ifstream processFile(kProcDirectory + kStatFilename);
  if(processFile.is_open())
  {
    while(std::getline(processFile, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "procs_running")
      {
        return value;
      }
    }
  }
   return 0; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) 
{ 
  std::string line,key;
  std::ifstream cmdFile(kProcDirectory + std::to_string(pid) + kCmdlineFilename); 
  if(cmdFile.is_open())
  {
    std::getline(cmdFile, line);
    return line;
  }
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) 
{ 
  std::string line,key;
  int value{0};

  std::ifstream statusFile(kProcDirectory + std::to_string(pid) + kStatusFilename); 
  if(statusFile.is_open())
  {
    while(std::getline(statusFile, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "VmSize:")
      {
        return std::to_string(value / 1024);
      }
    }
  }
  return ""; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) 
{
  std::string line,key;
  int value{0};

  std::ifstream statusFile(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(statusFile.is_open())
  {
    while(std::getline(statusFile, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if(key == "Uid:")
      {
        return std::to_string(value);
      }
    }
  }
   return ""; 
  
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) 
{
  std::string line,key, x;
  int value{0};
  std::string uid = Uid(pid);
  std::ifstream passwdFile(kPasswordPath);
  if(passwdFile.is_open()) 
  {
    while(std::getline(passwdFile, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), '-', '_');
      std::istringstream linestream(line);
      linestream >> key >> x >> value;
      if(std::to_string(value) == uid)
      {
        std::replace(key.begin(), key.end(), '_', '-');
        return key;
      }
    }
  }
   return "";
  
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) 
{ 
  std::string line;
  long value{0}; 
  std::ifstream statFile(kProcDirectory + std::to_string(pid) + kStatFilename);
  if(statFile.is_open())
  {
    std::getline(statFile, line);
    std::istringstream linestream(line);
    for(int i = 0; i < 21; i++)
    {
      linestream >> value;
    }
    long upTime = UpTime()- value / sysconf(_SC_CLK_TCK);
    return upTime;
  }
  return 0; 
}
