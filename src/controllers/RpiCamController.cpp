#include "RpiCamController.h"
#include "CommandExecutor.h"
#include <string>

RpiCamController::RpiCamController() {
    // Constructor implementation
}

void RpiCamController::startVideo(const RpiCamParams& params) {
    std::string command = "rpicam-vid --resolution " + params.getResolution() +
                          " --framerate " + std::to_string(params.getFramerate());
    CommandExecutor executor;
    executor.executeCommand(command);
}

void RpiCamController::captureJpeg(const RpiCamParams& params) {
    std::string command = "rpicam-jpeg --resolution " + params.getResolution() +
                          " --quality " + std::to_string(params.getQuality());
    CommandExecutor executor;
    executor.executeCommand(command);
}

void RpiCamController::captureStill(const RpiCamParams& params) {
    std::string command = "rpicam-still --resolution " + params.getResolution();
    CommandExecutor executor;
    executor.executeCommand(command);
}

void RpiCamController::captureRaw(const RpiCamParams& params) {
    std::string command = "rpicam-raw --resolution " + params.getResolution();
    CommandExecutor executor;
    executor.executeCommand(command);
}

void RpiCamController::runHello(const RpiCamParams& params) {
    std::string command = "rpicam-hello";
    CommandExecutor executor;
    executor.executeCommand(command);
}