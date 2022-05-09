#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include <cctype>
#include <cstring>

#include <vector>


typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;

struct SMFEvent {
	uint32 delta;
	uint8 status;
	uint16 data;

	enum EVENT_TYPE {
		MIDI_2BYTE,
		MIDI_1BYTE,
		SYSEX = 0xf0, 	// System Exclusive
		ESCSYSEX = 0xf7, 	// Escaped System Exclusive
		META = 0xff, 	// Meta
	};

	static uint8 event_type(const uint8 statbyte) {
		uint8 type = 0;
		switch(statbyte & 0xf0) {
		case 0x80:
		case 0x90: // note on
			type = MIDI_2BYTE;
			break;
		case 0xa0:
		case 0xb0: // control change
			type = MIDI_2BYTE;
			break;
		case 0xe0: // pitch bend
			type = MIDI_2BYTE;
			break;
		case 0xc0: // prog. change
			type = MIDI_1BYTE;
			break;
		case 0xd0: // ch. pressure
			type = MIDI_1BYTE;
			break;
		case 0xf0: // Sys Ex | Meta
			switch(statbyte) {
			case 0xf0:
				type = SYSEX;
				break;
			case 0xff:
				type = META;
				break;
			}
			break;
		}
		return type;
	}

	SMFEvent(void) {
		clear();
	}

	void clear() {
		delta = 0;
		status = 0;
	}

	~SMFEvent() {

	}
};

struct SMFChunk {
	uint8  type;
	uint32 length;
	uint16 format, ntracks, division;
	std::vector<SMFEvent> events;

	enum CHUNK_TYPE {
		CHUNK_NONE   = 0,
		CHUNK_MThd = 1,
		CHUNK_MTrk  = 2,
	};

	enum EVENT_TYPE {
		SYSEX = 0xf0, 	// System Exclusive
		ESCSYSEX = 0xf7, 	// Escaped System Exclusive
		META = 0xff, 	// Meta
		MIDI = 0x80, 		// Data
	};

	uint32 get_uint32BE(std::vector<char>::iterator & itr) {
		uint32 res = 0;
		for(uint16 i = 0; i < 4; ++i) {
			res <<= 8;
			res |= uint8(*itr);
			++itr;
		}
		return res;
	}

	uint32 get_uint16BE(std::vector<char>::iterator & itr) {
		uint32 res = *itr;
		++itr;
		res <<= 8;
		res |= *itr;
		++itr;
		return res;
	}

	uint32 get_uint32VL(std::vector<char>::iterator & itr) {
		uint8 b;
		uint32 res = 0;
		for( ; ; ) {
			res <<= 7;
			b = *itr;
			++itr;
			res |= (0x7f & b);
			if ( (b & 0x80) == 0 )
				break;
		}
		return res;
	}

	SMFEvent get_event(std::vector<char>::iterator & itr, uint8 & status) {
		SMFEvent event;
		event.delta = get_uint32VL(itr);
		if (((*itr) & 0x80) != 0) {
			status = *itr;
			++itr;
		}
		event.status = status;
		uint8 type = SMFEvent::event_type(status);
		if ( type == SMFEvent::MIDI_2BYTE ) {
			event.data = get_uint16BE(itr);
		} else if ( type == SMFEvent::MIDI_1BYTE ) {
			event.data = *itr;
			++itr;
		} else if ( type == SMFEvent::SYSEX ) {

		} else if ( type == SMFEvent::META ) {

		} else {

		}
		return event;
	}

	SMFChunk(std::vector<char>::iterator & itr) {
		char t[4];
		for(int i = 0; i < 4; ++i) {
			t[i] = *itr;
			++itr;
		}
		if ( strncmp(t, "MThd", 4) == 0 ) {
			type = CHUNK_MThd;
		} else if ( strncmp(t, "MTrk", 4) == 0 ) {
			type = CHUNK_MTrk;
		} else {
			type = CHUNK_NONE;
		}
		length = get_uint32BE(itr);
		if ( isHeader() ) {
			std::cout << "header" << std::endl;
			format = get_uint16BE(itr);
			ntracks = get_uint16BE(itr);
			division = get_uint16BE(itr);
		} else if ( isTrack() ) {
			std::cout << "track" << std::endl;
			events.clear();
			uint8 laststatus;
			auto itr_end = itr + length;
			while ( itr != itr_end ) {
				events.push_back(get_event(itr, laststatus));
			}
		}
	}

	~SMFChunk() {
		if ( isTrack() ) {
			events.clear();
		}
	}

	void clear(void) {
		format = 0;
		ntracks = 0;
		division = 0;
		events.clear();
	}

	bool isHeader(void) const {
		return type == CHUNK_MThd;
	}

	bool isTrack(void) const {
		return type == CHUNK_MTrk;
	}

	friend std::ostream & operator<<(std::ostream & out, const SMFChunk & chunk) {
		if ( chunk.isHeader() ) {
			out << "Header chunk ";
			out << "(format = " << chunk.format << ", ntracks = " << chunk.ntracks << ", division = " << chunk.division << ") ";
		} else if ( chunk.isTrack() ) {
			out << "Track chunk ";
			out << "(length = " << chunk.length << ") ";
		} else {
			out << "Unknown chunk ";
		}
		return out;
	}
};

/*
bool get_chunk(std::vector<char>::iterator & itr, SMFChunk & chunk) {
	chunk.clear();
	for(int i = 0; i < 4; ++i)
		chunk.ID[i] = *itr;
	chunk.length = get_uint32BE(itr);
	if ( chunk.isHeader() ) {
		chunk.format = get_uint16BE(itr);
		chunk.ntracks = get_uint16BE(itr);
		chunk.division = get_uint16BE(itr);
		return true;
	} else if ( chunk.isTrack() ) {
		std::vector<char>::iterator itr_end = itr + chunk.length;
		chunk.events.clear();
		// parse events
		uint32 b;
		uint8 status = 0;
		while ( itr != itr_end ) {
			b = get_uint32VL(itr);
			chunk.events.push_back(b);
			if (((*itr) & 0x80) != 0) {
				// not the running state
				status = *itr;
				++itr;
			}
			switch(status & 0xf0) {
			case 0x80:
			case 0x90: // note on
			case 0xa0:
			case 0xb0: // control change
			case 0xe0: // pitch bend
				b = *itr;
				++itr;
				b <<= 8;
				b |= *itr;
				++itr;
				chunk.events.push_back(b);
				break;
			case 0xc0: // prog. change
			case 0xd0: // ch. pressure
				b = *itr;
				++itr;
				chunk.events.push_back(b);
				break;
			case 0xf0: // Sys Ex | Meta
				switch(status) {
				case 0xf0: // sys ex
					b = get_uint32VL(itr);
					chunk.events.push_back(*itr);
					++itr;
					for(uint32 i = 0; i < b; ++i) {
						chunk.events.push_back(*itr);
						++itr;
					}
					break;
				case 0xff: // meta
					b = *itr;
					++itr;
					switch(b) {
					case 0x01:
					case 0x02:
					case 0x03:
					case 0x04:
					case 0x05:
					case 0x06:
					case 0x07:
					case 0x7f:
						chunk.events.push_back(get_uint32VL(itr));
						break;
					case 0x2f:
						// 0
						break;
					case 0x51:
						for(uint32 i = 0; i < 3; ++i) {
							chunk.events.push_back(*itr);
							++itr;
						}
						break;
					case 0x54:
						for(uint32 i = 0; i < 5; ++i) {
							chunk.events.push_back(*itr);
							++itr;
						}
						break;
					case 0x58:
						for(uint32 i = 0; i < 4; ++i) {
							chunk.events.push_back(*itr);
							++itr;
						}
						break;
					case 0x59:
						for(uint32 i = 0; i < 3; ++i) {
							chunk.events.push_back(*itr);
							++itr;
						}
						break;
					default:
						// error
						break;
					}
					break;
				}
				break;
			}
		}
		std::cout << "a track finished." << std::endl;
		return true;
	} else {
		return false;
	}
}
*/

int main(int argc, char **argv) {
	std::ifstream ifile;

	std::cout << "file: " << argv[1] << std::endl;
	ifile.open(argv[1], (std::ios::in | std::ios::binary) );
	if ( !ifile ) {
		std::cerr << "失敗" << std::endl;
		return -1;
	}

	std::istreambuf_iterator<char> smfbuf(ifile);
	std::istreambuf_iterator<char> end_smfbuf;
	std::vector<char> smf(smfbuf, end_smfbuf);
	ifile.close();

	std::cout << smf.size() << " bytes." << std::endl;
	std::vector<SMFChunk> midi;
	for(auto itr = smf.begin(); itr != smf.end(); ) {
		midi.push_back(SMFChunk(itr));
		std::cout << midi.back() << std::endl;
	}

	std::cout << "done. " << std::endl;
	return 0;
}
