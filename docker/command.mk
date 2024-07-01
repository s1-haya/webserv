include docker/config.mk

.PHONY	: build
build:
	@docker build -t $(IMAGE_NAME):$(IMAGE_TAG) -f docker/Dockerfile .; \

.PHONY	: run-webserv
run-webserv:
	@if [ -z "$$(docker images -q $(IMAGE_NAME))" ]; then \
		echo "$(IMAGE_NAME) not found. Building the image..."; \
		make build; \
	else \
		echo "$(IMAGE_NAME) found."; \
	fi
	@sleep 1
	@if [ -z "$$(docker ps -aqf name=$(CONTAINER_NAME))" ]; then \
		docker run -d --name $(CONTAINER_NAME) -p $(PORT):$(PORT) $(IMAGE_NAME); \
	else \
		echo "$(CONTAINER_NAME) is already running."; \
	fi

.PHONY	: stop
stop:
	@if [ -z "$$(docker ps -qf name=$(CONTAINER_NAME))" ]; then \
		echo "$(CONTAINER_NAME) already stopped."; \
	else \
		docker stop $(CONTAINER_NAME); \
	fi

.PHONY	: rm
rm:
	@if [ -z "$$(docker ps -aqf name=$(CONTAINER_NAME))" ]; then \
		echo "$(CONTAINER_NAME) does not exist."; \
	else \
		docker rm $(CONTAINER_NAME); \
	fi

.PHONY	: clean-webserv
clean-webserv:
	@make stop && make rm

.PHONY	: rmi
rmi:
	@make clean-webserv
	@if [ -z "$$(docker image ls -aqf reference=$(IMAGE_NAME):$(IMAGE_TAG))" ]; then \
		echo "$(IMAGE_NAME) does not exist."; \
	else \
		docker rmi $(IMAGE_NAME):$(IMAGE_TAG); \
	fi

.PHONY	: log
log:
	@if [ -z "$$(docker ps -qf name=$(CONTAINER_NAME))" ]; then \
		echo "$(CONTAINER_NAME) does not exist."; \
	else \
		docker logs $(CONTAINER_NAME); \
	fi

.PHONY	: ps
ps:
	@docker ps -a

.PHONY	: login
login:
	@if [ -z "$$(docker ps -qf name=$(CONTAINER_NAME))" ]; then \
		echo "$(CONTAINER_NAME) does not exist."; \
	else \
		docker exec -it $(CONTAINER_NAME) /bin/bash; \
	fi
