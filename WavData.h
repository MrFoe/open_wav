#ifndef WAVDATA_H_INCLUDED
#define WAVDATA_H_INCLUDED


// ���������, ����������� ��������� WAV �����.
struct WavHeader
{
    // WAV-������ ���������� � RIFF-���������:

    // �������� ������� "RIFF" � ASCII ���������
    char chunkId[4];

    unsigned long chunkSize;

    // �������� ������� "WAVE"
    char format[4];

    // ������ "WAVE" ������� �� ���� ����������: "fmt " � "data":
    // ���������� "fmt " ��������� ������ �������� ������:
    // �������� ������� "fmt "
    char subchunk1Id[4];

    // 16 ��� ������� PCM.
    // ��� ���������� ������ ����������, ������� � ���� �������.
    unsigned long subchunk1Size;

    // ��� PCM = 1 (�� ����, �������� �����������).
    // ��������, ������������ �� 1, ���������� ��������� ������ ������.
    unsigned short audioFormat;

    // ���������� �������. ���� = 1, ������ = 2 � �.�.
    unsigned short numChannels;

    // ������� �������������. 8000 ��, 44100 �� � �.�.
    unsigned long sampleRate;

    // sampleRate * numChannels * bitsPerSample/8
    unsigned long byteRate;

    // numChannels * bitsPerSample/8
    // ���������� ���� ��� ������ ������, ������� ��� ������.
    unsigned short blockAlign;

    // ��� ���������� "��������" ��� �������� ��������. 8 ���, 16 ��� � �.�.
    unsigned short bitsPerSample;

    // ���������� "data" �������� �����-������ � �� ������.

    // �������� ������� "data"
    // (0x64617461 � big-endian �������������)
    char subchunk2Id[4];

    // numSamples * numChannels * bitsPerSample/8
    // ���������� ���� � ������� ������.
    unsigned long subchunk2Size;

    // ����� ������� ��������������� Wav ������.
};

class WavData{

public:


    ~WavData(){
        if(this->rawData != NULL)
        {
            delete [] this->rawData;
        }
        if(this->normalizedData != NULL)
        {
            delete [] this->normalizedData;
        }
    };

    static WavData* readFromFile(const std::string& file);

    uint32_t getNumberOfSamples() const {return numberOfSamples;}
    void setNumberOfSamples(uint32_t& numberOfSamples) {this->numberOfSamples = numberOfSamples;}

    uint16_t getMaxVal() const { return maxVal; }
	void setMaxVal(uint16_t maxVal) { this->maxVal = maxVal; }

	uint16_t getMinVal() const { return minVal; }
	void setMinVal(uint16_t minVal) { this->minVal = minVal; }

	const WavHeader& getHeader() const { return header; }
	const uint16_t* getRawData() const { return rawData; }
	const double* getNormalizedData() const { return normalizedData; }

private:
    WavHeader header;
    uint16_t* rawData;
    double* normalizedData;


    uint16_t maxVal;
    uint16_t minVal;
    uint32_t numberOfSamples;

    WavData(WavHeader& header)
    {
        this->header = header;
        this->rawData = NULL;
        this->normalizedData = NULL;

        this->maxVal = 0;
        this->minVal = 0;
        this->numberOfSamples = 0;
    }

    static bool checkHeader(const WavHeader& wavHeader);
    static void readData(std::fstream& fs, const WavHeader& wavHeader, WavData& wavFile);

};

#endif // WAVDATA_H_INCLUDED
