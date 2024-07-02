IMAGE_NAME := apache
CONTAINER_NAME := apache-container
BUILD_CONTEXT := ../../

.PHONY	: build
build:
	@cd $(BUILD_CONTEXT) && docker build -f test/apache/Dockerfile -t $(IMAGE_NAME) .

.PHONY	: run
run:
	@docker run -d --name $(CONTAINER_NAME) -p 4242:80 $(IMAGE_NAME)

.PHONY	: stop
stop:
	@docker stop $(CONTAINER_NAME)

.PHONY	: rm
rm:
	@docker rm $(CONTAINER_NAME)

.PHONY	: clean
clean:
	@make stop && make rm

.PHONY	: fclean
fclean:
	@make clean && docker rmi $(IMAGE_NAME)

.PHONY	: log
log:
	@docker logs $(CONTAINER_NAME)

.PHONY	: ps
ps:
	@docker ps -a

.PHONY	: login
login:
	@docker exec -it $(CONTAINER_NAME) /bin/bash
