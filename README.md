# MULTI_ABS() UDF function

This is a MySQL UDF which does multiple ABS() calls to pairs of the arguments and accumulates the result.

 
## How to build it

### Development mode
If you want to change the function often for development purposes than a multi stage Dockerfile is not the best option because the base image which should include the GCC toolchain will be built every time. For this reason, there is a Dockerfile.base . To build the base image just run ```make build-base```. After that every time you want to build after code change just run ```make build```. Then Dockerfile and not Dockerfile.base will be used and the .so file called multiabs_udf.so will be created and copied into /usr/lib/mysql/plugin/ where MySQL (in the standard container) looks for plugins. 

### Non-development mode
When you just want to compile once (or rarely, in case of CI/CD) the function then you should modify the Dockerfile and remove the usage of a base image and just build it on the fly. To elaborate more. To switch from development mode:
```
# This should be the real code (found in Dockerfile.base) after the development has finished. 
# A base container can be cached and thus the image building will be really quick
#
#FROM mysql:8.0 as udf_layer
#
#RUN apt-get update && apt-get install -y \
#    gcc file\
#	libmysqlclient-dev \
#    && apt-get clean \
#    && rm -rf /var/lib/apt/lists/*
	
FROM multiabs_mysql_base:8.0 as udf_layer
...
```
to non-development, you should have this:
```
# This should be the real code (found in Dockerfile.base) after the development has finished. 
# A base container can be cached and thus the image building will be really quick

FROM mysql:8.0 as udf_layer

RUN apt-get update && apt-get install -y \
    gcc file\
	libmysqlclient-dev \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*
	
...
```
## Steps
- ```make build-base``` (in case you are developing)
- ```make build``` to build the containers
- ```make container-start``` to start the newly built container which contains the UDF
- ```make container-logs``` in case you want to see the stdout of the container
- ```make container-kill``` kills the container. Later it needs to be removed/destroyed
- ```make container-destroy``` to remove the container
- ```make function-create``` to create/load the function
- ```make function-test``` to run rudimentary test queries agains the functin
- ```make function-drop``` to drop/unload the function

If you want to skip the container-kill and container-destroy commands just pass `-rm` to docker in step container-start. The container opens local port 3306 which the function-* targets use with local MySQL CLI which you need to have installed in the host OS.


## Examples
MULTI_ABS() returns 0
MULTI_ABS(3, 8) returns 5
MULTI_ABS(3, 8, 12, 25) returns ABS(3,8)+ABS(12,25) = 18

MULTI_ABS(3) will error because the argument count is odd
MULTI_ABS(3,5,8) will error because the argument count is odd
