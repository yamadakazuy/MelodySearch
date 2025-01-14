/*
 * smf.h
 *
 *  Created on: 2022/05/10
 *      Author: sin
 *
 *      Ver. 20221017.02
 */

#ifndef SMF_H_
#define SMF_H_

#include <iostream>
#include <iomanip>
#include <cinttypes>
#include <vector>

namespace smf {

uint32_t get_uint32BE(std::istreambuf_iterator<char> & itr);
uint32_t get_uint16BE(std::istreambuf_iterator<char> & itr);
uint32_t get_uint32VLQ(std::istreambuf_iterator<char> & itr);
bool check_str(const std::string & str, std::istreambuf_iterator<char> & itr);

int octave(uint8_t notenum);
const char * notename(uint8_t notenum);

constexpr uint32_t INT_MTrk = 0x4d54726b;
constexpr uint32_t INT_MThd = 0x4d546864;
constexpr uint32_t INT_XFIH = 0x58464948;
constexpr uint32_t INT_XFKM = 0x58464b4d;
constexpr uint32_t INT_Cont = 0x436f6e74;

enum EVENT_TYPE {
	MIDI_NOTEOFF = 0x80,
	MIDI_NOTEON = 0x90,
	MIDI_POLYKEYPRESSURE = 0xa0,
	MIDI_CONTROLCHANGE = 0xb0,
	MIDI_PROGRAMCHANGE = 0xc0,
	MIDI_CHPRESSURE = 0xd0,
	MIDI_PITCHBEND = 0xe0,
	SYSTEM = 0xf0,
	SYS_EX = 0xf0, 	// System Exclusive
	SYS_SONGPOS = 0xf2,
	SYS_SONGSEL= 0xf3,
	SYS_TUNEREQ = 0xf6,
	SYS_ENDOFEX = 0xf7,
	SYS_TIMING_CLOCK = 0xf8,
	SYS_TIMING_START = 0xfa,
	SYS_TIMING_CONT = 0xfb,
	SYS_TIMIMG_STOP = 0xfc,
	SYS_ACTIVE_SENSING = 0xfe,
	SYS_RESET = 0xff,
	ESCSYSEX = 0xf7, 	// Escaped System Exclusive
	META = 0xff, 	// Meta
};


static constexpr char * namesofnote[] = {
		(char *) "C",
		(char *) "C#",
		(char *) "D",
		(char *) "D#",
		(char *) "E",
		(char *) "F",
		(char *) "F#",
		(char *) "G",
		(char *) "G#",
		(char *) "A",
		(char *) "A#",
		(char *) "B",
		(char *) "",
};

struct event {
	uint32_t delta;
	uint8_t  status;
	std::string data;


	static int octave(uint8_t notenum) {
		return (notenum / 12) - 1;
	}

	static const char * notename(uint8_t notenum) {
		return smf::namesofnote[notenum % 12];
	}

	event(void) : delta(0), status(0), data() {}

	event(std::istreambuf_iterator<char> & itr, uint8_t laststatus);

	void read(std::istreambuf_iterator<char> & itr, uint8_t laststatus = 0);

	void clear() {
		delta = 0;
		status = 0;
		data.clear();
	}

	~event() {
		data.clear();
	}

	bool isMeta(void) const {
		return status == META;
	}

	bool isEoT(void) const {
		return isMeta() && data[0] == 0x2f;
	}

	bool isTempo() const {
		return isMeta() && (data[0] == 0x51);
	}

	uint32_t tempo() const {
		if ( isTempo() )
			return 0;
		uint32_t tmp = 0;
		tmp = uint8_t(data[1]);
		tmp <<= 8;
		tmp |= uint8_t(data[2]);
		tmp <<= 8;
		tmp |= uint8_t(data[3]);
		return tmp;
	}

	bool isMIDI() const {
		uint8_t msb4 = (status & 0xf0)>>4;
		return (msb4 >= 8) and (msb4 <= 14);
	}

	bool isNote() const {
		if ( (status & 0xe0) == 0x80 ) {
			return true;
		}
		return false;
	}

	bool isNoteOn() const {
		if ( (status & 0xf0) == smf::MIDI_NOTEON ) {
			return data[1] > 0;
		}
		return false;
	}

	bool isNoteOff() const {
		return (status & 0xf0) == smf::MIDI_NOTEOFF;
	}

	int channel(void) const {
		return 0x0f & status;
	}

	int octave() const {
		if ( isNote() )
			return smf::event::octave(data[0]);
		return -2;
	}

	const uint32_t & deltaTime() const {
		return delta;
	}

	const char * notename() const {
		if ( !isNote() )
			return namesofnote[12];
		return smf::event::notename(data[0]);
	}

	int notenumber() const {
		if ( !isNote() )
			return 128;
		return int(data[0]);
	}

	std::ostream & printOn(std::ostream & out) const;

	std::ostream & printData(std::ostream & out, uint32_t offset = 1) const {
		for(auto i = data.cbegin() + offset; i != data.cend(); ++i) {
			out << *i;
		}
		return out;
	}

	friend std::ostream & operator<<(std::ostream & out, const event & evt) {
		return evt.printOn(out);
	}

};


struct note {
	uint32_t time;
	uint8_t channel;
	uint8_t number;
	uint32_t duration;

	note(uint32_t t, const smf::event & e, uint32_t d = 0) : time(t), channel(e.channel()), number(e.notenumber()), duration(d) { }

	friend std::ostream & operator<<(std::ostream & out, const note & n) {
		out << "[";
		out << std::dec << n.time << ", " << int(n.channel) << ".";
		out << smf::event::notename(n.number) << smf::event::octave(n.number) << ", " << n.duration;
		out << "]";
		return out;
	}
};

class MIDI {
	uint16_t _format, ntracks, _division;
	std::vector<std::vector<smf::event>> _tracks;

public:
	MIDI() :  _format(0), ntracks(0), _division(0), _tracks() {}
	MIDI(std::istream & smffile);
	/*
	score(std::istream & smffile) {
		std::istreambuf_iterator<char> itr(smffile);
		std::istreambuf_iterator<char> end_itr;

		if ( check_str("MThd", itr) ) {
			get_uint32BE(itr);
			// The header length is always 6.
			smfformat = get_uint16BE(itr);
			ntracks = get_uint16BE(itr);
			division = get_uint16BE(itr);
		} else {
			smfformat = 0;
			ntracks = 0;
			division = 0;
			tracks.clear();
			return;
		}
		while (itr != end_itr) {
			if ( check_str("MTrk", itr) ) {
				get_uint32BE(itr);
				//std::cout << "track" << std::endl;
				tracks.push_back(std::vector<event>());
				uint8_t laststatus = 0;
				do {
					event ev(itr, laststatus);
					//std::cout << ev << std::endl;
					laststatus = ev.status;
					tracks.back().push_back(ev);
				} while ( !tracks.back().back().isEoT() );

			}
		}
	}
	*/

	void clear() {
		for(unsigned int i = 0; i < _tracks.size(); ++i) {
			_tracks[i].clear();
		}
		_tracks.clear();
		_format = 0, ntracks = 0, _division = 0;
	}

	bool is_empty(void) const {
		return  ntracks == 0;
	}

	uint16_t format() const {
		return _format;
	}

	bool isSMPTE() const {
		return (_division & (uint16_t(1)<<15)) != 0;
	}

	uint16_t resolution() const {
		if ( ! isSMPTE() ) {
			return _division;
		} else {
			uint16_t smpte = -char(_division>>8);
			uint16_t tpf = _division & 0xff;
			return smpte * tpf;
		}
	}

	uint16_t noftracks() const {
		return ntracks;
	}

	const std::vector<smf::event> & track(int i) const {
		return _tracks[i];
	}

	std::vector<smf::note> score() const;

	std::ostream & header_info(std::ostream & out) const;

	friend std::ostream & operator<<(std::ostream & out, const MIDI & midi) {
		out << "smf";
		out << "(header: format = " << std::dec << midi.format() << ", ntracks = " << midi.noftracks() << ", resolution = " << midi.resolution() << ") ";
		/*
		for(uint16_t i = 0; i < midi.noftracks() ; ++i) {
			out << std::endl << "track " << std::dec << i << ": ";
			for(auto e = midi.tracks[i].cbegin(); e != midi.tracks[i].end() ; ++e) {
				out << *e;
			}

		}
		*/
		out << std::endl;
		return out;
	}
};

}

#endif /* SMF_H_ */
