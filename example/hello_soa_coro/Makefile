CXX = clang++
BACKEND=libevent
BUILDCHAIN = make

update-dockerfile:
	echo $(CXX)
	cd chat && make update-dockerfile CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN)
	cd user-service && make update-dockerfile CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN)
	cd session-service && make update-dockerfile CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN)

build: update-dockerfile ## run the docker image and open a shell
	cd chat && make build CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN)
	cd user-service && make build  CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN)
	cd session-service && make build CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN)

up: update-dockerfile ## run the docker image and open a shell
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose up -d --build

upforce : update-dockerfile
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose build --no-cache
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose up -d --build --force-recreate
	

down: ## stop running docker image, if any
	CXX=$(CXX) BACKEND=$(BACKEND) BUILDCHAIN=$(BUILDCHAIN) docker-compose stop

clean-logs:
	sh -c "truncate -s 0 /var/lib/docker/containers/*/*-json.log"
	
.PHONY: build up down update-dockerfile
