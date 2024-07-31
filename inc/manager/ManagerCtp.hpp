#pragma once

#include "WallStreetSheep/manager/IManager.hpp"

class ManagerCtp final:public IManager{
    virtual void start() override;
};