Currently in development, audioComm is a tool which can send and receive audio information through chat applications such as skype and facebook video chat. audioComm encodes ASCII values as different frequency values which can be sent over a computer's microphone. This can be useful for communicating sensitive information without typing said sensitive information. Ideal for discretely sharing a short message or public key to someone through an innocuous service such as skype.

WARNING: audioComm is not intended for encryption! What audioComm can do is send already encrypted data in disguise as a non-suspicious piece of data (i.e. share a PGP encrypted message with someone as a .wav file). Use audioComm on unencrypted data at your own risk!

To use:

1) download all files into a working directory, make

2) $ ./encode [txt file] [wav filename]

Now the text file should be encoded as a wav file in your working directory. At this point, you can send the .wav file to anyone. Some testing/tweaking still needs to be done on the playback of the .wav file through skype, but anyone with the original .wav file and the decode.py file should be able to decode the .wav file back into its original text file.

To decode the .wav file run:

$ python decode.py [wavfile] 2048

