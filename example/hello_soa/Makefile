CXX = g++
BACKEND=libevent
BUILDCHAIN = make

update-dockerfile:
	cd chat && make -e update-dockerfile
	cd user-service && make -e update-dockerfile
	cd session-service && make -e update-dockerfile

build: update-dockerfile ## run the docker image and open a shell
	cd chat && make -e build
	cd user-service && make -e build
	cd session-service && make -e build

up: update-dockerfile ## run the docker image and open a shell
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose up -d --build

force: update-dockerfile ## run the docker image and open a shell
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose build --no-cache
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose up -d --build

clean:
	docker-compose down
	docker-compose rm -f

down: ## stop running docker image, if any
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose stop

clean-logs:
	sh -c "truncate -s 0 /var/lib/docker/containers/*/*-json.log"
	
cacert :
	cd ca && bin/make_pki

.PHONY: build up down update-dockerfile cacert
