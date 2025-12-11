#define STR(X) #X
#ifdef AST_PROPERTIES_MARK
#   define AST_PROPERTIES AST_PROPERTIES_MARK public
#else
#   define AST_PROPERTIES public
#endif
#define properties AST_PROPERTIES
#include <string>

typedef double length_d, mass_d, time_d, area_d, speed_d, force_d, energy_d, power_d, angle_d, angvel_d; 
typedef int err_t;

class Test{
properties:
    // 标记下面都是properties
    time_d    m_timestamp;        
    angle_d   m_heading;          
    length_d  m_distance;
    
    time_d time1() const{return m_time1;}
    
    angle_d angle1() const{return m_angle1;}
    void setAngle1(angle_d value){m_angle1 = value;}
    
    angle_d angle2() const{return m_angle2;}
    err_t setAngle2(angle_d value){m_angle2 = value; return 0;}

    int valueInt() const{return m_valueInt;}
    const std::string& valueString() const{return m_valueString;}
    void setValueString(StringView s){m_valueString = std::string(s);}
    bool      m_valueBool;
protected:
    time_d    m_time1;    
    angle_d   m_angle1;
    angle_d   m_angle2;
    int       m_valueInt;
    std::string m_valueString;
};


void Test_ClassInit(Class* type)
{

    constexpr auto member_timestamp = &Test::m_timestamp;
    type->addProperty("timestamp", aNewProperty<Test, member_timestamp>());

    constexpr auto member_heading = &Test::m_heading;
    type->addProperty("heading", aNewProperty<Test, member_heading>());

    constexpr auto member_distance = &Test::m_distance;
    type->addProperty("distance", aNewProperty<Test, member_distance>());

    type->addProperty("time1", aNewProperty<Test, &Test::time1>());

    type->addProperty("angle1", aNewProperty<Test, &Test::angle1, &Test::setAngle1>());

    type->addProperty("angle2", aNewProperty<Test, &Test::angle2, &Test::setAngle2>());

    type->addProperty("valueInt", aNewProperty<Test, &Test::valueInt>());

    type->addProperty("valueString", aNewProperty<Test, &Test::valueString, &Test::setValueString>());

    constexpr auto member_valueBool = &Test::m_valueBool;
    type->addProperty("valueBool", aNewProperty<Test, member_valueBool>());

}