#include <EEPROM.h>

#include "config.h"

// Forward declarations
void __read_from_serial(String prompt, char *ptr, uint8_t len);
void __print_config();
void __read_config_from_serial();
bool __should_setup();
void __do_setup();
void __write_config();

// Static OAuth Configuration Data
struct ConfigData
{

    char tenantId[37];
    char clientId[37];
    char clientSecret[33];
    char resourceId[37];

    /**
     * The typical access token seems to be 1340 characters long
     * We want to have some safety
     */
    char token[1500];
    time_t validUntilEpoch;

} __configData;

boolean config_init()
{
    EEPROM.begin(sizeof(__configData));
    EEPROM.get(0, __configData);
    EEPROM.end();

    __print_config();

    if (__should_setup())
    {
        __do_setup();
    }
    else
    {
        Serial.println("[CONFIG] All options set, no setup requested...");
    }

    return true;
}

String config_get_option(ConfigOption option)
{
    char *value;
    switch (option)
    {
    case TENANT_ID:
        value = &__configData.tenantId[0];
        break;
    case CLIENT_ID:
        value = &__configData.clientId[0];
        break;
    case CLIENT_SECRET:
        value = &__configData.clientSecret[0];
        break;
    case RESOURCE_ID:
        value = &__configData.resourceId[0];
        break;
    case TOKEN:
        value = &__configData.token[0];
        break;

    default:
        break;
    }

    return String(value);
}

TokenData config_get_token()
{
    TokenData data;
    data.token = config_get_option(TOKEN);
    data.validUntilEpoch = __configData.validUntilEpoch;
    return data;
}

void config_update_token(TokenData *newTokenData)
{
    newTokenData->token.toCharArray(__configData.token, sizeof(__configData.token));
    __configData.validUntilEpoch = newTokenData->validUntilEpoch;
    __write_config();
}

void __read_from_serial(String prompt, char *ptr, uint8_t len)
{
    Serial.println("[CONFIG] " + prompt + ":");
    String data;
    data = Serial.readStringUntil('\n');
    data.toCharArray(ptr, len + 1);
}

void __print_config()
{
    Serial.println();
    Serial.println("[CONFIG] config.tenantId:        " + config_get_option(TENANT_ID));
    Serial.println("[CONFIG] config.clientId:        " + config_get_option(CLIENT_ID));
    Serial.println("[CONFIG] config.clientSecret:    " + config_get_option(CLIENT_SECRET));
    Serial.println("[CONFIG] config.resourceId:      " + config_get_option(RESOURCE_ID));
    Serial.println("[CONFIG] config.token:           " + config_get_option(TOKEN));
    Serial.println("[CONFIG] config.validUntilEpoch: " + String(__configData.validUntilEpoch));
    Serial.println();
}

void __read_config_from_serial()
{
    __read_from_serial("Enter TENANT", __configData.tenantId, 36);
    __read_from_serial("Enter CLIENT ID", __configData.clientId, 36);
    __read_from_serial("Enter CLIENT SECRET", __configData.clientSecret, 32);
    __read_from_serial("Enter RESOURCE ID", __configData.resourceId, 36);
}

bool __should_setup()
{
    // check for all options
    bool optionMissing = config_get_option(TENANT_ID).length() != 36 || config_get_option(CLIENT_ID).length() != 36 || config_get_option(CLIENT_SECRET).length() != 32 || config_get_option(RESOURCE_ID).length() != 36;

    // TODO check for "reset" interupt?

    return optionMissing;
}

void __do_setup()
{
    String data;
    Serial.setTimeout(10000);
    Serial.println("[CONFIG] Setup - press any character and Enter to setup (10s)...");
    data = Serial.readStringUntil('\n');

    if (data == "")
    {
        Serial.println("[CONFIG] Skipped setup...");
    }
    else
    {
        __read_config_from_serial();
        __print_config();
    }

    Serial.setTimeout(1000);
}

void __write_config()
{
    EEPROM.begin(sizeof(__configData));
    EEPROM.put(0, __configData);
    EEPROM.end();
    Serial.println("[CONFIG] Wrote settings to EEPROM...");
}
