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

struct TokenData
{
    String token;
    time_t validUntilEpoch;
};

/**
 * Initializes the configuration system
 */
boolean config_init();

/**
 * Get the value of a configuration option
 */
String config_get_option(ConfigOption option);

/**
 * Get the stored token TokenData
 */
TokenData config_get_token();

/**
 * Save the current token and remember access time
 */
void config_update_token(TokenData* newTokenData);

#endif