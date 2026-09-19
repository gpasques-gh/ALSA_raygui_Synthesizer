import rtmidi

midiout = rtmidi.MidiOut()
ports = midiout.get_ports()
print(ports)

midiout.open_port(5)  # index en dur, pour éliminer toute erreur de parsing
print("port ouvert")

midiout.send_message([0x90, 60, 112])
print("note envoyee")
