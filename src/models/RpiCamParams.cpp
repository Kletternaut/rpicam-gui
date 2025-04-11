#include "RpiCamParams.h"

// Konstruktor
RpiCamParams::RpiCamParams() : resolution("1920x1080"), framerate(30), quality(75) {}

// Setter-Methoden
void RpiCamParams::setResolution(const std::string& res) {
    resolution = res;
}

void RpiCamParams::setFramerate(int rate) {
    framerate = rate;
}

void RpiCamParams::setQuality(int qual) {
    quality = qual;
}

// Getter-Methoden
std::string RpiCamParams::getResolution() const {
    return resolution;
}

int RpiCamParams::getFramerate() const {
    return framerate;
}

int RpiCamParams::getQuality() const {
    return quality;
}