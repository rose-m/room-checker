#include <Arduino.h>

#ifndef CONFIG_H_
#define CONFIG_H_

enum ConfigOption
{
    TENANT_ID,
    CLIENT_ID,
    CLIENT_SECRET,
    RESOURCE_ID,
    
    TOKEN
};

/**
 * Initializes the configuration system
 */
boolean config_init();

/**
 * Get the value of a configuration option
 */
String config_get_option(ConfigOption option);

#endif