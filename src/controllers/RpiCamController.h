#ifndef RPICAMCONTROLLER_H
#define RPICAMCONTROLLER_H

#include <string>
#include "RpiCamParams.h"

class RpiCamController {
public:
    RpiCamController();
    void startVideo(const RpiCamParams& params);
    void captureJpeg(const RpiCamParams& params);
    void captureStill(const RpiCamParams& params);
    void captureRaw(const RpiCamParams& params);
    void runHello(const RpiCamParams& params);

private:
    void executeCommand(const std::string& command);
};

#endif // RPICAMCONTROLLER_H