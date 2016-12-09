#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <math.h>
#include <cstdint>
#include <iostream>
#include <fstream>
#include "WavData.h"
#include <cassert>
#include <stdlib.h>


/**
 * Checks a set of restrictions
 */
bool WavData::checkHeader(const WavHeader& wavHeader) {

	if (0 != strncmp(wavHeader.chunkId, "RIFF", sizeof(wavHeader.chunkId))
			|| 0 != strncmp(wavHeader.format, "WAVE", sizeof(wavHeader.format))) {
		fprintf(stderr, "Invalid RIFF/WAVE format\n");
		return false;
	}

	if (1 != wavHeader.audioFormat) {
		fprintf(stderr, "Invalid WAV format: only PCM audio format is supported\n");
		return false;
	}

	if (wavHeader.numChannels > 2) {
		fprintf(stderr, "Invalid WAV format: only 1 or 2 channels audio is supported\n");
		return false;
	}

	unsigned long bitsPerChannel = wavHeader.bitsPerSample / wavHeader.numChannels;
	if (16 != bitsPerChannel) {
		fprintf(stderr, "Invalid WAV format: only 16-bit per channel is supported\n");
		return false;
	}

	assert(wavHeader.subchunk2Size > 0);
	if (wavHeader.subchunk2Size > LONG_MAX) {
		fprintf(stderr, "File too big\n");
		return false;
	}

	return true;
}


void WavData::readData(std::fstream& fs, const WavHeader& wavHeader, WavData& wavFile) {
	raw_t value, minValue = 0, maxValue = 0;
	int16_t value16, valueLeft16, valueRight16;

	uint32_t bytesPerSample = static_cast<uint32_t>(wavHeader.bitsPerSample / 8);
	unsigned long numberOfSamplesXChannels = wavHeader.subchunk2Size /
			(wavHeader.numChannels * bytesPerSample);

	wavFile.rawData = new raw_t[numberOfSamplesXChannels];

	uint32_t sampleNumber = 0;
	for (; sampleNumber < numberOfSamplesXChannels && !fs.eof(); sampleNumber++) {

		if (1 == wavHeader.numChannels) {
			fs.read((char*)(&value16), sizeof(int16_t));
			value = static_cast<raw_t>(value16);

		} else {
			fs.read((char*)(&valueLeft16), sizeof(int16_t));
			fs.read((char*)(&valueRight16), sizeof(int16_t));
			value = static_cast<raw_t>((abs(valueLeft16) + abs(valueRight16)) / 2);
		}

		if (maxValue < value) {
			maxValue = value;
		}

		if (minValue > value) {
			minValue = value;
		}

		//wavFile.rawData->push_back(value);
		wavFile.rawData[sampleNumber] = value;
	}
	assert(sampleNumber > 0);
	sampleNumber++;

	// Normalization
	wavFile.normalizedData = new double[sampleNumber];
	double maxAbs = std::max(fabs(minValue), fabs(maxValue));

	for (uint32_t i = 0; i < sampleNumber; i++) {
		wavFile.normalizedData[i] = static_cast<double>(wavFile.rawData[i]) / maxAbs;
	}

	// Update values
	wavFile.setMinVal(minValue);
	wavFile.setMaxVal(maxValue);
	wavFile.setNumberOfSamples(sampleNumber);
}

/**
 * Read Wav data from a file
 */
WavData* WavData::readFromFile(const std::string& file) {
	WavHeader wavHeader;

	// Open file
	std::fstream fs;
	fs.open(file.c_str(), std::ios::in | std::ios::binary);

	if (!fs.good()) {
		fprintf(stderr, "Input file not found: %s\n", file.c_str());
		return NULL;
	}

	// Read header
	fs.read((char*)(&wavHeader), sizeof(WavHeader));
	if (!checkHeader(wavHeader)) {
		return NULL;
	}

	// Read raw data
	WavData* wavData = new WavData(wavHeader);
	readData(fs, wavHeader, *wavData);
	fs.close();

	return wavData;
}

