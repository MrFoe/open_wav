#ifndef WAVDATA_H_INCLUDED
#define WAVDATA_H_INCLUDED


// Структура, описывающая заголовок WAV файла.
struct WavHeader
{
    // WAV-формат начинается с RIFF-заголовка:

    // Содержит символы "RIFF" в ASCII кодировке
    char chunkId[4];

    unsigned long chunkSize;

    // Содержит символы "WAVE"
    char format[4];

    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:
    // Содержит символы "fmt "
    char subchunk1Id[4];

    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    unsigned long subchunk1Size;

    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    unsigned short audioFormat;

    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    unsigned short numChannels;

    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    unsigned long sampleRate;

    // sampleRate * numChannels * bitsPerSample/8
    unsigned long byteRate;

    // numChannels * bitsPerSample/8
    // Количество байт для одного сэмпла, включая все каналы.
    unsigned short blockAlign;

    // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
    unsigned short bitsPerSample;

    // Подцепочка "data" содержит аудио-данные и их размер.

    // Содержит символы "data"
    // (0x64617461 в big-endian представлении)
    char subchunk2Id[4];

    // numSamples * numChannels * bitsPerSample/8
    // Количество байт в области данных.
    unsigned long subchunk2Size;

    // Далее следуют непосредственно Wav данные.
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
