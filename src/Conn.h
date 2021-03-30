// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include <sys/types.h>

#include <string>
#include <tuple>
#include <type_traits>

#include "zeek/Dict.h"
#include "zeek/Timer.h"
#include "zeek/Rule.h"
#include "zeek/IPAddr.h"
#include "zeek/UID.h"
#include "zeek/WeirdState.h"
#include "zeek/ZeekArgs.h"
#include "zeek/IntrusivePtr.h"
#include "zeek/Session.h"
#include "zeek/iosource/Packet.h"

#include "zeek/analyzer/Tag.h"
#include "zeek/analyzer/Analyzer.h"

namespace zeek {

class Connection;
class NetSessions;
class EncapsulationStack;
class Val;
class RecordVal;

using ValPtr = IntrusivePtr<Val>;
using RecordValPtr = IntrusivePtr<RecordVal>;

namespace detail {

class Specific_RE_Matcher;
class RuleEndpointState;
class RuleHdrTest;

} // namespace detail

namespace analyzer {

class TransportLayerAnalyzer;
class Analyzer;

} // namespace analyzer

enum ConnEventToFlag {
	NUL_IN_LINE,
	SINGULAR_CR,
	SINGULAR_LF,
	NUM_EVENTS_TO_FLAG,
};

struct ConnID {
	IPAddr src_addr;
	IPAddr dst_addr;
	uint32_t src_port;
	uint32_t dst_port;
	bool is_one_way;	// if true, don't canonicalize order
	TransportProto proto;
};

static inline int addr_port_canon_lt(const IPAddr& addr1, uint32_t p1,
                                     const IPAddr& addr2, uint32_t p2)
	{
	return addr1 < addr2 || (addr1 == addr2 && p1 < p2);
	}

class BaseConnection : public Obj {};

class Connection final : public Session {
public:

	Connection(NetSessions* s, const detail::ConnIDKey& k, double t, const ConnID* id,
	           uint32_t flow, const Packet* pkt);
	~Connection() override;

	/**
	 * Invoked when an encapsulation is discovered. It records the encapsulation
	 * with the connection and raises a "tunnel_changed" event if it's different
	 * from the previous encapsulation or if it's the first one encountered.
	 *
	 * @param encap The new encapsulation. Can be set to null to indicated no
	 * encapsulation or clear an old one.
	 */
	void CheckEncapsulation(const std::shared_ptr<EncapsulationStack>& encap);

	/**
	 * Invoked when the session is about to be removed. Use Ref(this)
	 * inside Done to keep the session object around, though it'll
	 * no longer be accessible from the SessionManager.
	 */
	void Done() override;

	// Process the connection's next packet.  "data" points just
	// beyond the IP header.  It's updated to point just beyond
	// the transport header (or whatever should be saved, if we
	// decide not to save the full packet contents).
	//
	// If record_packet is true, the packet should be recorded.
	// If record_content is true, then its entire contents should
	// be recorded, otherwise just up through the transport header.
	// Both are assumed set to true when called.
	void NextPacket(double t, bool is_orig,
	                const IP_Hdr* ip, int len, int caplen,
	                const u_char*& data,
	                int& record_packet, int& record_content,
	                // arguments for reproducing packets
	                const Packet *pkt);

	// Keys are only considered valid for a connection when a
	// connection is in the session map. If it is removed, the key
	// should be marked invalid.
	const detail::ConnIDKey& Key() const	{ return key; }
	const std::unique_ptr<detail::HashKey>& HashKey() const override
		{ return hash_key; }
	void ClearKey() override
		{
		key_valid = false;
		hash_key.reset();
		}
	bool IsKeyValid() const	override	{ return key_valid; }

	const IPAddr& OrigAddr() const		{ return orig_addr; }
	const IPAddr& RespAddr() const		{ return resp_addr; }

	uint32_t OrigPort() const			{ return orig_port; }
	uint32_t RespPort() const			{ return resp_port; }

	void FlipRoles();

	analyzer::Analyzer* FindAnalyzer(analyzer::ID id);
	analyzer::Analyzer* FindAnalyzer(const analyzer::Tag& tag);	// find first in tree.
	analyzer::Analyzer* FindAnalyzer(const char* name);	// find first in tree.

	TransportProto ConnTransport() const { return proto; }

	// FIXME: Now this is in Analyzer and should eventually be removed here.
	//
	// If true, skip processing of remainder of connection.  Note
	// that this does not in itself imply that record_packets is false;
	// we might want instead to process the connection off-line.
	void SetSkip(bool do_skip)		{ skip = do_skip ? 1 : 0; }
	bool Skipping() const			{ return skip; }

	// Returns true if the packet reflects a reuse of this
	// connection (i.e., not a continuation but the beginning of
	// a new connection).
	bool IsReuse(double t, const u_char* pkt);

	/**
	 * Returns the associated "connection" record.
	 */
	const RecordValPtr& ConnVal() override;

	/**
	 * Append additional entries to the history field in the connection record.
	 */
	void AppendAddl(const char* str);

	void Match(detail::Rule::PatternType type, const u_char* data, int len,
	           bool is_orig, bool bol, bool eol, bool clear_state);

	/**
	 * Generates connection removal event(s).
	 */
	void RemovalEvent() override;

	void Weird(const char* name, const char* addl = "", const char* source = "");
	bool DidWeird() const	{ return weird != 0; }

	inline bool FlagEvent(ConnEventToFlag e)
		{
		if ( e >= 0 && e < NUM_EVENTS_TO_FLAG )
			{
			if ( suppress_event & (1 << e) )
				return false;
			suppress_event |= 1 << e;
			}

		return true;
		}

	void Describe(ODesc* d) const override;
	void IDString(ODesc* d) const;

	// Statistics.

	// Just a lower bound.
	unsigned int MemoryAllocation() const override;
	unsigned int MemoryAllocationConnVal() const override;

	static uint64_t TotalConnections()
		{ return total_connections; }
	static uint64_t CurrentConnections()
		{ return current_connections; }

	// Returns true if the history was already seen, false otherwise.
	bool CheckHistory(uint32_t mask, char code)
		{
		if ( (hist_seen & mask) == 0 )
			{
			hist_seen |= mask;
			AddHistory(code);
			return false;
			}
		else
			return true;
		}

	// Increments the passed counter and adds it as a history
	// code if it has crossed the next scaling threshold.  Scaling
	// is done in terms of powers of the third argument.
	// Returns true if the threshold was crossed, false otherwise.
	bool ScaledHistoryEntry(char code, uint32_t& counter,
	                        uint32_t& scaling_threshold,
	                        uint32_t scaling_base = 10);

	void HistoryThresholdEvent(EventHandlerPtr e, bool is_orig,
	                           uint32_t threshold);

	void AddHistory(char code)	{ history += code; }

	// Sets the root of the analyzer tree as well as the primary PIA.
	void SetRootAnalyzer(analyzer::TransportLayerAnalyzer* analyzer, analyzer::pia::PIA* pia);
	analyzer::TransportLayerAnalyzer* GetRootAnalyzer()	{ return root_analyzer; }
	analyzer::pia::PIA* GetPrimaryPIA()	{ return primary_PIA; }

	// Sets the transport protocol in use.
	void SetTransport(TransportProto arg_proto)	{ proto = arg_proto; }

	void SetUID(const UID &arg_uid)	 { uid = arg_uid; }

	UID GetUID() const { return uid; }

	std::shared_ptr<EncapsulationStack> GetEncapsulation() const
		{ return encapsulation; }

	void CheckFlowLabel(bool is_orig, uint32_t flow_label);

	uint32_t GetOrigFlowLabel() { return orig_flow_label; }
	uint32_t GetRespFlowLabel() { return resp_flow_label; }

	bool PermitWeird(const char* name, uint64_t threshold, uint64_t rate,
	                 double duration);

protected:

	// Allow other classes to access pointers to these:
	friend class detail::SessionTimer;

	NetSessions* sessions;
	detail::ConnIDKey key;
	std::unique_ptr<detail::HashKey> hash_key;
	bool key_valid;

	IPAddr orig_addr;
	IPAddr resp_addr;
	uint32_t orig_port, resp_port;	// in network order
	TransportProto proto;
	uint32_t orig_flow_label, resp_flow_label;	// most recent IPv6 flow labels
	uint32_t vlan, inner_vlan;	// VLAN this connection traverses, if available
	u_char orig_l2_addr[Packet::L2_ADDR_LEN];	// Link-layer originator address, if available
	u_char resp_l2_addr[Packet::L2_ADDR_LEN];	// Link-layer responder address, if available
	RecordValPtr conn_val;
	std::shared_ptr<EncapsulationStack> encapsulation; // tunnels
	int suppress_event;	// suppress certain events to once per conn.

	unsigned int skip:1;
	unsigned int weird:1;
	unsigned int finished:1;
	unsigned int saw_first_orig_packet:1, saw_first_resp_packet:1;

	// Count number of connections.
	static uint64_t total_connections;
	static uint64_t current_connections;

	std::string history;
	uint32_t hist_seen;

	analyzer::TransportLayerAnalyzer* root_analyzer;
	analyzer::pia::PIA* primary_PIA;

	UID uid;	// Globally unique connection ID.
	detail::WeirdStateMap weird_state;
};

} // namespace zeek
