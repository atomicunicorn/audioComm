import numpy as np
from scipy.io import wavfile
import sys

print " "
if len(sys.argv) != 3:
	print "decode.py takes exactly 3 arguments."
	sys.exit(0)

samps_per_freq = int(sys.argv[2])
rate, data = wavfile.read(sys.argv[1])
dataLength = np.size(data) #audio should be mono
num_characters = float(dataLength) / float(samps_per_freq)
freq_step_size = 9700.0 / 98.0
allowed_error = 1.0 # allowed error in hertz

print "dataLength", dataLength
print "samps_per_freq: ", samps_per_freq
print "num_characters: ", num_characters
print "dataLength / num_characters", dataLength / num_characters
print "rate: ", rate
print " "

# given a clip of the wave file (corresponding to one ascii character),
# get_freq will return the frequency of the sinusoid
def get_freq(clip):
	if len(clip) != samps_per_freq:
		print "clip size not correct for get_freq() function!"
		print "clip size is: ", len(clip)
		return None
	fft_window = np.fft.rfft(clip)
	fft_window = np.absolute(fft_window)
	freq_bins = np.fft.rfftfreq(samps_per_freq, 1.0/float(rate))
	max_amp = float('-inf')
	max_freq_ind = 0
	i = 0
	while i <= samps_per_freq / 2:
		if fft_window[i] > max_amp:
			max_freq_ind = i
			max_amp = fft_window[i]
		i += 1
	max_freq = (float(max_freq_ind) / float(np.size(freq_bins))) * float(rate)
	# print "Maximum frequency is: ", max_freq
	return max_freq

# 0th element holds the freq corresponding to the 0th ascii character
# nth element holds the freq corresponding to the nth ascii character
def build_freq_array():
	fft_window = np.zeros(samps_per_freq)
	freq_array = []
	i = 0
	while i < num_characters:
		start = i * samps_per_freq
		end = (i * samps_per_freq) + samps_per_freq 
		freq = get_freq(data[start: end])
		freq_array.append(freq)
		i += 1
	return freq_array

# takes the calculated frequency, and returns a the closest frequency value out of the accepted 
# frequency values that are used for encoding (i.e. nearest_freq(301.37) == 300)
def nearest_freq(calculated_freq):
	temp_freq = calculated_freq - 300.0
	step_estimate = temp_freq / freq_step_size
	steps = round(step_estimate) # probably ok, but some limitations on the round function. see pydocs
	temp_freq = (freq_step_size * steps) + 300.0
	return temp_freq

def freq_to_ascii(freq):
	num_steps = (freq - 300.0) / freq_step_size
	num_steps = round(num_steps)
	if num_steps < 3:
		if num_steps == 0:
			ascii_val = 9
		if num_steps == 1:
			ascii_val = 10
		if num_steps == 2:
			ascii_val = 11
		return ascii_val
	ascii_val = 32 + num_steps - 3
	return ascii_val

def freq_array_to_ascii_array(freq_array):
	ascii = []
	for elem in freq_array:
		if elem != None:
			ascii.append(freq_to_ascii(elem))
	return ascii

def write_message(ascii_array):
	f = open("message.txt", "w")
	full_string = ""
	for elem in ascii_array:
		full_string += str(unichr(int(elem)))
	f.write(full_string)

test_array = build_freq_array()
i = 0
final_freq_array = []
while i < len(test_array):
	if test_array[i] != None:
		final_freq_array.append(nearest_freq(test_array[i]))
	i += 1
ascii_array = freq_array_to_ascii_array(final_freq_array)
print "len(ascii_array): ", len(ascii_array)
for elem in ascii_array:
	print elem
write_message(ascii_array)
