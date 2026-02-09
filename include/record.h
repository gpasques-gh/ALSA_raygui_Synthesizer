#ifndef RECORD_H
#define RECORD_H

#include "defs.h"

typedef struct 
{
    unsigned char ChunkID[4];
    unsigned int ChunkSize;
    unsigned char Format[4];
    unsigned char Subchunk1ID[4];
    unsigned int Subchunk1Size;
    unsigned short AudioFormat;
    unsigned short NumChannels;
    unsigned int SampleRate;
    unsigned int ByteRate;
    unsigned short BlockAlign;
    unsigned short BitsPerSample;
    unsigned char Subchunk2ID[4];
    unsigned int Subchunk2Size;
} wav_header_t;

int init_wav_header(wav_header_t *header);

int init_wav_file(char *fname, FILE **fwav, wav_header_t *header);

int close_wav_file(FILE *fwav);

#endif 