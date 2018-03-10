#ifndef MOL_PROMISE_DEBUG_TEST_HELPER_DEF_GUARD_DEFINE_
#define MOL_PROMISE_DEBUG_TEST_HELPER_DEF_GUARD_DEFINE_

#include "reprocpp/debug.h"

/*
 * Helper to setup tests and examples and define the debug mode global counters.
 */

namespace repro {

LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(promises)

}

namespace prio {


LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(events)
LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(timeouts)
LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(sockets)
LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(server_connections)
LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(client_connections)

LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(ws_connections)
LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(ws_writers)

LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(tcp_connections)
LITTLE_MOLE_DEFINE_DEBUG_REF_CNT(ssl_connections)


}

#ifdef MOL_PROMISE_DEBUG
#include <iostream>
#define MOL_TEST_PRINT_CNTS()  \
	    std::cerr << "promises:" << promises_g_count << std::endl; \
	    std::cerr << "events:" << events_g_count << std::endl; \
	    std::cerr << "timeouts:" << timeouts_g_count << std::endl; \
	    std::cerr << "sockets:" << sockets_g_count << std::endl; \
		std::cerr << "tcp connections:" << tcp_connections_g_count << std::endl; \
		std::cerr << "ssl connections:" << ssl_connections_g_count << std::endl; \
		std::cerr << "processors:" << server_connections_g_count << std::endl; \
		std::cerr << "clients:" << client_connections_g_count << std::endl; \
		std::cerr << "websockets:" << prio::ws_connections_g_count << std::endl; \
		std::cerr << "clients:" << prio::ws_writers_g_count << std::endl; 

#else
#define MOL_TEST_PRINT_CNTS()
#endif

#ifdef MOL_PROMISE_DEBUG
#define MOL_TEST_ASSERT_CNTS(p,e) \
	    EXPECT_EQ(p,promises_g_count); \
	    EXPECT_EQ(e,events_g_count); \
	    EXPECT_EQ(e,timeouts_g_count); \
	    EXPECT_EQ(0,sockets_g_count); \
		EXPECT_EQ(0,tcp_connections_g_count); \
		EXPECT_EQ(0,ssl_connections_g_count); \
	    EXPECT_EQ(0,server_connections_g_count); \
	    EXPECT_EQ(0,client_connections_g_count); \
		EXPECT_EQ(0,prio::ws_connections_g_count); \
		EXPECT_EQ(0,prio::ws_writers_g_count); \
		
#else
#define MOL_TEST_ASSERT_CNTS(p,e)
#endif





#endif
