// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include <sys/types.h> // for u_char
#include <map>
#include <utility>

#include "zeek/Frag.h"
#include "zeek/PacketFilter.h"
#include "zeek/NetVar.h"
#include "zeek/analyzer/protocol/tcp/Stats.h"

namespace zeek {

class EncapsulationStack;
class Packet;
class Connection;
struct ConnID;

namespace analyzer::stepping_stone { class SteppingStoneManager; }

struct SessionStats {
	size_t num_TCP_conns;
	size_t max_TCP_conns;
	uint64_t cumulative_TCP_conns;

	size_t num_UDP_conns;
	size_t max_UDP_conns;
	uint64_t cumulative_UDP_conns;

	size_t num_ICMP_conns;
	size_t max_ICMP_conns;
	uint64_t cumulative_ICMP_conns;

	size_t num_fragments;
	size_t max_fragments;
	uint64_t num_packets;
};

class NetSessions {
public:
	NetSessions();
	~NetSessions();

	void Done();	// call to drain events before destructing

	// Looks up the connection referred to by the given Val,
	// which should be a conn_id record.  Returns nil if there's
	// no such connection or the Val is ill-formed.
	Connection* FindConnection(Val* v);

	/**
	 * Looks up the connection referred to by a given key.
	 *
	 * @param key The key for the connection to search for.
	 * @param proto The transport protocol for the connection.
	 * @return The connection, or nullptr if one doesn't exist.
	 */
	Connection* FindConnection(const detail::ConnIDKey& key, TransportProto proto);

	void Remove(Connection* c);
	void Insert(Connection* c, bool remove_existing = true);

	// Generating connection_pending events for all connections
	// that are still active.
	void Drain();

	// Clears the session maps.
	void Clear();

	void GetStats(SessionStats& s) const;

	void Weird(const char* name, const Packet* pkt,
	           const char* addl = "", const char* source = "");
	void Weird(const char* name, const IP_Hdr* ip,
	           const char* addl = "");

	detail::PacketFilter* GetPacketFilter(bool init=true)
		{
		if ( ! packet_filter && init )
			packet_filter = new detail::PacketFilter(detail::packet_filter_default);
		return packet_filter;
		}

	analyzer::stepping_stone::SteppingStoneManager* GetSTPManager()	{ return stp_manager; }

	unsigned int CurrentConnections()
		{
		return conns.size();
		}

	[[deprecated("Remove in v5.1. Use packet_analysis::IP::IPAnalyzer::ParseIPPacket.")]]
	int ParseIPPacket(int caplen, const u_char* const pkt, int proto,
	                  IP_Hdr*& inner);

	unsigned int ConnectionMemoryUsage();
	unsigned int ConnectionMemoryUsageConnVals();
	unsigned int MemoryAllocation();
	analyzer::tcp::TCPStateStats tcp_stats;	// keeps statistics on TCP states

private:

	using ConnectionMap = std::map<detail::ConnIDKey, Connection*>;

	Connection* LookupConn(const ConnectionMap& conns, const detail::ConnIDKey& key);

	// Inserts a new connection into the sessions map. If a connection with
	// the same key already exists in the map, it will be overwritten by
	// the new one.  Connection count stats get updated either way (so most
	// cases should likely check that the key is not already in the map to
	// avoid unnecessary incrementing of connecting counts).
	void InsertConnection(const detail::ConnIDKey& key, Connection* conn);

	ConnectionMap conns;

	SessionStats stats;

	analyzer::stepping_stone::SteppingStoneManager* stp_manager;
	detail::PacketFilter* packet_filter;
};

// Manager for the currently active sessions.
extern NetSessions* sessions;

} // namespace zeek
