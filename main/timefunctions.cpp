#include <apps/sntp/sntp.h>
#include "timefunctions.h"
#include <string>

const std::string ntpserver = "pool.ntp.org";


void init_time()
{

    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, (char*)ntpserver.c_str() );
    sntp_init();
}
