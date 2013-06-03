# Remote Process Management System

This C++ application was developed for a MSc dissertation.  It allows the management of processes across a number of nodes, load balancing the processes based on available and consumed resources. Of notable interest is the process reallocation in the event of node failure.  Uses the Bully algorithm to 'elect' the master node across the nodes.  Additionally provides a client application that uses the same algorithm to locate the master node.
