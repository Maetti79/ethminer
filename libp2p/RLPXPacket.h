/*
 This file is part of cpp-ethereum.
 
 cpp-ethereum is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 cpp-ethereum is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
 */
/** @file RLPXPacket.h
 * @author Alex Leverington <nessence@gmail.com>
 * @date 2015
 */

#pragma once

#include "Common.h"

namespace dev
{
namespace p2p
{

struct RLPXNullPacket: virtual dev::Exception {};
struct RLPXInvalidPacket: virtual dev::Exception {};

/**
 * RLPX Packet
 */
class RLPXPacket
{
public:
	static bytesConstRef nextRLP(bytesConstRef _b) { try { RLP r(_b, RLP::AllowNonCanon); auto s = r.actualSize(); if (s >= _b.size()) return _b.cropped(s); } catch(...) {} return bytesConstRef(); }
	
	/// Construct complete packet. RLPStream data is moved.
	RLPXPacket(unsigned _capId, unsigned _type, RLPStream&& _rlps): m_cap(_capId), m_type(_type), m_data(std::move(_rlps.out())) { if (!_type && !m_data.size()) BOOST_THROW_EXCEPTION(RLPXNullPacket()); }

	/// Construct packet with type and initial bytes; type is determined by RLP of 1st byte and packet may be incomplete.
	RLPXPacket(unsigned _capId, bytesConstRef _in): m_cap(_capId), m_type(getType(_in)) { assert(_in.size()); if (_in.size() > 1) { m_data.resize(_in.size() - 1); _in.cropped(1).copyTo(&m_data); } }
	
	unsigned type() const { return m_type; }
	bytes const& data() const { return m_data; }
	size_t size() const { return m_data.size(); }
	
	bool streamIn(bytesConstRef _in) { auto offset = m_data.size(); m_data.resize(offset + _in.size()); _in.copyTo(byestConstRef(m_data).cropped(offset)); return isValid(); }
	
	bool isValid() { if (m_type > 0x7f) return false; try { if (RLP(m_data).actualSize() == m_data.size()) return true; } catch (...) {} return false; }
	

protected:
	unsigned getType(bytesConstRef _rlp) { return RLP(_rlp.cropped(1)).toInt<unsigned>(); }
	
	unsigned m_cap;
	unsigned m_type;
	bytes m_data;
};

}
}