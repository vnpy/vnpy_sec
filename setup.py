import platform

from setuptools import Extension, setup


def get_ext_modules() -> list:
    """
    获取三方模块

    Windows需要编译封装接口
    Linux和Mac由于缺乏二进制库支持无法使用
    """

    extra_compile_flags = ["-O2", "-MT"]
    extra_link_args = []
    runtime_library_dirs = []

    vnsecmd = Extension(
        "vnpy_sec.api.vnsecmd",
        [
            "vnpy_sec/api/vnsec/vnsecmd/vnsecmd.cpp",
        ],
        include_dirs=["vnpy_sec/api/include",
                      "vnpy_sec/api/vnsec"],
        define_macros=[],
        undef_macros=[],
        library_dirs=["vnpy_sec/api/libs", "vnpy_sec/api"],
        libraries=["DFITCSECMdApi", "DFITCSECTraderApi", "HsFutuSystemInfo", "HsFutuSystemInfo"],
        extra_compile_args=extra_compile_flags,
        extra_link_args=extra_link_args,
        runtime_library_dirs=runtime_library_dirs,
        depends=[],
        language="cpp",
    )

    vnsectd = Extension(
        "vnpy_sec.api.vnsectd",
        [
            "vnpy_sec/api/vnsec/vnsectd/vnsectd.cpp",
        ],
        include_dirs=["vnpy_sec/api/include",
                      "vnpy_sec/api/vnsec"],
        define_macros=[],
        undef_macros=[],
        library_dirs=["vnpy_sec/api/libs", "vnpy_sec/api"],
        libraries=["DFITCSECMdApi", "DFITCSECTraderApi", "HsFutuSystemInfo", "HsFutuSystemInfo"],
        extra_compile_args=extra_compile_flags,
        extra_link_args=extra_link_args,
        runtime_library_dirs=runtime_library_dirs,
        depends=[],
        language="cpp",
    )

    return [vnsectd, vnsecmd]


setup(
    ext_modules=get_ext_modules(),
)
