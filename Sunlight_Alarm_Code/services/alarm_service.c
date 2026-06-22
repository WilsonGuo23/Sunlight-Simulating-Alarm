#include "alarm_service.h"

#include "time_service.h"
#include "triac_driver.h"
#include "stdio.h"

/* =========================
 * Alarm state (internal)
 * ========================= */
static uint8_t alarm_hour_;
static uint8_t alarm_minute_;

/* Alarm enable flag:
 * controls whether alarm comparison is active
 */
static bool alarm_enabled_;

/* =========================================================
 * Initialize alarm service state
 * Default: 00:00, enabled
 * ========================================================= */
void alarm_service_init(void)
{
    alarm_hour_ = 0;
    alarm_minute_ = 0;

    alarm_enabled_ = true;
}

/* =========================================================
 * Set alarm time (safe-guarded to valid ranges)
 * hour:   0–23
 * minute: 0–59
 * ========================================================= */
void alarm_service_set_alarm(uint8_t hour,
                             uint8_t minute)
{
    alarm_hour_ = hour % 24;
    alarm_minute_ = minute % 60;
}

/* =========================================================
 * Get current alarm time
 * Uses pointer outputs (optional params supported)
 * ========================================================= */
void alarm_service_get_alarm(uint8_t *hour,
                             uint8_t *minute)
{
    if (hour)
    {
        *hour = alarm_hour_;
    }

    if (minute)
    {
        *minute = alarm_minute_;
    }
}

/* =========================================================
 * Enable or disable alarm triggering
 * ========================================================= */
void alarm_service_enable(bool enable)
{
    alarm_enabled_ = enable;
}

/* =========================================================
 * Query alarm enable state
 * ========================================================= */
bool alarm_service_is_enabled(void)
{
    return alarm_enabled_;
}

/* =========================================================
 * Main alarm check function (called periodically)
 *
 * Compares:
 *   - current system time
 *   - configured alarm time
 *
 * If match occurs → triggers sunrise sequence
 *
 * Note:
 *   - Does NOT block
 *   - Must be called from scheduler/task loop
 * ========================================================= */
void alarm_service_update(void)
{
    uint8_t current_hour;
    uint8_t current_minute;

    /* Check if sunrise animation is already running */
    bool sunrise_active_ = sunrise_service_is_active();

    /* Skip alarm logic if disabled or already active */
    if ((!alarm_enabled_) || sunrise_active_)
    {
        return;
    }

    /* Fetch current time from RTC/time service */
    time_service_get(
        &current_hour,
        &current_minute);

    /* Trigger condition: exact match */
    if ((current_hour == alarm_hour_) &&
        (current_minute == alarm_minute_))
    {
        sunrise_service_start();
    }
}

/* =========================================================
 * Increment alarm hour (wrap at 24)
 * ========================================================= */
void alarm_service_increment_hour(void)
{
    alarm_hour_++;

    if (alarm_hour_ >= 24)
    {
        alarm_hour_ = 0;
    }
}

/* =========================================================
 * Increment alarm minute (wrap at 60)
 * ========================================================= */
void alarm_service_increment_minute(void)
{
    alarm_minute_++;

    if (alarm_minute_ >= 60)
    {
        alarm_minute_ = 0;
    }
}

/* =========================================================
 * Decrement alarm hour (wrap around 23 → 0)
 * ========================================================= */
void alarm_service_decrement_hour(void)
{
    if (alarm_hour_ <= 0)
    {
        alarm_hour_ = 23;
    }
    else
    {
        alarm_hour_--;
    }
}

/* =========================================================
 * Decrement alarm minute (wrap around 59 → 0)
 * ========================================================= */
void alarm_service_decrement_minute(void)
{
    if (alarm_minute_ <= 0)
    {
        alarm_minute_ = 59;
    }
    else
    {
        alarm_minute_--;
    }
}

/* =========================================================
 * Unified increment interface
 * hour_mode:
 *   true  → adjust hours
 *   false → adjust minutes
 * ========================================================= */
void alarm_service_increment(bool hour_mode)
{
    if (hour_mode)
    {
        alarm_service_increment_hour();
    }
    else
    {
        alarm_service_increment_minute();
    }
}

/* =========================================================
 * Unified decrement interface
 * hour_mode:
 *   true  → adjust hours
 *   false → adjust minutes
 * ========================================================= */
void alarm_service_decrement(bool hour_mode)
{
    if (hour_mode)
    {
        alarm_service_decrement_hour();
    }
    else
    {
        alarm_service_decrement_minute();
    }
}