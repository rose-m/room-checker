#include <EEPROM.h>

#include "config.h"

// Forward declarations
void __read_from_serial(String prompt, char *ptr, uint8_t len);
void __print_config();
void __read_config_from_serial();

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

} __configData;

boolean config_init()
{
    EEPROM.begin(sizeof(__configData));
    EEPROM.get(0, __configData);

    __print_config();

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

        EEPROM.put(0, __configData);
        Serial.println("[CONFIG] Wrote settings to EEPROM...");
    }
    Serial.setTimeout(1000);

    EEPROM.end();

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
    Serial.println("[CONFIG] config.tenantId:     " + String(__configData.tenantId));
    Serial.println("[CONFIG] config.clientId:     " + String(__configData.clientId));
    Serial.println("[CONFIG] config.clientSecret: " + String(__configData.clientSecret));
    Serial.println("[CONFIG] config.resourceId:   " + String(__configData.resourceId));
    Serial.println("[CONFIG] config.token:        " + String(__configData.token));
    Serial.println();
}

void __read_config_from_serial()
{
    __read_from_serial("Enter TENANT", __configData.tenantId, 36);
    __read_from_serial("Enter CLIENT ID", __configData.clientId, 36);
    __read_from_serial("Enter CLIENT SECRET", __configData.clientSecret, 32);
    __read_from_serial("Enter RESOURCE ID", __configData.resourceId, 36);
}