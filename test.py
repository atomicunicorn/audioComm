import sys
import os

def tester(orig_txt_file, decod_txt_file):
	fo = open(orig_txt_file, "r")
	fd = open(decod_txt_file, "r")
	o_text = fo.read()
	d_text = fd.read()
	if len(o_text) != len(d_text):
		print "text files not of same size"
		print "original length: ", len(o_text)
		print "decoded length: ", len(d_text)
		return False
	i = 0 
	while i < len(o_text):
		if o_text[i] != d_text[i]:
			print "incorrect ascii at index {0}".format(i)
			return False
		i += 1
	print "all tests passed"
	return True

#creates a .txt file holding the string, also encodes the .txt file as a wav
# then tests the decoding of the file
def create_and_test(string, textfile):
	f = open(textfile, "w")
	f.write(string)
	f.close()
	fileroot = textfile[0:len(textfile) - 3]
	wavfile = fileroot + "wav"
	print "textfile is: ", textfile
	print "wavfile is: ", wavfile
	os.system("./encode {0} {1}".format(textfile, wavfile))
	os.system("python decode.py {0} {1}".format(wavfile, 2048))
	tester(textfile, "message.txt") 
	os.system("rm {0}*".format(fileroot))


def main():
	create_and_test("I shat myself", "shit.txt")
	create_and_test("", "empty.txt")
	create_and_test("hello world!", "hello.txt")
	create_and_test("""



		""", "newline.txt")
	create_and_test("""-----BEGIN PGP PUBLIC KEY BLOCK-----
Version: 2.6.i

mQCNAi+UeBsAAAEEAMP0kXU75GQdzwwlMiwZBUKFUDRgR4wH9y5NP9JaZfVX8shT
ESbCTbGSEExB2ktEPh5//dkfftsKNlzlAugKFKTgBv20tQ9lDKfdbPVR0HmTLz0e
wVIeqEue4+Mt/Kq7kMcQy+5sX2RBAiZTYl0n/JdY/WxflU0taq1kH/UUPkklAAUR
tB5NYXJ0eSBNY0ZseSA8bWFydHlAZnV0dXJlLmNvbT6JAJUCBRAvlHhGrWQf9RQ+
SSUBAQX+BACnhx7OTb1SfAcJVF/1kuRPUWuGcl57eZgv4syc1O9T3YNr0terWQBT
K0vFR00FdaBv9X9XwlxaBJHGeiBcmhaiOTwB912ysoteUzZHne3sHPw3MkYboAFx
xHg43Cnj60OeZG2PKp/kU91ipOJP1cs8/xYOGkeoAMqDfwPeFlkBiA==
=ddBN
-----END PGP PUBLIC KEY BLOCK-----
%""", "pgp.txt")

main()