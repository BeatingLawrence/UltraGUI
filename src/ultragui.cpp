#include "ultragui.h"

#if ((LIBRARY_VERSION_TWEAK != 0) && (LIBRARY_VERSION_TWEAK != 1) && (LIBRARY_VERSION_TWEAK != 99))
#error "WRONG VERSION TYPE"
#endif

using namespace gui;

//=============================================================================
std::string UltraGui::strPrint(std::string format, ...)
{
    std::string ret;
    va_list testList;
    va_list list;
    va_start(testList, format);
    va_copy(list, testList);
    char garbage;
    int required = vsnprintf(&garbage, 0, format.c_str(), testList);

    if (required > 0)
    {
        ret.resize(required);
        vsnprintf(&ret[0], required + 1, format.c_str(), list);
    }

    va_end(testList);
    va_end(list);
    return ret;
}
//=============================================================================
std::string UltraGui::substrUntil(const std::string& str, const std::string& token)
{
    auto found = str.find(token);

    if (found == std::string::npos) return str;

    return str.substr(0, found);
}
//=============================================================================
std::string UltraGui::substrFrom(const std::string& str, const std::string& token)
{
    auto found = str.find(token);

    if (found == std::string::npos) return "";

    return str.substr(found + token.size());
}
//=============================================================================
UltraGui::dual UltraGui::split(const std::string& str, const std::string& token)
{
    return {substrUntil(str, token), substrFrom(str, token)};
}
//=============================================================================
QVector<UltraEntry> UltraGui::createUltraEntryVector(QVector<QString> strings)
{
    QVector<UltraEntry> toReturn;
    int32_t i = 0;

    for (auto& el : strings) toReturn.push_back({el.toStdString().c_str(), i++});

    return toReturn;
}
//=============================================================================
QColor UltraGui::transient(const QColor& first, const QColor& second, uint8_t selector)
{
    float mR  = second.redF() - first.redF();
    float mG  = second.greenF() - first.greenF();
    float mB  = second.blueF() - first.blueF();
    float mA  = second.alphaF() - first.alphaF();
    uint8_t R = ((float)selector * mR) + first.red();
    uint8_t G = ((float)selector * mG) + first.green();
    uint8_t B = ((float)selector * mB) + first.blue();
    uint8_t A = ((float)selector * mA) + first.alpha();
    return QColor(R, G, B, A);
}
//=============================================================================
QColor UltraGui::faded(const QColor& color, float alpha)
{
    QColor ret(color);
    ret.setAlphaF(alpha);
    return ret;
}
//=============================================================================
UltraGUIVersion UltraGui::version()
{
    UltraGUIVersion ret;
    std::string ver = LIBRARY_VERSION;
    std::string typ;

    auto splitted = split(ver, ".");
    ret.major     = std::stoll(splitted.left);
    splitted      = split(splitted.right, ".");
    ret.minor     = std::stoll(splitted.left);
    splitted      = split(splitted.right, ".");
    ret.patch     = std::stoll(splitted.left);

    switch (std::stoll(splitted.right))
    {
        case 0:
            ret.type = UltraGUIVersion::Alpha;
            typ      = "a";
            break;

        case 1:
            ret.type = UltraGUIVersion::Beta;
            typ      = "b";
            break;

        case 99:
            ret.type = UltraGUIVersion::Release;
            break;

        default:
            throw "Version type mismatch";
    }

    ret.text = strPrint("%u.%u.%u%s", ret.major, ret.minor, ret.patch, typ.c_str());

    return ret;
}
//=============================================================================
