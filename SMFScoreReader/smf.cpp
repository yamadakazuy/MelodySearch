/*
 * smf.cpp
 *
 *  Created on: 2022/05/10
 *      Author: sin
 *
 *      Ver. 20221017.02
 */

#include <iostream>
#include <iomanip>
#include <deque>

#include "smf.h"

// read 4 bytes to get a 32 bit value in the big endian byte order
uint32_t smf::get_uint32BE(std::istreambuf_iterator<char> & itr) {
	uint32_t res = 0;
	for(uint16_t i = 0; i < 4; ++i) {
		res <<= 8;
		res |= uint8_t(*itr);
		++itr;
	}
	return res;
}

// read 2 bytes to get a 16 bit value in the big endian byte order
uint32_t smf::get_uint16BE(std::istreambuf_iterator<char> & itr) {
	uint32_t res = *itr;
	++itr;
	res <<= 8;
	res |= *itr;
	++itr;
	return res;
}

uint32_t smf::get_uint32VLQ(std::istreambuf_iterator<char> & itr) {
	uint8_t b;
	uint32_t res = 0;
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

bool smf::check_str(const std::string & str, std::istreambuf_iterator<char> & itr) {
		bool res = true;
		std::cerr << "check_str: ";
		for(auto i = str.begin(); i != str.end(); ++i, ++itr) {
			std::cerr << *itr;
			res &= (*i == *itr);
		}
		std::cerr << std::endl;
		return res;
//	return std::equal(str.begin(), str.end(), itr);
}


smf::event::event(std::istreambuf_iterator<char> & itr, uint8_t laststatus) {
	delta = get_uint32VLQ(itr);
	status = laststatus;
	if (((*itr) & 0x80) != 0) {
		status = *itr;
		++itr;
	}
	uint8_t type;
	uint32_t len;
	type = status & 0xf0;
	if ( (smf::MIDI_NOTEOFF <= type && type <= smf::MIDI_CONTROLCHANGE) || (type == smf::MIDI_PITCHBEND) ) {
		data.push_back(*itr);
		++itr;
		data.push_back(*itr);
		++itr;
	} else if ( type == smf::MIDI_PROGRAMCHANGE || type == smf::MIDI_CHPRESSURE ) {
		data.push_back(*itr);
		++itr;
	} else if ( status == smf::SYS_EX ) {
		len = get_uint32VLQ(itr);
		for(uint32_t i = 0; i < len; ++i) {
			data.push_back(*itr);
			++itr;
		}
	} else if ( status == smf::ESCSYSEX ) {
		len = get_uint32VLQ(itr);
		for(uint32_t i = 0; i < len; ++i) {
			data.push_back(*itr);
			++itr;
		}
	} else if ( status == smf::META ) {
		data.push_back(*itr); // function
		++itr;
		len = get_uint32VLQ(itr);
		for(uint32_t i = 0; i < len; ++i) {
			data.push_back(*itr);
			++itr;
		}
	} else {
		std::cerr << "error!" << std::dec << delta << std::hex << status << std::endl;
		// error.
	}
}


void smf::event::read(std::istreambuf_iterator<char> & itr, uint8_t laststatus) {
	std::istreambuf_iterator<char> end_itr;
	delta = get_uint32VLQ(itr);
	status = laststatus;
	bool not_running_status = false;
	if (((*itr) & 0x80) != 0) {
		not_running_status = true;
		status = *itr;
		++itr;
	} /*
	else {
		std::cout << "running status" << std::endl;
	}
	*/
	uint32_t len;
	uint8_t type = status & 0xf0;
	if ( (smf::MIDI_NOTEOFF <= type && type <= smf::MIDI_CONTROLCHANGE) || (type == smf::MIDI_PITCHBEND) ) {
		data.push_back(*itr);
		++itr;
		data.push_back(*itr);
		++itr;
	} else if ( type == smf::MIDI_PROGRAMCHANGE || type == smf::MIDI_CHPRESSURE ) {
		data.push_back(*itr);
		++itr;
	} else if ( type == smf::SYSTEM ) {
		if ( status == smf::SYS_EX ) {
			len = get_uint32VLQ(itr);
			for(uint32_t i = 0; i < len; ++i) {
				data.push_back(*itr);
				++itr;
			}
			//std::cerr << "sys_ex (" << data.size() << ") " << std::endl;
		}else if ( status == smf::ESCSYSEX ) {
			len = get_uint32VLQ(itr);
			for(uint32_t i = 0; i < len; ++i) {
				data.push_back(*itr);
				++itr;
			}
			//std::cerr << "escsysex (" << data.size() << ") " << std::endl;
		} else if ( status == smf::META ) {
			data.push_back(*itr); // function
			++itr;
			len = get_uint32VLQ(itr);
			//if (len == 6208) {
			//	std::cerr << "caution!" << std::endl;
			//}
			for(uint32_t i = 0; i < len; ++i) {
				data.push_back(*itr);
				++itr;
				if ( itr == end_itr ) {
					break;
				}
			}
			//std::cerr << "meta (" << data.size() << ") " << std::endl;
		} else if (status == smf::SYS_SONGPOS ) {
			data.push_back(*itr);
			++itr;
			data.push_back(*itr);
			++itr;
			//std::cerr << "system common: song pos pointer " << (((unsigned int)data[1])<<7 | (unsigned int)data[0]) << std::endl;
		} else if ( status == smf::SYS_SONGSEL ) {
			if ( (*itr & 0x80) != 0 ) {
				std::cerr << "smf::smf::read warning! " << "SYS_SONGSEL song select followed by 8bit number 0x" << std::hex << (((unsigned int) *itr) & 0xff) << std::endl;
			}
			data.push_back( (*itr) & 0x7f);
			++itr;
			//std::cerr << "system common: song select " << std::hex << (unsigned int) (data[0]) << std::endl;
		} else if ( status == smf::SYS_TUNEREQ ) {
			//std::cerr << "system common: tune request" << std::endl;
		} else {
			std::cerr << "smf::event::read unknown system event!";
			if ( not_running_status ) {
				std::cerr << " status = " << std::hex << int(status);
			} else {
				std::cerr << " running status = " << std::hex << (unsigned int) laststatus;
			}
			std::cerr << ", type = " << std::hex << int(type) << std::endl;
			// error.
		}
	}
	//std::cout << *this << std::endl;
}

std::ostream & smf::event::printOn(std::ostream & out) const {
	uint8_t type = status & 0xf0;
	if ( (smf::MIDI_NOTEOFF <= type) && (type <= smf::MIDI_PITCHBEND) ) {
		out << "(";
		if ( delta > 0 )
			out << delta << ", ";
		switch(type) {
		case smf::MIDI_NOTEOFF:
			out << "NOTEOFF:" << channel() << ", "
			<< notename() << octave(); // << ", " << int(evt.data[1]);
			break;
		case smf::MIDI_NOTEON:
			out << "NOTE ON:" << channel() << ", "
			<< notename() << octave() << ", " << int(data[1]);
			break;
		case smf::MIDI_POLYKEYPRESSURE:
			out << "POLYKEY PRESS, " << channel() << ", "
			<< std::dec << int(data[0]) << ", " << int(data[1]);
			break;
		case smf::MIDI_CONTROLCHANGE:
			out << "CTL CHANGE, " << channel() << ", "
			<< std::dec << int(data[0]) << ", " << int(data[1]);
			break;
		case smf::MIDI_PROGRAMCHANGE:
			out << "PRG CHANGE, " << channel() << ", "
			<< std::dec << int(data[0]);
			break;
		case smf::MIDI_CHPRESSURE:
			out << "CHANNEL PRESS, " << channel() << ", "
			<< std::dec << int(data[0]);
			break;
		case smf::MIDI_PITCHBEND:
			out << "CHANNEL PRESS, " << channel() << ", "
			<< std::dec << (uint16_t(data[1])<<7 | data[0]);
			break;
		}
		out << ")";
	} else if ( status == smf::ESCSYSEX ) {
		out << "(";
		if ( delta != 0 )
			out << delta << ", ";
		out<< "ESCSYSEX ";
		for(auto i = data.begin(); i != data.end(); ++i) {
			if ( isprint(*i) && !isspace(*i) ) {
				out << char(*i);
			} else {
				out << std::hex << std::setw(2) << int(*i);
			}
		}
		out << ")";
	} else if ( status == smf::META ) {
		out << "(";
		if ( delta != 0 )
			out << std::dec << delta << ", ";
		out<< "M ";
		uint32_t tempo;
		switch (data[0]) {
		case 0x01:
			out << "text: ";
			printData(out, 1);
			break;
		case 0x02:
			out << "(c): ";
			printData(out, 1);
			break;
		case 0x03:
			out << "seq.name: ";
			printData(out, 1);
			break;
		case 0x04:
			out << "instr: ";
			printData(out, 1);
			break;
		case 0x05:
			out << "lyrics: ";
			printData(out, 1);
			break;
		case 0x06:
			out << "marker: ";
			printData(out, 1);
			break;
		case 0x07:
			out << "cue: ";
			printData(out, 1);
			break;
		case 0x08:
			out << "prog.: ";
			printData(out, 1);
			break;
		case 0x09:
			out << "dev.: ";
			printData(out, 1);
			break;
		case 0x21:
			out << "out port " << std::dec << int(data[1]);
			break;
		case 0x2f:
			out << "EoT";
			break;
		case 0x51:
			tempo = uint8_t(data[1]);
			tempo <<= 8;
			tempo |= uint8_t(data[2]);
			tempo <<= 8;
			tempo |= uint8_t(data[3]);
			out << "tempo 4th = " << std::dec << (60000000L/tempo);
			break;
		case 0x58:
			out << "time sig.: " << std::dec << int(data[1]) << "/" << int(1<<data[2]);
			out << ", " << int(data[3]) << " mclk., " << int(data[4]) << " 32nd";
			break;
		case 0x59:
			out << "key sig.:";
			if (data[0] == 0) {
				out << "C ";
			} else if (char(data[0]) < 0) {
				out << int(-char(data[0])) << " flat(s) ";
			} else {
				out << int(data[0]) << "sharp(s) ";
			}
			if (data[1] == 0) {
				out << "major";
			} else {
				out << "minor";
			}
			break;
		default:
			out << "unknown type " << "0x" << std::hex << (((unsigned int)data[0])& 0xff);
			out << " (" << std::dec << data.size() << ") ";
			bool textmode = false;
			for(auto i = data.begin(); i != data.end(); ++i) {
				if ( isprint(*i) && !isspace(*i) ) {
					if (!textmode)
						out << " \"";
					textmode = true;
					out << char(*i);
				} else {
					if (textmode)
						out << "\" ";
					textmode = false;
					unsigned int val = *i & 0xff;
					out << std::hex << std::setw(2) << std::setfill('0') << val;
				}
			}
		}
		out << ")";
	} else if ( status == smf::SYS_EX ) {
		out << "(";
		if ( delta != 0 )
			out << delta << ", ";
		out<< "SYS_EX " ;
		out << "(" << std::dec << data.size() << ") ";
		bool textmode = false;
		for(auto i = data.begin(); i != data.end(); ++i) {
			if ( isprint(*i) && !isspace(*i) ) {
				if (!textmode)
					out << " \"";
				textmode = true;
				out << char(*i);
			} else {
				if (textmode)
					out << "\" ";
				textmode = false;
				unsigned int val = *i & 0xff;
				out << std::hex << std::setw(2) << std::setfill('0') << val;
			}
		}
		if (textmode)
			out << "\"";
		out << ")";
	} else if (status == smf::SYS_SONGPOS ) {
		out << "(";
		if ( delta != 0 )
			out << std::dec << delta << ", ";
		uint16_t val14bit = data[1] & 0x7f;
		val14bit <<= 7;
		val14bit |= (uint16_t)data[0] & 0x7f;
		out << "SYS_SONGPOS " << val14bit << ")";
	} else if ( status == smf::SYS_SONGSEL ) {
		out << "(";
		if ( delta != 0 )
			out << std::dec << delta << ", ";
		out << "SYS_SONGSEL "<< (uint16_t) data[0] << ")";
	} else if ( status == smf::SYS_TUNEREQ ) {
		out << "(";
		if ( delta != 0 )
			out << std::dec << delta << ", ";
		out << "SYS_TUNEREQ" << ")";
	} else {
		out << "(";
		if ( delta != 0 )
			out << std::dec << delta << ", ";
		out << "UNKNOWN MESSAGE: ";
		// error.
		std::cerr << "smfevent::operator<< error!";
		out << std::hex << (unsigned int) status << " ";
		for(auto i = data.begin(); i != data.end(); ++i) {
			if ( isprint(*i) && !isspace(*i) ) {
				out << char(*i);
			} else {
				out << std::hex << std::setw(2) << int(*i);
			}
		}
		out << ")";
		// error.
	}
	return out;
}

smf::score::score(std::istream & smffile) {
	std::istreambuf_iterator<char> itr(smffile);
	std::istreambuf_iterator<char> end_itr;

	uint32_t tracksig;
	tracksig = get_uint32BE(itr);
	if ( tracksig == INT_MThd ) {
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
		uint32_t tracksig = get_uint32BE(itr);
		if ( tracksig == INT_MTrk ) {
			//uint32_t len =
			get_uint32BE(itr);  // skip the track length
			//std::cerr << len << std::endl;
			tracks.push_back(std::vector<event>());
			uint8_t laststatus = 0;
			event ev;
			//long counter = 0;
			do {
				ev.clear();
				ev.read(itr, laststatus);
				//std::cout << "laststatus = " << std::hex << (int) laststatus << " " << ev << std::endl;
				if ( ev.isMIDI() ) {
					laststatus = ev.status;
				}
				tracks.back().push_back(ev);
			} while ( !ev.isEoT() and itr != end_itr /* tracks.back().back().isEoT() */ );

		} else if ( tracksig == INT_XFIH) {
			//std::cerr << "XFIH" << std::endl;
			uint32_t l = get_uint32BE(itr);
			//std::cerr << int(l) << std::endl;
			for (unsigned int i = 0; itr != end_itr and i < l; ++i, ++itr) {
				//std::cerr << std::hex << std::setfill('0') << std::setw(2) << (0x0000L | uint8_t(*itr)) << " ";
			}
			//std::cerr << std::endl;
		} else if ( tracksig == INT_XFKM ) {
			//std::cerr << "XFKM" << std::endl;
			uint32_t l = get_uint32BE(itr);
			//std::cerr << int(l) << std::endl;
			for (unsigned int i = 0; itr != end_itr and i < l; ++i, ++itr) {
				//std::cerr << std::hex << std::setfill('0') << std::setw(2) << (0x0000L | uint8_t(*itr)) << " ";
			}
			//std::cerr << std::endl;
		} else if ( tracksig == INT_Cont ) {
			uint32_t l = get_uint32BE(itr);
			std::cerr << "Unknown track signature Cont " << std::dec << int(l) << std::endl;
			for (unsigned int i = 0; itr != end_itr and i < l; ++i, ++itr) {
				//std::cerr << std::hex << std::setfill('0') << std::setw(2) << (0x0000L | uint8_t(*itr)) << " ";
			}
		} else {
			std::cerr << " Warning: Abandoned unknown non-MTrk data chunk: " << std::hex << tracksig << " ";
			break;
		}
	}
	return;
}

std::ostream & smf::score::header_info(std::ostream & out) const {
	out << "Format = " << std::dec << smfformat;
	out << ", num. of Tracks = " << ntracks;
	out << ", division = " << division;
	return out;
}

// smf::score の tracks に含まれるトラックをスキャンし，
// note-on と note-off イベントの組を音符 smf::note として開始時刻，音程，長さの組
// に解釈し，smf::note の開始時刻順の列として返す．
std::vector<smf::note> smf::score::notes() const {
	std::vector<smf::note> noteseq;
	struct {
		std::vector<smf::event>::const_iterator iter; // iterator
		uint64_t elapsed;    // time elapsed from the start to just before delta time of *iter
	} track[noftracks()];
	for(int i = 0; i < noftracks(); ++i) {
		track[i] = {tracks[i].cbegin(), 0};
	}

	struct {
		struct {
			bool on;
			uint64_t index;
		} note[128];
	} midi[16];

	uint64_t globaltime = 0, next_global, tracks_next;
	int cnt = 0;
	while (true) {
		next_global = UINT64_MAX;
		for(int i = 0; i < noftracks(); ++i) {
			while ( (! track[i].iter->isEoT())
					&& (track[i].elapsed + track[i].iter->deltaTime() <= globaltime) ) {
				const smf::event & evt = *(track[i].iter);
				std::cout << evt << std::endl;
				if ( evt.isNoteOn() ) {
					midi[evt.channel()].note[evt.notenumber()].on = true;
					noteseq.push_back(note(globaltime, evt));
					midi[evt.channel()].note[evt.notenumber()].index = noteseq.size() - 1;
				} else if ( evt.isNoteOff() ) {
					midi[evt.channel()].note[evt.notenumber()].on = false;
					const int & idx = midi[evt.channel()].note[evt.notenumber()].index;
					noteseq[idx].duration = globaltime - noteseq[idx].time;
				} else {
					//std::cout << globaltime << evt << ", ";
				}
				// go iterator forward
				track[i].elapsed += track[i].iter->deltaTime();
				std::cout << track[i].elapsed << "; ";
				++track[i].iter;
				tracks_next = globaltime + track[i].iter->deltaTime();
				// update elapsed time
			}
			if (tracks_next < next_global) {
				next_global = tracks_next;
				std::cout << "next " << next_global << std::endl;
			}
			std::cout << std::endl;
			if ( track[i].iter->isEoT() )
				continue;
		}
		globaltime = next_global;
		if ( ++cnt > 100 )
			break;
	}

	/*
	// zero global time events
	for(uint32_t i = 0; i < noftracks(); ++i) {
		track[i].elasped = 0;
		while ( track[i].cursor->deltaTime() == 0 && ! track[i].cursor->isEoT() ) {
			// issue events
			const smf::event & evt = *tcursor[i].cursor;
			//std::cout << i << ": " << evt << " ";
			if ( evt.isNoteOn() ) {
				noteseq.push_back(note(globaltime, evt));
				emu[evt.channel()].key[evt.notenumber()].noteon = true;
				emu[evt.channel()].key[evt.notenumber()].index = noteseq.size() - 1;
			} else if ( evt.isNoteOff() ) {
				if ( emu[evt.channel()].key[evt.notenumber()].noteon ) {
					const uint64_t & idx = emu[evt.channel()].key[evt.notenumber()].index;
					noteseq[idx].duration = globaltime - noteseq[idx].time;
					emu[evt.channel()].key[evt.notenumber()].noteon = false;
				}
			}
			++tcursor[i].cursor;
		}
		//std::cout << std::endl;
		if ( trk[i].cursor->isEoT() )
			continue;
		tcursor[i].to_go = tcursor[i].cursor->deltaTime();

	}
	uint64_t min_to_go;

	while (true) {
		min_to_go = 0;
		for(uint32_t i = 0; i < noftracks(); ++i) {
			if ( tcursor[i].cursor->isEoT() )
				continue;
			if ( min_to_go == 0 or tcursor[i].to_go < min_to_go ) {
				min_to_go = tcursor[i].to_go;
			}
		}
		//std::cout << "min_to_go = " << min_to_go << std::endl;
		globaltime += min_to_go;
		//std::cout << "global = " << globaltime << std::endl;
		if (min_to_go == 0)
			break;
		for(uint32_t i = 0; i < noftracks(); ++i) {
			if ( tcursor[i].cursor->isEoT() )
				continue;
			tcursor[i].to_go -= min_to_go;

			if ( tcursor[i].to_go == 0 ) {
				do {
					const smf::event & evt = *tcursor[i].cursor;
					// events occur

					if ( evt.isNoteOn() ) {
						noteseq.push_back(note(globaltime, evt));
						emu[evt.channel()].key[evt.notenumber()].noteon = true;
						emu[evt.channel()].key[evt.notenumber()].index = noteseq.size() - 1;
					} else if ( evt.isNoteOff() ) {
						if ( emu[evt.channel()].key[evt.notenumber()].noteon ) {
							const uint64_t & idx = emu[evt.channel()].key[evt.notenumber()].index;
							noteseq[idx].duration = globaltime - noteseq[idx].time;
							emu[evt.channel()].key[evt.notenumber()].noteon = false;
						}
					}
					++tcursor[i].cursor;
				} while ( tcursor[i].cursor->deltaTime() == 0 && ! tcursor[i].cursor->isEoT() );
				//std::cout << std::endl;
				trk[i].to_go = tcursor[i].cursor->deltaTime();
			}

		}
	}
	 */
	return noteseq;
}
