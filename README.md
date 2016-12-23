# MIDIBINZ #
### Created by Gabe Lamorie, Josh Eugenio, Lindley Cannon and Tom Matthias ###
### For James McTigue in DAT450 at Cogswell Polytechnical College ###

MIDIBinz is an audio application that will take in a real-time audio signal and convert the digital information to MIDI information, which will then be sent over a network to control parameters of any Virtual Instrument using MIDI Standards.  

The process of the application is as follows:
There will be 4 band pass filters, which will have a set frequency bandwidth, which will only allow a certain range of frequencies through.  When the application is in record mode, an audio input stream will fill the buffer.  When the input buffer is filled the 4 filter instances will be called to separate the input signal into 4 separate containers.  A find maximum function will be call on each bin, assigning each maximum value of each container, to a variable.  These four new values will then be scaled to a range 0-127(the original value will be between 0 and 1). The values will then be assigned to user-defined parameters on the GUI.  Once parameters are assigned, and application is in record mode, the application will send four Midi Control Change (CC) values between 0 and 127, over the network..  These MIDI control change values will then be used to control parameters in an MIDI based Virtual Instrument.
