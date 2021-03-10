#include <jni.h>
#include <sys/types.h>
#include <riru.h>
#include <malloc.h>
#include <cstring>

static void forkAndSpecializePre(
        JNIEnv *env, jclass clazz, jint *uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jintArray *fdsToClose, jintArray *fdsToIgnore, jboolean *is_child_zygote,
        jstring *instructionSet, jstring *appDataDir, jboolean *isTopApp, jobjectArray *pkgDataInfoList,
        jobjectArray *whitelistedDataInfoList, jboolean *bindMountAppDataDirs, jboolean *bindMountAppStorageDirs) {
}

static void forkAndSpecializePost(JNIEnv *env, jclass clazz, jint res) {
    if (res == 0) {
        // in app process
    } else {
        // in zygote process, res is child pid
        // don't print log here, see https://github.com/RikkaApps/Riru/blob/77adfd6a4a6a81bfd20569c910bc4854f2f84f5e/riru-core/jni/main/jni_native_method.cpp#L55-L66
    }
}

static void specializeAppProcessPre(
        JNIEnv *env, jclass clazz, jint *uid, jint *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jint *mountExternal, jstring *seInfo, jstring *niceName,
        jboolean *startChildZygote, jstring *instructionSet, jstring *appDataDir,
        jboolean *isTopApp, jobjectArray *pkgDataInfoList, jobjectArray *whitelistedDataInfoList,
        jboolean *bindMountAppDataDirs, jboolean *bindMountAppStorageDirs) {
}

static void specializeAppProcessPost(
        JNIEnv *env, jclass clazz) {
    // added from Android 10, but disabled at least in Google Pixel devices
}

static void forkSystemServerPre(
        JNIEnv *env, jclass clazz, uid_t *uid, gid_t *gid, jintArray *gids, jint *runtimeFlags,
        jobjectArray *rlimits, jlong *permittedCapabilities, jlong *effectiveCapabilities) {
}

static void forkSystemServerPost(JNIEnv *env, jclass clazz, jint res) {
    if (res == 0) {
        // in system server process
    } else {
        // in zygote process, res is child pid
        // don't print log here, see https://github.com/RikkaApps/Riru/blob/77adfd6a4a6a81bfd20569c910bc4854f2f84f5e/riru-core/jni/main/jni_native_method.cpp#L55-L66
    }
}

static int shouldSkipUid(int uid) {
    // By default (the module does not provide this function in init), Riru will only call
    // module functions in "normal app processes" (10000 <= uid % 100000 <= 19999)

    // Provide this function so that the module can control if a specific uid should be skipped
    return false;
}

static void onModuleLoaded() {
    // called when the shared library of Riru core is loaded
}

static RiruVersionedModuleInfo *module = nullptr;

extern "C" {

int riru_api_version;
RiruApi *riru_api = nullptr;
const char *riru_magisk_module_path = nullptr;

#ifndef RIRU_MODULE_LEGACY_INIT
RiruVersionedModuleInfo *init(Riru *riru) {
    if (module == nullptr) {
        module = (RiruVersionedModuleInfo *) malloc(sizeof(RiruVersionedModuleInfo));
        memset(module, 0, sizeof(RiruModuleInfo));
        module->moduleInfo.supportHide = true;
        module->moduleInfo.version = RIRU_MODULE_VERSION;
        module->moduleInfo.versionName = RIRU_MODULE_VERSION_NAME;
        module->moduleInfo.onModuleLoaded = onModuleLoaded;
        module->moduleInfo.shouldSkipUid = shouldSkipUid;
        module->moduleInfo.forkAndSpecializePre = forkAndSpecializePre;
        module->moduleInfo.forkAndSpecializePost = forkAndSpecializePost;
        module->moduleInfo.specializeAppProcessPre = specializeAppProcessPre;
        module->moduleInfo.specializeAppProcessPost = specializeAppProcessPost;
        module->moduleInfo.forkSystemServerPre = forkSystemServerPre;
        module->moduleInfo.forkSystemServerPost = forkSystemServerPost;
    }

    auto core_max_api_version = riru->riruApiVersion;
    if (core_max_api_version < RIRU_MODULE_MIN_API_VERSION) {
        // This is possible if the user downgrade Riru
        return nullptr;
    }
    riru_api_version = core_max_api_version <= RIRU_MODULE_API_VERSION ? core_max_api_version : RIRU_MODULE_API_VERSION;
    module->moduleApiVersion = riru_api_version;

    riru_api = riru->riruApi;
    riru_magisk_module_path = riru->magiskModulePath;
    return module;
}
#else
RiruVersionedModuleInfo *init(Riru *riru) {
    static int step = 0;
    step += 1;

    if (module == nullptr) {
        module = (RiruVersionedModuleInfo *) malloc(sizeof(RiruVersionedModuleInfo));
        memset(module, 0, sizeof(RiruModuleInfo));
        module->moduleInfo.supportHide = true;
        module->moduleInfo.version = RIRU_MODULE_VERSION;
        module->moduleInfo.versionName = RIRU_MODULE_VERSION_NAME;
        module->moduleInfo.onModuleLoaded = onModuleLoaded;
        module->moduleInfo.shouldSkipUid = shouldSkipUid;
        module->moduleInfo.forkAndSpecializePre = forkAndSpecializePre;
        module->moduleInfo.forkAndSpecializePost = forkAndSpecializePost;
        module->moduleInfo.specializeAppProcessPre = specializeAppProcessPre;
        module->moduleInfo.specializeAppProcessPost = specializeAppProcessPost;
        module->moduleInfo.forkSystemServerPre = forkSystemServerPre;
        module->moduleInfo.forkSystemServerPost = forkSystemServerPost;
    }

    switch (step) {
        case 1: {
            auto core_max_api_version = riru->riruApiVersion;
            if (core_max_api_version < RIRU_MODULE_MIN_API_VERSION) {
                // This is possible if the user downgrade Riru
                return nullptr;
            }

            riru_api_version = core_max_api_version <= RIRU_MODULE_API_VERSION ? core_max_api_version : RIRU_MODULE_API_VERSION;
            if (riru_api_version >= 24) {
                module->moduleApiVersion = riru_api_version;
                riru_api = riru->riruApi;
                riru_magisk_module_path = riru->magiskModulePath;
                return module;
            } else {
                return (RiruVersionedModuleInfo *) &riru_api_version;
            }
        }
        case 2: {
            riru_api = (RiruApi *) riru;
            return (RiruVersionedModuleInfo *) &module->moduleInfo;
        }
        default: {
            return nullptr;
        }
    }
}
#endif
}