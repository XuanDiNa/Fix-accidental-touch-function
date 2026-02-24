add_rules("mode.debug", "mode.release")

target("FixAccidentalTouch")
    set_kind("shared")
    add_files("src/**.cpp")
    add_includedirs("src")
    set_languages("c++20")
    set_strip("all")
    
    if is_plat("windows") then
        add_cxflags("/utf-8", "/EHa")
        add_syslinks("Shell32", "User32")
    end
