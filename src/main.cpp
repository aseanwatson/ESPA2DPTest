#include <Arduino.h>
#include "AudioConfig.h"
#include "BluetoothA2DPSource.h"
//#include "esp_spiffs.h"
#include "SPIFFS.h"
#include <vector>

BluetoothA2DPSource a2dp_source;

class FileOneChannelSoundData : public OneChannelSoundData
{
public:
    FileOneChannelSoundData(bool loop = false, ChannelInfo channelInfo = Both)
        : OneChannelSoundData(loop, channelInfo)
    {
        unload();
    }

    void load(fs::File& file)
    {
        // unload in case resize moves buffer
        unload();

        file.seek(0);

        // make _data bigger (if needed) to accomidate file
        _data.resize(file.size());

        // read the data, capturing the actual number of bytes read
        size_t len = file.read(_data.data(), _data.size());

        // make sure the data have the size of data read
        _data.resize(len);

        // update the base class with the dynamic data
        setDataRaw(_data.data(), len);
    }

    void unload()
    {
        setDataRaw(NULL, 0);
    }

private:
    std::vector<uint8_t> _data;
};

FileOneChannelSoundData soundData(true);

void loadSamples(const char* filename)
{
    fs::File file = SPIFFS.open(filename);
    soundData.load(file);
    file.close();
}

void setup()
{
    SPIFFS.begin();
    loadSamples("/hello.raw");

    a2dp_source.start("Zone Wireless"); /* Zone Wireless == name of my headphones*/
    a2dp_source.write_data(&soundData);
}

void loop()
{
    delay(100);
}