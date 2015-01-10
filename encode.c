#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>

/* sample data in a .wav file is little endian.
   refer to https://ccrma.stanford.edu/courses/422/projects/WaveFormat/
   for information on the endianness of different parts of the
   .wav file header */
void write_little_endian(unsigned int word, int num_bytes, FILE *wav_file) {
    unsigned buf;
    while(num_bytes>0)
    {   buf = word & 0xff;
        fwrite(&buf, 1,1, wav_file);
        num_bytes--;
    word >>= 8;
    }
}

/* data is the sample data. All the other arguments specify the header information 
   for audioComm, the default sample rate should be 44100 for compatability */
void write_wav(char * filename, unsigned long num_samples, short int * data, unsigned int s_rate) {
    FILE* wav_file;
    unsigned int sample_rate;
    unsigned int num_channels;
    unsigned int bytes_per_sample;
    unsigned int byte_rate;
    unsigned long i;    /* counter for samples */
 
    num_channels = 1;   /* default to mono */
    bytes_per_sample = 2;   /* bit depth of 16 */
    sample_rate = s_rate;
    //printf("sample_rate %i\n", sample_rate);
    byte_rate = sample_rate*num_channels*bytes_per_sample;
 
    wav_file = fopen(filename, "w");
 
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
    write_little_endian(bytes_per_sample* num_samples * num_channels, 4, wav_file);
    //printf("bytes in wav file: %lu\n", bytes_per_sample* num_samples * num_channels);

    /* write the sample values */
    //printf("num_samples called in write_wav: %lu\n", num_samples);
    for (i=0; i< num_samples; i++) {
		write_little_endian((unsigned int)(data[i]),bytes_per_sample, wav_file);
    }
 
    fclose(wav_file);
}

/* returns 1 if file extension is .txt, 0 otherwise */
unsigned int is_txt_file(const char *fspec) {
    char *e = strrchr (fspec, '.');
    if (e == NULL) {
        return 0;
    }
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

/* returns 1 if file extension is .wav, 0 otherwise */
unsigned int is_wav_file(const char *fspec) {
	char *e = strrchr (fspec, '.');
	if (e == NULL) {
		return 0;
	}
	if ( (e[0] == '.') && (e[1] == 'w') && (e[2] == 'a') && (e[3] == 'v') ) {
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
	300 Hz and 10 KHz. can take ASCII values from 32 to 127 along with 
	control ascii vals 9(tab), 10(line feed), 11(vertical tab) */
float ascii_to_freq(char c) {
	float ascii = (float) c;
	//printf("ascii is: %f\n", ascii);
	float num_steps;
	float freq_step_size;
	float freq;
	freq_step_size = (float)9700 / (float)98; // divide the frequency space by the number of ascii vals
	if (ascii < (float)32) {
		if (ascii == (float)9 ) { num_steps = (float)0; }
		if (ascii == (float)10) { num_steps = (float)1; }
		if (ascii == (float)11) { num_steps = (float)2; }
	}
	else {
		num_steps = ascii - (float)32 + 3;
	}
	freq = (float)300 + (freq_step_size * num_steps);
	//printf("freq is: %f\n", freq);
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
	//printf("argv[1] is: %s\n", argv[1]);
	//printf("argv[2] is: %s\n", argv[2]);

	FILE * txt;
	char * txt_string;
	txt = fopen(argv[1], "r"); 
	txt_string = argv[1];
	if (is_txt_file(txt_string) == 0) {
		printf("File to be encoded must be a .txt file.\n");
		return 0;
	}
	if (is_wav_file(argv[2]) == 0) {
		printf("File audio is written to must be a .wav file.\n");
		return 0;
	}


	char * wav_name;
	unsigned long num_samples;
	unsigned int sample_rate = 44100;
	float * freq_array;
	float * full_freq_array;
	unsigned long samps_per_freq;
	unsigned int txt_len;
	char * txt_array;
	float freq_radians_per_sample;

	/* 
	samps_per_freq is number of samples to be recorded for a 
	given sine wave. this size is determined by accounting for the decoding FFT
	window size which needs to accurately represent the freq range
	we are working with.
	More info: http://support.ircam.fr/docs/AudioSculpt/3.0/co/Window%20Size.html 
	*/

	samps_per_freq = (unsigned long)2048; 
	samps_per_freq = samps_per_freq / (unsigned long) 2; // required since there is some strangeness going on in write_wav
	txt_array = txt_to_string(txt);
	txt_len = strlen(txt_array);
	freq_array = calloc(txt_len, sizeof(float));
	num_samples = samps_per_freq * (unsigned long)txt_len;
	int *buffer;
	buffer = calloc(num_samples, sizeof(int));
	full_freq_array = calloc(num_samples, sizeof(float));

	printf("\n");

	/* build freq_array of frequencies corresponding to the ascii vals in txt file */
	for (unsigned int i = 0; i < txt_len; i++) {
		freq_array[i] = ascii_to_freq(txt_array[i]);
	}

	/* write the buffer, k corresponds to index in freq_array and i to samples
	   of specified ascii character*/
	unsigned int buffer_index = 0;
	float phase;
	float amplitude;
	unsigned int k = 0;
	unsigned int i = 0;
	for (k = 0; k < txt_len; k++) {
		freq_radians_per_sample = freq_array[k] * 2 * M_PI/sample_rate;
		phase  = 0;
		amplitude = 32000;
		for (i = 0; i < samps_per_freq; i++) {
			phase += freq_radians_per_sample;
			buffer[buffer_index] = (int)(amplitude * sin(phase));
			buffer_index += 1;
		} 
	}

	write_wav(argv[2], 2 * num_samples, buffer, sample_rate);
	//printf("wrote wav file\n");
	//printf("length in samples: %lu\n", num_samples * 2);
	//printf("length in seconds: %lu\n", num_samples * 2 / sample_rate);

	return 1;
}












