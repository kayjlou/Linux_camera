#include "LedControl.h"

std::string LedControl::intToHexString(int value) {
    char buffer[3];
    snprintf(buffer, sizeof(buffer), "%02X", value);

    return "0x" + std::string(buffer);
}

void LedControl::setMax() {
    setValue(Constants::MaximumLedValue);
}

void LedControl::setReduced() {
    if (currentValue > Constants::ReducedLedValue) {
        valueBackup = currentValue.load();
        setValue(Constants::ReducedLedValue);
    }
}

void LedControl::switchOff() {
    I2cCommunication::write(I2cCommunication::Command::LedBrightness, "0x00");
}

void LedControl::setValue(int value) {
    auto hex = intToHexString(value);
    I2cCommunication::write(I2cCommunication::Command::LedBrightness, hex);
    currentValue = value;

    #ifdef DEBUG
    int i = currentValue.load();
    printf("LED brightness set to %d\n", i);
    #endif
}

int LedControl::increase() {
    int newValue = currentValue + Constants::LedStep;

    if (newValue <= Constants::MaximumLedValue) {
        setValue(newValue);
    }

    return convertToPercent(currentValue);
}

int LedControl::decrease() {
    int newValue = currentValue - Constants::LedStep;;

    if (newValue >= Constants::MinimumLedValue) {
        setValue(newValue);
    }

    return convertToPercent(currentValue);
}

int LedControl::convertToPercent(int value) {
    return static_cast<int>(static_cast<float>(value) / Constants::MaximumLedValue * 100.0f);
}

void LedControl::applyBackupValue() {
    setValue(valueBackup);
}
