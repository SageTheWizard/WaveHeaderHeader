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
            m_nFileSizeOnDisk = std::filesystem::file_size(strFileName);
            oFileHandler.open(strFileName, std::ifstream::binary | std::ifstream::ate);

            // Validate File is large  enough to atleast contain WAVE Header
            uint32_t nSize = oFileHandler.tellg();
            m_oWaveHeaderUncorrupted = nSize >= m_cnHeaderSize;
            if (!m_oWaveHeaderUncorrupted)
            {
                oFileHandler.close();
                return;
            }
            oFileHandler.seekg(std::ifstream::beg);

            // Copy Header into struct
            oFileHandler.read(reinterpret_cast<char *>(&m_sHeader), sizeof(SWAVEHeader));

            oFileHandler.close();
        }
        ~Header()
        {
        }

        bool isFileValid()
        {
            int asdf = sizeof(m_sHeader);
            if (!m_oWaveHeaderUncorrupted)
            {
                return false;
            }

            bool isValid = true;

            // Verify Format vales in the header
            isValid &= m_sHeader.nFormatLength == 16;
            isValid &= strncmp(m_sHeader.RIFF, m_csRIFF, 4) == 0;
            isValid &= strncmp(m_sHeader.DATA, m_csDATA, 4) == 0;
            isValid &= strncmp(m_sHeader.FMT, m_csFMT, 4) == 0;

            // Verify is PCM
            // TODO: Make configurable for other types?
            isValid &= m_sHeader.nFormat == 1;

            // Check File Size is Consistent
            isValid &= (m_cnHeaderSize + m_sHeader.nFileDataSize == m_nFileSizeOnDisk);

            // Verify Sample Rate / Bits Per Sample / Channel Count is consistent
            isValid &= (m_sHeader.nSampleRate * m_sHeader.nChannels * m_sHeader.nBytesPerSample) ==
                       (m_sHeader.nSoundByteSize);

            return isValid;
        }

        double getRunTime()
        {
            return (m_sHeader.nFileDataSize * 1.0) / m_sHeader.nSoundByteSize;
        }

        int32_t getSampleRate()
        {
            return m_sHeader.nSampleRate;
        }

        int16_t getChannelCount()
        {
            return m_sHeader.nChannels;
        }

        int16_t getBitsPerSample()
        {
            return m_sHeader.nBitsPerSample;
        }

        bool isPCM()
        {
            return true; // See Above TODO
        }

    private:
        bool m_oWaveHeaderUncorrupted = false;
        const uint32_t m_cnHeaderSize = 44;
        const char m_csRIFF[4] = {'R', 'I', 'F', 'F'};
        const char m_csWAVE[4] = {'W', 'A', 'V', 'E'};
        const char m_csFMT[4] = {'f', 'm', 't', ' '};
        const char m_csDATA[4] = {'d', 'a', 't', 'a'};

        // https://docs.fileformat.com/audio/wav/
        struct SWAVEHeader
        {
            char RIFF[4];             // 1  - 4
            uint32_t nFileSize;       // 5  - 8
            char WAVE[4];             // 9  - 12
            char FMT[4];              // 13 - 16
            uint32_t nFormatLength;   // 17 - 20
            uint16_t nFormat;         // 21 - 22
            uint16_t nChannels;       // 23 - 24
            uint32_t nSampleRate;     // 25 - 28
            uint32_t nSoundByteSize;  // 29 - 32
            uint16_t nBytesPerSample; // 33 - 34
            uint16_t nBitsPerSample;  // 35 - 36
            char DATA[4];             // 37 - 40
            uint32_t nFileDataSize;   // 41 - 44
        } __attribute__((__packed__));

        SWAVEHeader m_sHeader;
        uint64_t m_nFileSizeOnDisk;
    };
}