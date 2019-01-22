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

RUN mkdir /work

COPY multiabs_udf.c /work

RUN cd /work \
	&& gcc -o multiabs_udf.so multiabs_udf.c `mysql_config --cflags` -shared -fPIC \
	&& file multiabs_udf.so

FROM mysql:8.0

COPY --from=udf_layer /work/multiabs_udf.so /usr/lib/mysql/plugin/