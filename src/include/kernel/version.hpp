
#pragma once

namespace kernel {

struct KernelInfo {
    const char* name;
    struct VersionTag {
        int major;
        int minor;
    } version;
};

extern KernelInfo g_kernel_info;

}