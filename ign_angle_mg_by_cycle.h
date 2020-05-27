#include <stdint.h>

#ifndef IGN_ANGLE_MG_BY_CYCLE
#define IGN_ANGLE_MG_BY_CYCLE


#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N 16
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N 16

static const uint16_t ign_angle_mg_by_cycle_rpm_scale[IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N] = {
    600,    720,    840,    990,    1170,   1380,   1560,   1950,   2310,   2730,   3210,   3840,   4530,   5370,   6360,   7650
};

#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_MIN ign_angle_mg_by_cycle_rpm_scale[0]
#define IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_MAX ign_angle_mg_by_cycle_rpm_scale[IGN_ANGLE_MG_BY_CYCLE_RPM_SCALE_N-1]-1

static const uint16_t ign_angle_mg_by_cycle_mg_scale[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N] = {
    29,     57,     85,     113,    141,    169,    197,    225,    253,    281,    309,    337,    365,    393,    421,    449
};

#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_MIN ign_angle_mg_by_cycle_mg_scale[0]
#define IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_MAX ign_angle_mg_by_cycle_mg_scale[IGN_ANGLE_MG_BY_CYCLE_MG_SCALE_N-1]-1

#endif /*IGN_ANGLE_MG_BY_CYCLE*/
