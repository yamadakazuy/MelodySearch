/*
 * smf.cpp
 *
 *  Created on: 2022/05/10
 *      Author: sin
 *
 *      Ver. 20221016
 */

#include <iostream>
#include <iomanip>
#include <deque>

#include "smf.h"

uint32_t smf::get_uint32BE(std::istreambuf_iterator<char> & itr) {
	uint32_t res = 0;
	for(uint16_t i = 0; i < 4; ++i) {
		res <<= 8;
		res |= uint8_t(*itr);
		++itr;
	}
	return res;
}

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
//		bool res = true;
//		for(auto i = sig.begin(); i != sig.end(); ++i, ++itr) {
//			res &= (*i == *itr);
//		}
//		return res;
	return std::equal(str.begin(), str.end(), itr);
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
	}
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
			std::cerr << "sys_ex (" << data.size() << ") " << std::endl;
		}else if ( status == smf::ESCSYSEX ) {
			len = get_uint32VLQ(itr);
			for(uint32_t i = 0; i < len; ++i) {
				data.push_back(*itr);
				++itr;
			}
			std::cerr << "escsysex (" << data.size() << ") " << std::endl;
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
			std::cerr << "system common: song pos pointer " << (((unsigned int)data[1])<<7 | (unsigned int)data[0]) << std::endl;
		} else if ( status == smf::SYS_SONGSEL ) {
			if ( (*itr & 0x80) != 0 ) {
				std::cerr << "smf::smf::read warning! " << "SYS_SONGSEL song select followed by 8bit number 0x" << std::hex << (((unsigned int) *itr) & 0xff) << std::endl;
			}
			data.push_back( (*itr) & 0x7f);
			++itr;
			std::cerr << "system common: song select " << std::hex << (unsigned int) (data[0]) << std::endl;
		} else if ( status == smf::SYS_TUNEREQ ) {
			std::cerr << "system common: tune request" << std::endl;
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
			/*
			std::cout << "track " << tracks.size() << std::endl;
			if ( tracks.size() == 49 ) {
				std::cout << 49 << std::endl;
			}
			*/
			tracks.push_back(std::vector<event>());
			uint8_t laststatus = 0;
			event ev;
			//long counter = 0;
			do {
				ev.clear();
				ev.read(itr, laststatus);
				/*
				counter += 1;
				if ( (counter % 100) == 0 ) {
					std::cout << counter << std::endl;
				}
				*/
				laststatus = ev.status;
				tracks.back().push_back(ev);
				std::cout << ev << std::endl;
			} while ( !ev.isEoT() and itr != end_itr /* tracks.back().back().isEoT() */ );

		} else {
			std::cerr << "Warning: Encountered and abandoned unknown non-MTrk data chunk after MThd or MTrk. " << std::endl;
			break;
		}
	}
	return;
}

std::vector<smf::note> smf::score::notes() const{
	std::vector<smf::note> noteseq;
	struct trkinfo {
		std::vector<smf::event>::const_iterator cursor;
		uint32_t to_go;
	} trk[tracks.size()];
	struct {
		struct {
			bool noteon;
			uint64_t index;
		} key[128];
	} emu[16];

	for(int i = 0; i < noftracks(); ++i) {
		trk[i].cursor = tracks[i].cbegin();
	}
	uint64_t globaltime = 0;
	// zero global time events
	for(uint32_t i = 0; i < noftracks(); ++i) {
		trk[i].to_go = 0;
		while ( trk[i].cursor->deltaTime() == 0 && ! trk[i].cursor->isEoT() ) {
			// issue events
			const smf::event & evt = *trk[i].cursor;
			std::cout << i << ": " << evt << " ";
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
			++trk[i].cursor;
		}
		std::cout << std::endl;
		if ( trk[i].cursor->isEoT() )
			continue;
		trk[i].to_go = trk[i].cursor->deltaTime();

	}
	uint64_t min_to_go;

	while (true) {
		min_to_go = 0;
		for(uint32_t i = 0; i < noftracks(); ++i) {
			if ( trk[i].cursor->isEoT() )
				continue;
			if ( min_to_go == 0 or trk[i].to_go < min_to_go ) {
				min_to_go = trk[i].to_go;
			}
		}
		//std::cout << "min_to_go = " << min_to_go << std::endl;
		globaltime += min_to_go;
		//std::cout << "global = " << globaltime << std::endl;
		if (min_to_go == 0)
			break;
		for(uint32_t i = 0; i < noftracks(); ++i) {
			if ( trk[i].cursor->isEoT() )
				continue;
			trk[i].to_go -= min_to_go;

			if ( trk[i].to_go == 0 ) {
				do {
					const smf::event & evt = *trk[i].cursor;
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
					++trk[i].cursor;
				} while ( trk[i].cursor->deltaTime() == 0 && ! trk[i].cursor->isEoT() );
				//std::cout << std::endl;
				trk[i].to_go = trk[i].cursor->deltaTime();
			}

		}
	}

	return noteseq;
}
