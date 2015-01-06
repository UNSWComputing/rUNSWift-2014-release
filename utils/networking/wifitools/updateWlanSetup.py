import sys
ROBOTS_TO_IPS = {
   "robot1": "10",
   "robot2": "11",
}


# Accept a robot name.
if len(sys.argv) < 2:
   sys.exit("No robot given")

robotName = sys.argv[1]
if robotName not in ROBOTS_TO_IPS:
   sys.exit("Invalid robot name")

# Generate the filled in runswiftwireless file.
runswiftWirelessFinalise = "/tmp/runswiftwireless"
runswiftWirelessTemplate = "runswiftwireless_template"

template = open(runswiftWirelessTemplate, "r").read()
filledIn = template % {
   "playerIP": ROBOTS_TO_IPS[robotName]
}

output = open(runswiftWirelessFinalise, "w")
output.write(filledIn)
output.close()
# Sync it to the robot.

import subprocess
subprocess.Popen("scp %s root@%s:/etc/init.d/runswiftwireless" % (runswiftWirelessFinalise, robotName), shell=True)