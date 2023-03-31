/*
MIT License

Copyright (c) 2023 SageTheWizard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <fstream>
#include <string>
#include <filesystem>
#include <string.h>

namespace WAVE
{
    class Header
    {
    public:
        Header(std::string strFileName)
        {
            std::ifstream oFileHandler;
            m_oFileSizeOnDisk = std::filesystem::file_size(strFileName);
            oFileHandler.open(strFileName, std::ios::binary);

            // Validate File is large  enough to atleast contain WAVE Header
            oFileHandler.seekg(std::ios::end);
            m_oWaveHeaderUncorrupted = oFileHandler.tellg() >= m_oHeaderSize;
            if (m_oWaveHeaderUncorrupted)
            {
                oFileHandler.close();
                return;
            }
            oFileHandler.seekg(std::ios::beg);

            // Copy Header into struct
            oFileHandler.read(reinterpret_cast<char *>(&m_sHeader), sizeof(SWAVEHeader));

            oFileHandler.close();
        }
        ~Header()
        {
        }

        bool isFileValid()
        {
            if (!m_oWaveHeaderUncorrupted)
            {
                return false;
            }

            bool isValid = true;

            // Verify Strings in the header
            isValid &= strcmp(m_sHeader.RIFF, "RIFF") == 0;
            isValid &= strcmp(m_sHeader.DATA, "data") == 0;
            isValid &= strcmp(m_sHeader.FMT, "fmt\0") == 0;
        }

    private:
        bool m_oWaveHeaderUncorrupted = false;
        const uint32_t m_oHeaderSize = 44;

        typedef struct SWAVEHeader
        {
            char RIFF[4];
            uint32_t nFileSize;
            char WAVE[4];
            char FMT[4];
            uint32_t nBitDepth;
            uint8_t byFormat;
            uint8_t nChannels;
            uint32_t nSampleRate;
            uint32_t nSoundByteSize;
            uint8_t nBytesPerSample;
            uint8_t nBitsPerSample;
            char DATA[4];
            uint32_t nFileDataSize;
        } __attribute__((__packed__));

        SWAVEHeader m_sHeader;
        uint64_t m_oFileSizeOnDisk;
    };
}