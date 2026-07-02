GIT_COMMIT := $(shell git rev-parse --short HEAD 2>/dev/null || echo unknown)
GIT_BRANCH := $(shell git rev-parse --abbrev-ref HEAD 2>/dev/null || echo unknown)
BUILD_DATE := $(shell date '+%Y-%m-%d' 2>/dev/null || echo unknown)
BUILD_TIME := $(shell date '+%H:%M:%S' 2>/dev/null || echo unknown)

