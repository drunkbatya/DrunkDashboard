#include "version.h"
#include <furi.h>

#ifndef GIT_COMMIT
#define GIT_COMMIT "unknown"
#endif

#ifndef GIT_BRANCH
#define GIT_BRANCH "unknown"
#endif

#ifndef GIT_BRANCH_NUM
#define GIT_BRANCH_NUM "0"
#endif

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__ " " __TIME__
#endif

#ifndef VERSION
#define VERSION "0.0.0"
#endif

#ifndef TARGET
#define TARGET 1
#endif

#ifndef BUILD_DIRTY
#define BUILD_DIRTY false
#endif

#ifndef FIRMWARE_ORIGIN
#define FIRMWARE_ORIGIN "DrunkDashboard"
#endif

#ifndef GIT_ORIGIN
#define GIT_ORIGIN "local"
#endif

#define VERSION_MAGIC (0xBE40u)
#define VERSION_MAJOR (0x1u)
#define VERSION_MINOR (0x1u)

struct Version {
    // Header
    const uint16_t magic;
    const uint8_t major;
    const uint8_t minor;
    // Payload
    const char* git_hash;
    const char* git_branch;
    const char* build_date;
    const char* version;
    // Payload bits and pieces
    const uint8_t target;
    const bool build_is_dirty;
    // v 1.1
    const char* firmware_origin;
    const char* git_origin;
};

/* version of current running firmware (bootloader/flipper) */
static const Version version = {
    .magic = VERSION_MAGIC,
    .major = VERSION_MAJOR,
    .minor = VERSION_MINOR,
    .git_hash = GIT_COMMIT,
    .git_branch = GIT_BRANCH,
    .build_date = BUILD_DATE,
    .version = VERSION
#ifdef FURI_RAM_EXEC
    " (RAM)"
#endif
    ,
    .target = TARGET,
    .build_is_dirty = BUILD_DIRTY,
    .firmware_origin = FIRMWARE_ORIGIN,
    .git_origin = GIT_ORIGIN,
};

const Version* version_get(void) {
    return &version;
}

const char* version_get_githash(const Version* v) {
    return v ? v->git_hash : version.git_hash;
}

const char* version_get_gitbranch(const Version* v) {
    return v ? v->git_branch : version.git_branch;
}

const char* version_get_gitbranchnum(const Version* v) {
    UNUSED(v);
    return GIT_BRANCH_NUM;
}

const char* version_get_builddate(const Version* v) {
    return v ? v->build_date : version.build_date;
}

const char* version_get_version(const Version* v) {
    return v ? v->version : version.version;
}

uint8_t version_get_target(const Version* v) {
    return v ? v->target : version.target;
}

bool version_get_dirty_flag(const Version* v) {
    return v ? v->build_is_dirty : version.build_is_dirty;
}

const char* version_get_firmware_origin(const Version* v) {
    return v ? v->firmware_origin : version.firmware_origin;
}

const char* version_get_git_origin(const Version* v) {
    return v ? v->git_origin : version.git_origin;
}
