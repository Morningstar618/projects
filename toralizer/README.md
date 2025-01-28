#######################################################################
##The following dependencies must be satisfied before running the tool.##
#######################################################################

1. Install tor service.
2. Run ./install.sh as root
3. Restart tor service to change IP or bad proxy connections

**USAGE**
toralize <command> | for example -> toralize curl http://<ip>:<port>

**Note**
If you use the hostname of the website instead of the IP, the DNS call will be made outside of the tor
network and will not be private.

