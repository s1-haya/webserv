IMAGE_NAME := nginx-config-test
CONTAINER_NAME := nginx-config-test-container
BUILD_CONTEXT := ../../

.PHONY	: build
build:
	@cd $(BUILD_CONTEXT) && docker build -f test/config/Dockerfile -t $(IMAGE_NAME) .

.PHONY	: run
run:
	@docker run -d --name $(CONTAINER_NAME) -v ./conf_file:/etc/nginx/conf_file $(IMAGE_NAME)

.PHONY	: stop
stop:
	-@docker stop $(CONTAINER_NAME)

.PHONY	: rm
rm:
	-@docker rm $(CONTAINER_NAME)

.PHONY	: clean
clean:
	-@make stop && make rm

.PHONY	: fclean
fclean:
	-@make clean && docker rmi $(IMAGE_NAME)

.PHONY	: log
log:
	@docker logs $(CONTAINER_NAME)

.PHONY	: ps
ps:
	@docker ps -a

.PHONY	: check_container
check_container:
	@docker inspect -f '{{.State.Status}}' $(CONTAINER_NAME) 2>/dev/null | grep -q 'running' || $(MAKE) run

.PHONY	: login
login: check_container
	@docker exec -it $(CONTAINER_NAME) /bin/bash;

.PHONY	: re
re: fclean build
