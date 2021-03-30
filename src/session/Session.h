// See the file "COPYING" in the main distribution directory for copyright.

#pragma once

#include <memory>

#include "zeek/Hash.h"
#include "zeek/Obj.h"
#include "zeek/EventHandler.h"
#include "zeek/Timer.h"

namespace zeek {

class RecordVal;
using RecordValPtr = IntrusivePtr<RecordVal>;
class Session;

namespace detail { class SessionTimer; }
namespace analyzer { class Analyzer; }

typedef void (Session::*timer_func)(double t);

class Session : public Obj {
public:

	/**
	 * Construct a new session.
	 *
	 * @param t The timestamp for the start and end of the session when it's initially
	 * created. The end time will be updated as later packets are processed.
	 * @param timeout_event The event that will be emitted when the session times out.
	 * @param status_update_event The event that will be emitted for session status
	 * updates. This can be set to null to disable status updates. This event also
	 * won't be emitted unless the EnableStatusUpdateTimer() method is called on the
	 * session, and the interval is set to a non-zero value.
	 * @param status_update_interval The interval in seconds for the status update
	 * event to be emitted. Setting this to zero disables the status update timer.
	 */
	Session(double t, EventHandlerPtr timeout_event,
	        EventHandlerPtr status_update_event = nullptr,
	        double status_update_interval = 0);

	virtual ~Session() {}

	/**
	 * Invoked when the session is about to be removed. Use Ref(this)
	 * inside Done to keep the session object around, though it'll
	 * no longer be accessible from the SessionManager.
	 */
	virtual void Done() = 0;

	// Keys are only considered valid for a session when the session is
	// stored by the SessionManager. If it is removed, the key will be
	// marked as invalid.
	virtual const std::unique_ptr<detail::HashKey>& HashKey() const = 0;
	virtual void ClearKey() = 0;
	virtual bool IsKeyValid() const = 0;

	double StartTime() const		{ return start_time; }
	void SetStartTime(double t)		{ start_time = t; }
	double LastTime() const			{ return last_time; }
	void SetLastTime(double t) 		{ last_time = t; }

	// True if we should record subsequent packets (either headers or
	// in their entirety, depending on record_contents).  We still
	// record subsequent SYN/FIN/RST, regardless of how this is set.
	bool RecordPackets() const		{ return record_packets; }
	void SetRecordPackets(bool do_record)	{ record_packets = do_record ? 1 : 0; }

	// True if we should record full packets for this connection,
	// false if we should just record headers.
	bool RecordContents() const		{ return record_contents; }
	void SetRecordContents(bool do_record)	{ record_contents = do_record ? 1 : 0; }

	// Set whether to record *current* packet header/full.
	void SetRecordCurrentPacket(bool do_record)
		{ record_current_packet = do_record ? 1 : 0; }
	void SetRecordCurrentContent(bool do_record)
		{ record_current_content = do_record ? 1 : 0; }

	/**
	 * Returns the associated record val for this session.
	 */
	virtual const RecordValPtr& GetVal() = 0;

	/**
	 * Return the memory allocation required by the session record. This requires at
	 * least one call to GetVal() first in order to setup the record object.
	 */
	virtual unsigned int MemoryAllocationVal() const = 0;

	/**
	 * A lower-bound calculation of how much memory a session object is using.
	 */
	virtual unsigned int MemoryAllocation() const;

	/**
	 * Generates session removal event(s). Must be overridden by child classes to
	 * provide specific removal events.
	 */
	virtual void RemovalEvent() = 0;

	/**
	 * Generate an event for this session.
	 *
	 * @param f The handler for the event to be generated. If the handler doesn't
	 * exist, this method doesn't generate anything.
	 * @param analyzer
	 * @param name If given, this will be passed as the first argument to the
	 * handler, followed by the session value. If null, then the event's first
	 * argument is the session value.
	 */
	void Event(EventHandlerPtr f, analyzer::Analyzer* analyzer = nullptr,
	           const char* name = nullptr);

	/**
	 * Enqueues an event associated with this connection and given analyzer.
	 */
	void EnqueueEvent(EventHandlerPtr f, analyzer::Analyzer* analyzer, Args args);

	/**
	 * A version of EnqueueEvent() taking a variable number of arguments.
	 */
	template <class... Args>
	std::enable_if_t<
		std::is_convertible_v<
			std::tuple_element_t<0, std::tuple<Args...>>, ValPtr>>
	EnqueueEvent(EventHandlerPtr h, analyzer::Analyzer* analyzer, Args&&... args)
		{ return EnqueueEvent(h, analyzer, zeek::Args{std::forward<Args>(args)...}); }

	virtual	void Describe(ODesc* d) const override;

	/**
	 * Sets the session to expire after a given amount of time.
	 *
	 * @param lifetime The amount of time in seconds from the current network time.
	 */
	void SetLifetime(double lifetime);

	/**
	 * Sets the inactivity timeout for this session.
	 *
	 * @param timeout The number of seconds of inactivity allowed for this session
	 * before it times out.
	 */
	void SetInactivityTimeout(double timeout);

	/**
	 * Returns the inactivity timeout for the session.
	 */
	double InactivityTimeout() const	{ return inactivity_timeout; }

	/**
	 * Activates the timer for the status update event.
	 */
	void EnableStatusUpdateTimer();

	/**
	 * Cancels all timers associated with this session.
	 */
	void CancelTimers();

	/**
	 * Called when the lifetime of the session expires. Fires a timeout event and
	 * removes the session from the manager.
	 * TODO: This function has a terrible name considering there's an AddTimer() and
	 * a RemoveTimer() method in this class as well.
	 *
	 * @param t This argument is ignored.
	 */
	void DeleteTimer(double t);

protected:

	friend class detail::SessionTimer;

	/**
	 * Add a given timer to expire at a specific time.
	 *
	 * @param timer A pointer to a method that will be called when the timer expires.
	 * @param t The time when the timer expires. This is an absolute time, not a time
	 * relative to the current network time.
	 * @param do_expire If set to true, the timer is also evaluated when Zeek
	 * terminates.
	 * @param type The type of timer being added.
	 */
	void AddTimer(timer_func timer, double t, bool do_expire,
	              detail::TimerType type);

	/**
	 * Remove a specific timer from firing.
	 */
	void RemoveTimer(detail::Timer* t);

	/**
	 * The handler method for inactivity timers.
	 */
	void InactivityTimer(double t);

	/**
	 * The handler method for status update timers.
	 */
	void StatusUpdateTimer(double t);

	// TODO: is this method used by anyone?
	void RemoveConnectionTimer(double t);

	double start_time, last_time;
	TimerPList timers;
	double inactivity_timeout;

	unsigned int installed_status_timer:1;
	unsigned int timers_canceled:1;
	unsigned int is_active:1;
	unsigned int record_packets:1, record_contents:1;
	unsigned int record_current_packet:1, record_current_content:1;

	EventHandlerPtr session_timeout_event;
	EventHandlerPtr session_status_update_event;
	double session_status_update_interval;
};

namespace detail {

class SessionTimer final : public Timer {
public:
	SessionTimer(Session* arg_conn, timer_func arg_timer,
	                double arg_t, bool arg_do_expire, TimerType arg_type)
		: Timer(arg_t, arg_type)
		{ Init(arg_conn, arg_timer, arg_do_expire); }
	~SessionTimer() override;

	void Dispatch(double t, bool is_expire) override;

protected:

	void Init(Session* conn, timer_func timer, bool do_expire);

	Session* conn;
	timer_func timer;
	bool do_expire;
};

} // namespace detail
} // namespace zeek

#define ADD_TIMER(timer, t, do_expire, type) \
	AddTimer(timer_func(timer), (t), (do_expire), (type))
