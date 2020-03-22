server : local_server.c utils.h
	gcc local_server.c -o server

driver : driver.c nmb.c utils.h
	gcc  nmb.c driver.c -o driver
