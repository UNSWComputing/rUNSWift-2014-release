"""Team Status.

Prints out all the teams and the player numbers in a quick to read way.
Usage:
python team_status.py

NOTE: This is based on the configs in the repo, not talking to the actual robots.
"""

import ConfigParser
import os

teams = {}
CONFIG_DIR = os.path.expandvars("$RUNSWIFT_CHECKOUT_DIR/image/home/nao/data/configs")
configFiles = os.listdir(CONFIG_DIR)

# Read the configs.
for filename in configFiles:
   if not os.path.isfile(os.path.join(CONFIG_DIR, filename)):
      continue
   conf = ConfigParser.ConfigParser()
   confFile = open(os.path.join(CONFIG_DIR, filename), "r")
   try:
      conf.readfp(confFile)
   except:
      print "Couldn't open: %s" % filename
      continue
   confFile.close()
   playerNum = conf.get("player", "number")
   teamNum = int(conf.get("player", "team"))
   teamList = teams.get(teamNum, {})
   listForPlayerNum = teamList.get(playerNum, [])
   listForPlayerNum.append(filename.replace(".cfg", ""))
   teamList[playerNum] = listForPlayerNum
   teams[teamNum] = teamList

# Print the teams we grouped.
for team, teamList in teams.iteritems():
   print "===== TEAM %3s =====" % team
   for playerNum in sorted(teamList.keys()):
      print "%3s - %s" % (playerNum, " | ".join(sorted(teamList[playerNum])))
   print "===================="