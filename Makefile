STUID = 211300046
STUNAME = 胡涛

# DO NOT modify the following code!!!

GITFLAGS = -q --author='tracer-ics2022 <tracer@njuics.org>' --no-verify --allow-empty

# prototype: git_commit(msg)
define git_commit
	-@git add $(NEMU_HOME)/.. -A --ignore-errors
	-@while (test -e .git/index.lock); do sleep 0.1; done
	-@(echo "> $(1)" && echo $(STUID) $(STUNAME) && uname -a && uptime) | git commit -F - $(GITFLAGS)
	-@sync
endef

_default:
	@echo "Please run 'make' under subprojects."

submit:
	git gc
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s http://why.ink:8080/static/submit.sh)"

COUNT_LINES := $(shell  find . -name "*.h" -o -name "*.c" | xargs grep -v "^$$" | wc -l)

#COUNT_FRAMEWORK :=20338

#COUNT_ADD := `expr $(COUNT_LINES)-$(COUNT_FRAMEWORK)`#, $(COUNT_ADD) lines added.

count:
	@echo There are $(COUNT_LINES) lines in nemu   

.PHONY: default submit
