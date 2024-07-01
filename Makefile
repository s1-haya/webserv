NAME		:=	webserv

SRC_DIR		:=	srcs
SRC_DIRS	:=	$(shell find $(SRC_DIR) -type d)
SRCS		:=	$(shell find $(SRC_DIR) -type f -name "*.cpp")

OBJ_DIR		:=	objs
OBJS		:=	$(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

INCLUDES	:=	$(addprefix -I,$(SRC_DIRS))

CXX			:=	c++
CXXFLAGS	:=	-std=c++98 -Wall -Wextra -Werror -MMD -MP -pedantic

DEPS		:=	$(OBJS:.o=.d)
MKDIR		:=	mkdir -p

#--------------------------------------------

.PHONY	: all
all		: $(NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

.PHONY	: clean
clean:
	$(RM) -r $(OBJ_DIR)

.PHONY	: fclean
fclean: clean
	$(RM) $(NAME)

.PHONY	: re
re: fclean all

#--------------------------------------------

PATH_CONFIG	:=	default.conf

.PHONY	: run
run: all
	@./$(NAME) $(PATH_CONFIG)

.PHONY	: val
val: all
	@valgrind ./$(NAME) $(PATH_CONFIG)

.PHONY	: check
check:
	@make check-srcs
	@make check-test

.PHONY	: check-srcs
check-srcs:
	@cppcheck --enable=all srcs

.PHONY	: check-test
check-test:
	@mypy test

.PHONY	: format
format:
	@make format-srcs
	@make format-test
	@echo "srcs and test format done!"

.PHONY	: format-srcs
format-srcs:
	@find srcs -name "*.cpp" -exec clang-format -i {} +
	@find srcs -name "*.hpp" -exec clang-format -i {} +

.PHONY	: format-test
format-test:
	@black test
	@isort test

.PHONY	: test
test:
	@make unit && make e2e

.PHONY	: debug
debug:
	@pytest -sv ./test/unit
	@pytest -sv ./test/integration

.PHONY	: unit
unit:
	@pytest -v ./test/unit

.PHONY	: e2e
e2e:
	@pytest -v ./test/integration

#--------------------------------------------

WEBSERV_IMAGE_NAME := webserv-image
WEBSERV_IMAGE_TAG := production
WEBSERV_CONTAINER_NAME := webserv-container
WEBSERV_PORT := 8080

.PHONY	: build-webserv
build-webserv:
	@docker build -t $(WEBSERV_IMAGE_NAME):$(WEBSERV_IMAGE_TAG) .; \

.PHONY	: run-webserv
run-webserv:
	@if [ -z "$$(docker images -q $(WEBSERV_IMAGE_NAME):$(WEBSERV_IMAGE_TAG))" ]; then \
		echo "$(WEBSERV_IMAGE_NAME):$(WEBSERV_IMAGE_TAG) not found. Building the image..."; \
		make build-webserv; \
	else \
		echo "$(WEBSERV_IMAGE_NAME):$(WEBSERV_IMAGE_TAG) found."; \
	fi
	@sleep 1
	@if [ -z "$$(docker ps -aqf name=$(WEBSERV_CONTAINER_NAME))" ]; then \
		docker run -d --name $(WEBSERV_CONTAINER_NAME) -p $(WEBSERV_PORT):$(WEBSERV_PORT) $(WEBSERV_IMAGE_NAME):$(WEBSERV_IMAGE_TAG); \
	elif [ -z "$$(docker ps -qf name=$(WEBSERV_CONTAINER_NAME))" ]; then \
		docker start $(WEBSERV_CONTAINER_NAME); \
	else \
		echo "$(WEBSERV_CONTAINER_NAME) is already running."; \
	fi

.PHONY	: re-run-webserv
re-run-webserv:
	@make fclean-webserv && make run-webserv

.PHONY	: stop-webserv
stop-webserv:
	@if [ -z "$$(docker ps -qf name=$(WEBSERV_CONTAINER_NAME))" ]; then \
		echo "$(WEBSERV_CONTAINER_NAME) already stopped."; \
	else \
		docker stop $(WEBSERV_CONTAINER_NAME); \
	fi

.PHONY	: rm-webserv
rm-webserv:
	@if [ -z "$$(docker ps -aqf name=$(WEBSERV_CONTAINER_NAME))" ]; then \
		echo "$(WEBSERV_CONTAINER_NAME) does not exist."; \
	else \
		docker rm $(WEBSERV_CONTAINER_NAME); \
	fi

.PHONY	: clean-webserv
clean-webserv:
	@make stop-webserv && make rm-webserv

.PHONY	: fclean-webserv
fclean-webserv:
	@make clean-webserv
	@if [ -z "$$(docker image ls -aqf reference=$(WEBSERV_IMAGE_NAME):$(WEBSERV_IMAGE_TAG))" ]; then \
		echo "$(WEBSERV_IMAGE_NAME):$(WEBSERV_IMAGE_TAG) does not exist."; \
	else \
		docker rmi $(WEBSERV_IMAGE_NAME):$(WEBSERV_IMAGE_TAG); \
	fi

.PHONY	: log-webserv
log-webserv:
	@if [ -z "$$(docker ps -qf name=$(WEBSERV_CONTAINER_NAME))" ]; then \
		echo "$(WEBSERV_CONTAINER_NAME) does not exist."; \
	else \
		docker logs $(WEBSERV_CONTAINER_NAME); \
	fi

.PHONY	: login-webserv
login-webserv:
	@if [ -z "$$(docker ps -qf name=$(WEBSERV_CONTAINER_NAME))" ]; then \
		echo "$(WEBSERV_CONTAINER_NAME) does not exist."; \
	else \
		docker exec -it $(WEBSERV_CONTAINER_NAME) /bin/bash; \
	fi

#--------------------------------------------

DEV_IMAGE_NAME := dev-image
DEV_IMAGE_TAG := development
DEV_CONTAINER_NAME := dev-container
DEV_PORT := 49152

.PHONY	: build-dev
build-dev:
	@docker build -t $(DEV_IMAGE_NAME):$(DEV_IMAGE_TAG) -f .devcontainer/Dockerfile .; \

.PHONY	: run-dev
run-dev:
	@if [ -z "$$(docker images -q $(DEV_IMAGE_NAME):$(DEV_IMAGE_TAG))" ]; then \
		echo "$(DEV_IMAGE_NAME):$(DEV_IMAGE_TAG) not found. Building the image..."; \
		make build-dev; \
	else \
		echo "$(DEV_IMAGE_NAME):$(DEV_IMAGE_TAG) found."; \
	fi
	@sleep 1
	@if [ -z "$$(docker ps -aqf name=$(DEV_CONTAINER_NAME))" ]; then \
		docker run -it --name $(DEV_CONTAINER_NAME) -p $(DEV_PORT):$(WEBSERV_PORT) $(DEV_IMAGE_NAME):$(DEV_IMAGE_TAG); \
	elif [ -z "$$(docker ps -qf name=$(DEV_CONTAINER_NAME))" ]; then \
		docker start $(DEV_CONTAINER_NAME); \
	else \
		echo "$(DEV_CONTAINER_NAME) is already running."; \
	fi

.PHONY	: re-run-dev
re-run-dev:
	@make fclean-dev && make run-dev

.PHONY	: stop-dev
stop-dev:
	@if [ -z "$$(docker ps -qf name=$(DEV_CONTAINER_NAME))" ]; then \
		echo "$(DEV_CONTAINER_NAME) already stopped."; \
	else \
		docker stop $(DEV_CONTAINER_NAME); \
	fi

.PHONY	: rm-dev
rm-dev:
	@if [ -z "$$(docker ps -aqf name=$(DEV_CONTAINER_NAME))" ]; then \
		echo "$(DEV_CONTAINER_NAME) does not exist."; \
	else \
		docker rm $(DEV_CONTAINER_NAME); \
	fi

.PHONY	: clean-dev
clean-dev:
	@make stop-dev && make rm-dev

.PHONY	: fclean-dev
fclean-dev:
	@make clean-dev
	@if [ -z "$$(docker image ls -aqf reference=$(DEV_IMAGE_NAME):$(DEV_IMAGE_TAG))" ]; then \
		echo "$(DEV_IMAGE_NAME):$(DEV_IMAGE_TAG) does not exist."; \
	else \
		docker rmi $(DEV_IMAGE_NAME):$(DEV_IMAGE_TAG); \
	fi

.PHONY	: log-dev
log-dev:
	@if [ -z "$$(docker ps -qf name=$(DEV_CONTAINER_NAME))" ]; then \
		echo "$(DEV_CONTAINER_NAME) does not exist."; \
	else \
		docker logs $(DEV_CONTAINER_NAME); \
	fi

.PHONY	: login-dev
login-dev:
	@if [ -z "$$(docker ps -qf name=$(DEV_CONTAINER_NAME))" ]; then \
		echo "$(DEV_CONTAINER_NAME) does not exist."; \
	else \
		docker exec -it $(DEV_CONTAINER_NAME) /bin/bash; \
	fi

.PHONY	: ps
ps:
	@docker ps -a

#--------------------------------------------

-include $(DEPS)
