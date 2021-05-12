//
// Copyright 2021 The ANGLE Project Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// CLPlatform.h: Defines the cl::Platform class, which provides information about platform-specific
// OpenCL features.

#ifndef LIBANGLE_CLPLATFORM_H_
#define LIBANGLE_CLPLATFORM_H_

#include "libANGLE/CLDevice.h"
#include "libANGLE/renderer/CLPlatformImpl.h"

#include "anglebase/no_destructor.h"

namespace cl
{

class Platform final : public _cl_platform_id, public Object
{
  public:
    using Ptr     = std::unique_ptr<Platform>;
    using PtrList = std::list<Ptr>;

    ~Platform();

    bool hasDevice(const Device *device) const;
    const Device::PtrList &getDevices() const;

    cl_int getInfo(PlatformInfo name, size_t valueSize, void *value, size_t *sizeRet);

    cl_int getDeviceIDs(cl_device_type deviceType,
                        cl_uint numEntries,
                        Device **devices,
                        cl_uint *numDevices) const;

    static void CreatePlatform(const cl_icd_dispatch &dispatch,
                               rx::CLPlatformImpl::InitData &initData);
    static const PtrList &GetPlatforms();
    static Platform *GetDefault();
    static bool IsValid(const Platform *platform);
    static bool IsValidOrDefault(const Platform *platform);

    static constexpr const char *GetVendor();

  private:
    Platform(const cl_icd_dispatch &dispatch,
             rx::CLPlatformImpl::InitData &initData,
             rx::CLDeviceImpl::InitList &&deviceInitList);

    static PtrList &GetList();

    const rx::CLPlatformImpl::Ptr mImpl;
    const rx::CLPlatformImpl::Info mInfo;
    const Device::PtrList mDevices;

    static constexpr char kVendor[]    = "ANGLE";
    static constexpr char kIcdSuffix[] = "ANGLE";
};

inline bool Platform::hasDevice(const Device *device) const
{
    return std::find_if(mDevices.cbegin(), mDevices.cend(), [=](const Device::Ptr &ptr) {
               return ptr.get() == device || ptr->hasSubDevice(device);
           }) != mDevices.cend();
}

inline const Device::PtrList &Platform::getDevices() const
{
    return mDevices;
}

inline Platform::PtrList &Platform::GetList()
{
    static angle::base::NoDestructor<PtrList> sList;
    return *sList;
}

inline const Platform::PtrList &Platform::GetPlatforms()
{
    return GetList();
}

inline Platform *Platform::GetDefault()
{
    return GetList().empty() ? nullptr : GetList().front().get();
}

inline bool Platform::IsValid(const Platform *platform)
{
    const PtrList &platforms = GetPlatforms();
    return std::find_if(platforms.cbegin(), platforms.cend(),
                        [=](const Ptr &ptr) { return ptr.get() == platform; }) != platforms.cend();
}

// Our CL implementation defines that a nullptr value chooses the platform that we provide as
// default, so this function returns true for a nullptr value if a default platform exists.
inline bool Platform::IsValidOrDefault(const Platform *platform)
{
    return platform != nullptr ? IsValid(platform) : GetDefault() != nullptr;
}

constexpr const char *Platform::GetVendor()
{
    return kVendor;
}

}  // namespace cl

#endif  // LIBANGLE_CLPLATFORM_H_