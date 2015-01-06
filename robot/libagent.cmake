SET(AGENT_SRCS
   libagent/libagent.cpp
   libagent/libagent.hpp
   libagent/AgentData.hpp
   utils/options.cpp
)

add_library(agent MODULE ${AGENT_SRCS})
