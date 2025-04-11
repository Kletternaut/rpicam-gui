#ifndef RPICAMPARAMS_H
#define RPICAMPARAMS_H

#include <string>

class RpiCamParams {
public:
    RpiCamParams(); // Konstruktor

    // Getter-Methoden
    std::string getResolution() const;
    int getFramerate() const;
    int getQuality() const;

    // Setter-Methoden
    void setResolution(const std::string& res);
    void setFramerate(int rate);
    void setQuality(int qual);

private:
    std::string resolution;
    int framerate;
    int quality;
};

#endif // RPICAMPARAMS_H