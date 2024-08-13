#pragma once

#include <string>

struct Tick{
    std::string InstrumentId;
    int64_t TS;
    double Price;
    double Open;
    double Highest;
    double Lowest;
    double Volume;
    double Hold;
};

struct Bar{

};