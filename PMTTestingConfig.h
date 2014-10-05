#ifndef PMTTESTINGCONFIG_H
#define PMTTESTINGCONFIG_H
#include <map>
#include <vector>

struct LEDAmp
{
    float highV;
    int frq;
    float width;
    int trigDelay;
    float trigWidth;
};

typedef std::vector<LEDAmp> LEDPulserConfig;
typedef std::map<float,LEDPulserConfig> PMTTestingConfig;
#endif // PMTTESTINGCONFIG_H
