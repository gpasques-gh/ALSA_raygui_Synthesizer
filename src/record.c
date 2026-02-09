#include "record.h"

#include "defs.h"

int init_wav_header(wav_header_t *header)
{
    header->ChunkID[0] = 'R';
    header->ChunkID[1] = 'I';
    header->ChunkID[2] = 'F';
    header->ChunkID[3] = 'F';

    header->Format[0] = 'W';
    header->Format[1] = 'A';
    header->Format[2] = 'V';
    header->Format[3] = 'E';

    header->Subchunk1ID[0] = 'f';
    header->Subchunk1ID[1] = 'm';
    header->Subchunk1ID[2] = 't';
    header->Subchunk1ID[3] = ' ';

    header->Subchunk2ID[0] = 'd';
    header->Subchunk2ID[1] = 'a';
    header->Subchunk2ID[2] = 't';
    header->Subchunk2ID[3] = 'a';

    header->NumChannels = 1;
    header->BitsPerSample = 16;
    header->Subchunk2Size = 300 * MAX_SAMPLES * (unsigned int) header->NumChannels * (unsigned int) header->BitsPerSample / 8;
    header->ChunkSize = (unsigned int) header->Subchunk2Size + 36;
    header->Subchunk1Size = 16;
    header->AudioFormat = 1;
    header->SampleRate = RATE;
    header->ByteRate = 
        (unsigned int) header->SampleRate *
        (unsigned int) header->NumChannels *
        (unsigned int) header->BitsPerSample / 8;
    header->BlockAlign = (unsigned int) header->NumChannels * (unsigned int) header->BitsPerSample / 8;
        
    return 0;
}

int init_wav_file(char *fname, FILE **fwav, wav_header_t *header)
{
    *fwav = fopen(fname, "wb");

    if (*fwav != NULL)
    {
        fwrite(header, 1, sizeof(*header), *fwav);
    }
    else
    {
        fprintf(stderr, "cannot open wav file to write data\n");
        return 1;
    }
    return 0;
}

int close_wav_file(FILE *fwav)
{
    if (fwav != NULL)
        fclose(fwav);
    else 
    {
        fprintf(stderr, "cannot close wav file\n");
        return 1;
    }
    return 0;
}
