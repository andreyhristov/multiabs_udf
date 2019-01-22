build:
	docker build --no-cache . --tag multiabs_mysql:8.0

build-base:
	docker build --no-cache . -f Dockerfile.base --tag multiabs_mysql_base:8.0
	

MYSQL_80_3306_NAME=3306-mysql-80
MYSQL_80_DOCKER_IMAGE=multiabs_mysql:8.0


recreate-db:
	rm -rf mysqldb && \
	mkdir mysqldb && \
	chmod 777 mysqldb

container-start:
	mkdir -p mysqldb && chmod 777 mysqldb && \
	docker run -d --name $(MYSQL_80_3306_NAME)  \
		--user 1000 -p 3306:3306 \
		--volume `pwd`/mysqldb:/var/lib/mysql \
		--env MYSQL_ROOT_PASSWORD=root \
		--env MYSQL_ALLOW_EMPTY_PASSWORD=1 \
		--env MYSQL_USER=user \
		--env MYSQL_PASSWORD=pass \
		--env MYSQL_DATABASE=test  \
		$(MYSQL_80_DOCKER_IMAGE)

container-logs:
	/usr/bin/docker logs -f $(MYSQL_80_3306_NAME)

container-kill:
	/usr/bin/docker kill `docker ps -q -f name=$(MYSQL_80_3306_NAME)`

container-destroy:
	/usr/bin/docker rm `docker ps -aq -f name="$(MYSQL_80_3306_NAME)"`


#see  in multiabs_udf.so
FUNCTION_NAME=multi_abs

#see in Dockerfile
SONAME=multiabs_udf.so
	
function-create:
	mysql -h 127.0.0.1 -u root -proot --port 3306 -e "CREATE FUNCTION $(FUNCTION_NAME) RETURNS INTEGER SONAME '$(SONAME)'"

function-drop:
	mysql -h 127.0.0.1 -u root -proot --port 3306 -e "DROP FUNCTION $(FUNCTION_NAME)"

function-test:
	mysql -h 127.0.0.1 -u root -proot --port 3306 -e "SELECT $(FUNCTION_NAME)()"
	mysql -h 127.0.0.1 -u root -proot --port 3306 -e "SELECT $(FUNCTION_NAME)(12)"
	mysql -h 127.0.0.1 -u root -proot --port 3306 -e "SELECT $(FUNCTION_NAME)(8, 12)"
	mysql -h 127.0.0.1 -u root -proot --port 3306 -e "SELECT $(FUNCTION_NAME)('18', '32')"
		
	