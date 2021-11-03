/*
 * Copyright (C) 2021 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "android.hardware.biometrics.fingerprint@2.3-service.x2"

#include "BiometricsFingerprint.h"

#include <android-base/logging.h>
#include <inttypes.h>
#include <unistd.h>
#include <fstream>
#include <thread>

#define FOD_STATUS_PATH "/sys/kernel/oppo_display/notify_fppress"
#define DIMLAYER_PATH "/sys/kernel/oppo_display/dimlayer_hbm"
#define STATUS_ON 1
#define STATUS_OFF 0

#define DEBUG_ADAPTOR 1

namespace android {
namespace hardware {
namespace biometrics {
namespace fingerprint {
namespace V2_3 {
namespace implementation {

template <typename T>
static inline void set(const std::string& path, const T& value) {
    std::ofstream file(path);
    file << value;
    if(DEBUG_ADAPTOR) {
        LOG (INFO) << "set(): value: " << value << " to path: " << path;
    }
}

BiometricsFingerprint::BiometricsFingerprint() : isEnrolling(false) {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint() initialising constructor.";
    }
    mOppoBiometricsFingerprint = vendor::oppo::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint::getService();
    if (mOppoBiometricsFingerprint == nullptr) {
        LOG (ERROR) << "BiometricsFingerprint(): Oppo Biometrics hal didn't init trying again, Attempting rescue.";
        for (int i = 0; i < 10; i++) {
            mOppoBiometricsFingerprint = vendor::oppo::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprint::getService();
            if (mOppoBiometricsFingerprint != nullptr) {
                LOG (INFO) << "BiometricsFingerprint(): rescue(): Got service for oppo biometrics hal exiting loop.";
                break;
            }
            LOG (INFO) << "BiometricsFingerprint(): rescue(): Unable to get service in witin: " << i+1 << " attempts.";
            sleep(15);
        }
        if (mOppoBiometricsFingerprint == nullptr) {
            LOG(ERROR) << "BiometricsFingerprint(): oppo biometrics service didn't start fingerprint hardware won't be available.";
            exit(0);
        }
    }
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint() Exiting constructor successfully.";
    }
}

class OppoClientCallback : public vendor::oppo::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprintClientCallback {
public:
    OppoClientCallback(sp<android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprintClientCallback> clientCallback) : mClientCallback(clientCallback) {}
    Return<void> onEnrollResult(uint64_t deviceId, uint32_t fingerId,
        uint32_t groupId, uint32_t remaining) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onEnrollResult(): called";
        }
        return mClientCallback->onEnrollResult(deviceId, fingerId, groupId, remaining);
    }

    Return<void> onAcquired(uint64_t deviceId, vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo acquiredInfo,
        int32_t vendorCode) {
        std::string acquiredInfoString = "UNKNOWN";
        switch (acquiredInfo) {
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_GOOD:
                acquiredInfoString = "ACQUIRED_GOOD";
                break;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_PARTIAL:
                acquiredInfoString = "ACQUIRED_PARTIAL";
                break;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_INSUFFICIENT:
                acquiredInfoString = "ACQUIRED_INSUFFICIENT";
                break;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_IMAGER_DIRTY:
                acquiredInfoString = "ACQUIRED_IMAGER_DIRTY";
                break;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_TOO_SLOW:
                acquiredInfoString = "ACQUIRED_TOO_SLOW";
                break;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_TOO_FAST:
                acquiredInfoString = "ACQUIRED_TOO_FAST";
                break;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_VENDOR:
                acquiredInfoString = "ACQUIRED_VENDOR";
                break;
            default:
                break;
        }
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onAcquired(): called with acquiredInfo: " << acquiredInfoString << " vendorCode: " << vendorCode << " deviceID: " << deviceId;
        }
        return mClientCallback->onAcquired(deviceId, OppoToAOSPFingerprintAcquiredInfo(acquiredInfo), vendorCode);
    }

    Return<void> onAuthenticated(uint64_t deviceId, uint32_t fingerId, uint32_t groupId,
        const hidl_vec<uint8_t>& token) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onAuthenticated(): called setting isAuthComplete to true";
        }
        isAuthComplete = true;
        return mClientCallback->onAuthenticated(deviceId, fingerId, groupId, token);
    }

    Return<void> onError(uint64_t deviceId, vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError error, int32_t vendorCode) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onError(): called";
        }
        return mClientCallback->onError(deviceId, OppoToAOSPFingerprintError(error), vendorCode);
    }

    Return<void> onRemoved(uint64_t deviceId, uint32_t fingerId, uint32_t groupId,
        uint32_t remaining) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onRemoved(): called";
        }
        return mClientCallback->onRemoved(deviceId, fingerId, groupId, remaining);
    }

    Return<void> onEnumerate(uint64_t deviceId, uint32_t fingerId, uint32_t groupId,
        uint32_t remaining) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onEnumerate(): called";
        }
        return mClientCallback->onEnumerate(deviceId, fingerId, groupId, remaining);
    }

    Return<void> onTouchUp(uint64_t deviceId) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onTouchUp(): called";
        }
        set(FOD_STATUS_PATH, STATUS_ON);
        isAuthComplete = false;
        return Void();
    }
    Return<void> onTouchDown(uint64_t deviceId) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onTouchDown(): called";
        }
        return Void();
    }
    Return<void> onSyncTemplates(uint64_t deviceId, const hidl_vec<uint32_t>& fingerId, uint32_t remaining) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onSyncTemplates(): called";
        }
        return Void();
    }
    Return<void> onFingerprintCmd(int32_t deviceId, const hidl_vec<uint32_t>& groupId, uint32_t remaining) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onFingerprintCmd(): called";
        }
        return Void(); 
    }
    Return<void> onImageInfoAcquired(uint32_t type, uint32_t quality, uint32_t match_score) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onImageInfoAcquired(): called";
        }
        return Void();
    }
    Return<void> onMonitorEventTriggered(uint32_t type, const hidl_string& data) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onMonitorEventTriggered(): called";
        }
        return Void();
    }
    Return<void> onEngineeringInfoUpdated(uint32_t length, const hidl_vec<uint32_t>& keys, const hidl_vec<hidl_string>& values) { 
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "OppoClientCallback(): onEngineeringInfoUpdated(): called";
        }
        return Void();
    }

private:
    sp<android::hardware::biometrics::fingerprint::V2_1::IBiometricsFingerprintClientCallback> mClientCallback;

    Return<android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo> OppoToAOSPFingerprintAcquiredInfo(vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo info) {
        switch(info) {
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_GOOD: return android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_GOOD;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_PARTIAL: return android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_PARTIAL;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_INSUFFICIENT: return android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_INSUFFICIENT;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_IMAGER_DIRTY: return android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_IMAGER_DIRTY;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_TOO_SLOW: return android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_TOO_SLOW;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_TOO_FAST: return android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_TOO_FAST;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_VENDOR: return android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_VENDOR;
            default:
                return android::hardware::biometrics::fingerprint::V2_1::FingerprintAcquiredInfo::ACQUIRED_GOOD;
        }
    }

    Return<android::hardware::biometrics::fingerprint::V2_1::FingerprintError> OppoToAOSPFingerprintError(vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError error) {
        switch(error) {
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_NO_ERROR: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_NO_ERROR;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_HW_UNAVAILABLE: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_HW_UNAVAILABLE;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_UNABLE_TO_PROCESS: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_UNABLE_TO_PROCESS;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_TIMEOUT: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_TIMEOUT;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_NO_SPACE: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_NO_SPACE;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_CANCELED: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_CANCELED;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_UNABLE_TO_REMOVE: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_UNABLE_TO_REMOVE;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_LOCKOUT: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_LOCKOUT;
            case vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_VENDOR: return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_VENDOR;
            default:
                return android::hardware::biometrics::fingerprint::V2_1::FingerprintError::ERROR_NO_ERROR;
        }
    }
};

Return<uint64_t> BiometricsFingerprint::setNotify(const sp<IBiometricsFingerprintClientCallback>& clientCallback) {
    mOppoClientCallback = new OppoClientCallback(clientCallback);
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): setNotify(): was mOppoClientCallback invalid: " << (mOppoClientCallback == nullptr);
    }
    return mOppoBiometricsFingerprint->setNotify(mOppoClientCallback);
}

Return<RequestStatus> BiometricsFingerprint::OppoToAOSPRequestStatus(vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus req) {
    switch(req) {
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_UNKNOWN: return RequestStatus::SYS_UNKNOWN;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_OK: return RequestStatus::SYS_OK;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_ENOENT: return RequestStatus::SYS_ENOENT;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_EINTR: return RequestStatus::SYS_EINTR;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_EIO: return RequestStatus::SYS_EIO;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_EAGAIN: return RequestStatus::SYS_EAGAIN;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_ENOMEM: return RequestStatus::SYS_ENOMEM;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_EACCES: return RequestStatus::SYS_EACCES;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_EFAULT: return RequestStatus::SYS_EFAULT;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_EBUSY: return RequestStatus::SYS_EBUSY;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_EINVAL: return RequestStatus::SYS_EINVAL;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_ENOSPC: return RequestStatus::SYS_ENOSPC;
        case vendor::oppo::hardware::biometrics::fingerprint::V2_1::RequestStatus::SYS_ETIMEDOUT: return RequestStatus::SYS_ETIMEDOUT;
        default:
            return RequestStatus::SYS_UNKNOWN;
    }
}

void BiometricsFingerprint::setFingerprintScreenState(const bool on) {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): setFingerprintScreenState(): state: " << on;
    }
    mOppoBiometricsFingerprint->setScreenState(
        on ? vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintScreenState::FINGERPRINT_SCREEN_ON :
            vendor::oppo::hardware::biometrics::fingerprint::V2_1::FingerprintScreenState::FINGERPRINT_SCREEN_OFF
        );
    set(DIMLAYER_PATH, on ? STATUS_ON: STATUS_OFF);
}

Return<uint64_t> BiometricsFingerprint::preEnroll() {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): preEnroll(): called setFingerprintScreenState() to true";
    }
    setFingerprintScreenState(true);
    return mOppoBiometricsFingerprint->preEnroll();
}

Return<RequestStatus> BiometricsFingerprint::enroll(const hidl_array<uint8_t, 69>& hat, uint32_t gid, uint32_t timeoutSec) {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): enroll(): called. setting isEnrolling to true.";
    }
    isEnrolling = true;
    return OppoToAOSPRequestStatus(mOppoBiometricsFingerprint->enroll(hat, gid, timeoutSec));
}

Return<RequestStatus> BiometricsFingerprint::postEnroll() {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): postEnroll(): called. setting isEnrolling to false.";
    }
    isEnrolling = false;
    setFingerprintScreenState(isEnrolling);
    return OppoToAOSPRequestStatus(mOppoBiometricsFingerprint->postEnroll());
}

Return<uint64_t> BiometricsFingerprint::getAuthenticatorId() {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): getAuthenticatorId(): called.";
    }
    return mOppoBiometricsFingerprint->getAuthenticatorId();
}

Return<RequestStatus> BiometricsFingerprint::cancel() {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): cancel(): called was Enrolling: " << isEnrolling;
    }
    if (isEnrolling)
        isEnrolling = false;
    else {
        LOG (INFO) << "BiometricsFingerprint(): cancel(): called was not Enrolling: " << isEnrolling << " setting screenstate to false"; 
        setFingerprintScreenState(false);
    }
    return OppoToAOSPRequestStatus(mOppoBiometricsFingerprint->cancel());
}

Return<RequestStatus> BiometricsFingerprint::enumerate() {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): enumerate(): called.";
    }
    return OppoToAOSPRequestStatus(mOppoBiometricsFingerprint->enumerate());
}

Return<RequestStatus> BiometricsFingerprint::remove(uint32_t gid, uint32_t fid) {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): remove(): called.";
    }
    return OppoToAOSPRequestStatus(mOppoBiometricsFingerprint->remove(gid, fid));
}

Return<RequestStatus> BiometricsFingerprint::setActiveGroup(uint32_t gid, const hidl_string& storePath) {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): setActiveGroup(): called.";
    }
    return OppoToAOSPRequestStatus(mOppoBiometricsFingerprint->setActiveGroup(gid, storePath));
}

Return<RequestStatus> BiometricsFingerprint::authenticate(uint64_t operationId, uint32_t gid) {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): authenticate(): called.";
    }
    RequestStatus status = OppoToAOSPRequestStatus(mOppoBiometricsFingerprint->authenticate(operationId, gid));
    if (status == RequestStatus::SYS_OK) {
        if (DEBUG_ADAPTOR) {
            LOG (INFO) << "BiometricsFingerprint(): authenticate(): called. setting setFingerprintScreenState() to true";
        }
        setFingerprintScreenState(true);
    }
    return OppoToAOSPRequestStatus(mOppoBiometricsFingerprint->authenticate(operationId, gid));
}

Return<bool> BiometricsFingerprint::isUdfps(uint32_t) {
    return true;
}

Return<void> BiometricsFingerprint::onFingerDown(uint32_t, uint32_t, float, float) {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): onFingerDown(): called.";
    }
    return Void();
}

Return<void> BiometricsFingerprint::onFingerUp() {
    if (DEBUG_ADAPTOR) {
        LOG (INFO) << "BiometricsFingerprint(): onFingerUp(): Finger removal detected isEnrolling: " << isEnrolling << " isAuthComplete: " << isAuthComplete;
    }
    set(FOD_STATUS_PATH, STATUS_OFF);
    return Void();
}

}  // namespace implementation
}  // namespace V2_3
}  // namespace fingerprint
}  // namespace biometrics
}  // namespace hardware
}  // namespace android
