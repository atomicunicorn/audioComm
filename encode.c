#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "make_wav.h"

/* sample data in a .wav file is little endian.
   refer to https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
   for information on the endianness of different parts of the
   .wav file header */
void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file) {
	unsigned buf;
	while (num_bytes > 0) {
		buf = word & 0xff;
		fwrite(&buf, 1, 1, wav_file);
		num_bytes--;
	}
	word >>= 8;
}

/* data is the sample data. All the other arguments specify the header information 
   for audioComm, the default sample rate should be 44100 for compatability */
void write_wav(char * filename, unsigned long num_samples, short int * data, int s_rate) {
    FILE* wav_file;
    unsigned int sample_rate;
    unsigned int num_channels;
    unsigned int bytes_per_sample;
    unsigned int byte_rate;
    unsigned long i;    /* counter for samples */
 
    num_channels = 1;   /* default to mono */
    bytes_per_sample = 2;   /* bit depth of 16 */
 
    if (s_rate<=0) sample_rate = 44100;
    else sample_rate = (unsigned int) s_rate;
 
    byte_rate = sample_rate*num_channels*bytes_per_sample;
 
    wav_file = fopen(filename, "w");
 
    /* more info on the data chunks of a .wav file:
	https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
	*/

    /* write RIFF header */
    fwrite("RIFF", 1, 4, wav_file);
    write_little_endian(36 + bytes_per_sample* num_samples*num_channels, 4, wav_file);
    fwrite("WAVE", 1, 4, wav_file);
 
    /* write fmt  subchunk */
    fwrite("fmt ", 1, 4, wav_file);
    write_little_endian(16, 4, wav_file);   /* SubChunk1Size is 16 */
    write_little_endian(1, 2, wav_file);    /* PCM is format 1 */
    write_little_endian(num_channels, 2, wav_file);
    write_little_endian(sample_rate, 4, wav_file);
    write_little_endian(byte_rate, 4, wav_file);
    write_little_endian(num_channels*bytes_per_sample, 2, wav_file);  /* block align */
    write_little_endian(8*bytes_per_sample, 2, wav_file);  /* bits/sample */
 
    /* write data subchunk */
    fwrite("data", 1, 4, wav_file);
    write_little_endian(bytes_per_sample* num_samples*num_channels, 4, wav_file);

    /* write the sample values */
    for (i=0; i< num_samples; i++) {
		write_little_endian((unsigned int)(data[i]),bytes_per_sample, wav_file);
    }
 
    fclose(wav_file);
}

/* returns 1 if file extension is .txt, 0 otherwise*/
unsigned int is_txt_file(const char *fspec) {
    char *e = strrchr (fspec, '.');
    if (e == NULL)
        return 0;
    if ( (e[0] == '.') && (e[1] == 't') && (e[2] == 'x') && (e[3] == 't') ) {
    	if ( e[4] == '\0' ) {
    		return 1;
    	}
    	return 0;
    }
    else {
    	return 0;
    }
}

/* takes the ascii value of a given printable character, and maps to a frequency between
	300 Hz and 10 KHz. can take ASCII values from 32 to 127 */
float ascii_to_freq(char c) {
	float ascii = (float) c;
	printf("ascii is: %f\n", ascii);
	float num_steps;
	float freq_step_size;
	float freq;
	freq_step_size = (float)9700 / (float)95; // divide the frequency space by the number of ascii vals
	num_steps = ascii - (float)32;
	freq = (float)300 + (freq_step_size * num_steps);
	return freq;
}

/* takes a given txt file and returns string */
char * txt_to_string(FILE *file) {
	//FILE* file = fopen(txt_file, "r");
	if (file == NULL) {
		return NULL;
	}

	fseek(file, 0, SEEK_END);
	long int size = ftell(file);
	rewind(file);
	size += 1;

	char* content = calloc((size_t)size, (size_t)1);
	fread(content, 1, size, file);

	return content;
}

/* takes a text file and converts the ASCII values into a wav file where the first 
   sine wav generated / heard corresponds to the first ASCII character in the text
   file. The second sine wav generated / heard corresponds to the second ASCII character
   and so on. */
int main(int argc, char *argv[]) {

	/* first argument is the text file. second arg is the wavfile name */
	/* check command line args */
	if (argc != 3) {
		printf("encode.c requires a .txt file and a .wav argument to write to.\n");
		return 0;
	}
	printf("argv[1] is: %s\n", argv[1]);
	printf("argv[2] is: %s\n", argv[2]);

	FILE * txt;
	char * txt_string;
	txt = fopen(argv[1], "r"); 
	txt_string = argv[1];
	if (is_txt_file(txt_string) == 0) {
		printf("File to be encoded must be a .txt file.\n");
		return 0;
	}

	char * wav_name;
	unsigned long num_samples;
	short int * samples;
	unsigned int sample_rate = 44100;
	float * freq_array;
	unsigned long samps_per_freq;
	unsigned int txt_len;
	char * txt_array;

	/* 
	samps_per_freq is number of samples to be recorded for a 
	given sine wave. this size is determined by accounting for the decoding FFT
	window size which needs to accurately represent the freq range
	we are working with.
	More info: http://support.ircam.fr/docs/AudioSculpt/3.0/co/Window%20Size.html 
	*/

	samps_per_freq = (unsigned long) 11025; // quarter of a second per frequency
	txt_array = txt_to_string(txt);
	txt_len = strlen(txt_array);
	freq_array = calloc(txt_len, sizeof(float));

	for (unsigned int i = 0; i < txt_len; i++) {
		printf("i is: %i\n", i);
		freq_array[i] = ascii_to_freq(txt_array[i]);
		printf("freq_array[i] is: %f\n", freq_array[i]);
	}

}












