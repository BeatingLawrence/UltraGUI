#include "./ultragui.h"
#include <cstdlib>

using namespace hci;

UltraGui::Version_t UltraGui::version()
{
    Version_t version;
    version.maj = atoi(VERSION_MAJ);
    version.min = atoi(VERSION_MIN);
    version.pat = atoi(VERSION_PAT);
    version.completeText = QString("UltraGUI-%1.%2.%3%4").arg(VERSION_MAJ, VERSION_MIN, VERSION_PAT, VERSION_SUF);
    return version;
}

