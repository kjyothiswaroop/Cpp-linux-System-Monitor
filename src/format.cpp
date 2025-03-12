#include <string>
#include <iomanip>
#include <sstream>
#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) 
{
    long hours = seconds / 3600;
    long minutes = (seconds % 3600) / 60;
    long secs = seconds % 60;
    std::ostringstream timeStr;
    
    timeStr << std::setw(2) << std::setfill('0') << hours << ":";
    timeStr << std::setw(2) << std::setfill('0') << minutes << ":";
    timeStr << std::setw(2) << std::setfill('0') << secs; 
    
    return timeStr.str();
}