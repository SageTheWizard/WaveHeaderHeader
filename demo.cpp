#include <iostream>
#include <WaveHeader.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Need to specify file path\n";
    }

    WAVE::Header oWaveHeader(*(argv + 1));

    std::cout << "File Valid: " << oWaveHeader.isFileValid() << "\n";
    std::cout << "Run Time: " << oWaveHeader.getRunTime() << "\n";
    std::cout << "Sample Rate: " << oWaveHeader.getSampleRate() << "\n";
    std::cout << "Channel Count: " << oWaveHeader.getChannelCount() << "\n";
    std::cout << "Bits Per Sample: " << oWaveHeader.getBitsPerSample() << "\n";

    return 0;
}