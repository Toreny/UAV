/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#ifndef __AP_ARMING_H__
#define __AP_ARMING_H__ 

#include <AP_AHRS/AP_AHRS.h>
#include <AP_HAL/AP_HAL.h>
#include <AP_Param/AP_Param.h>
#include <GCS_MAVLink/GCS_MAVLink.h>

class AP_Arming {
public:
    enum ArmingChecks {
        ARMING_CHECK_NONE       = 0x0000,
        ARMING_CHECK_ALL        = 0x0001,
        ARMING_CHECK_BARO       = 0x0002,
        ARMING_CHECK_COMPASS    = 0x0004,
        ARMING_CHECK_GPS        = 0x0008,
        ARMING_CHECK_INS        = 0x0010,
        ARMING_CHECK_PARAMETERS = 0x0020,
        ARMING_CHECK_RC         = 0x0040,
        ARMING_CHECK_VOLTAGE    = 0x0080,
        ARMING_CHECK_BATTERY    = 0x0100,
        ARMING_CHECK_AIRSPEED   = 0x0200,
        ARMING_CHECK_LOGGING    = 0x0400,
    };

    enum ArmingMethod {
        NONE = 0,
        RUDDER,
        MAVLINK
    };

    enum ArmingRequired {
        NO           = 0,
        YES_MIN_PWM  = 1,
        YES_ZERO_PWM = 2
    };

    enum ArmingRudder {
        ARMING_RUDDER_DISABLED  = 0,
        ARMING_RUDDER_ARMONLY   = 1,
        ARMING_RUDDER_ARMDISARM = 2
    };

    // for the hacky function pointer to gcs_send_text_p
    FUNCTOR_TYPEDEF(gcs_send_t_p, void, MAV_SEVERITY, const prog_char_t *);

    AP_Arming(const AP_AHRS &ahrs_ref, const AP_Baro &baro, Compass &compass,
              const enum HomeState &home_set, gcs_send_t_p);

    ArmingRequired arming_required();
    bool arm(uint8_t method);
    bool disarm();
    bool is_armed();
    ArmingRudder rudder_arming() const { return (ArmingRudder)rudder_arming_value.get(); }
    uint16_t get_enabled_checks();

    /*
      pre_arm_checks() is virtual so it can be modified
      in a vehicle specific subclass
    */
    virtual bool pre_arm_checks(bool report);
    void set_skip_gyro_cal(bool set) { skip_gyro_cal = set; }

    void set_logging_available(bool set) { logging_available = set; }

    //for params
    static const struct AP_Param::GroupInfo        var_info[];

protected:
    bool                                                armed:1;
    bool                                                logging_available:1;
    bool                                                skip_gyro_cal:1;

    //Parameters
    AP_Int8                                           require;
    AP_Int8                               rudder_arming_value;
        //bitmask for which checks are required
    AP_Int16                                checks_to_perform;

    //how the vehicle was armed
    uint8_t                                     arming_method;

    const AP_AHRS                                       &ahrs;
    const AP_Baro                                  &barometer;
    Compass                                         &_compass;
    const enum HomeState                         &home_is_set;
    gcs_send_t_p                              gcs_send_text_P;
    uint32_t                                  last_accel_pass_ms[INS_MAX_INSTANCES];
    uint32_t                                  last_gyro_pass_ms[INS_MAX_INSTANCES];

    void set_enabled_checks(uint16_t);

    bool barometer_checks(bool report);

    bool airspeed_checks(bool report);

    bool logging_checks(bool report);

    bool ins_checks(bool report);

    bool compass_checks(bool report);

    bool gps_checks(bool report);

    bool battery_checks(bool report);

    bool hardware_safety_check(bool report);

    bool manual_transmitter_checks(bool report);
};

#endif //__AP_ARMING_H__
