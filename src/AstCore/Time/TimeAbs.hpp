#pragma once
#include "AstCore/AstCoreExport.h"

class ATime
{
public:
    ATime()
    {

    }
    double epoch() const{return m_epoch;}
    double second() const{return m_second;}

protected:
    double m_epoch;
    double m_second;
};

