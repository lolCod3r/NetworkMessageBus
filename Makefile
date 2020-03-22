server : local_server.c util.h
	gcc local_server.c -o server

driver : driver.c nmb.c util.h
	gcc  nmb.c driver.c -o driver
