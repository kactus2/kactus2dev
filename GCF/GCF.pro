######################################################################
# Generic Component Framework Library from VCreate Logic
######################################################################

include(config.pri)

message( $${GCF_LIBS} )

TARGET      = GCF
TEMPLATE    = subdirs
CONFIG     += ordered
SUBDIRS     = source components

isEmpty(PROJECT_BASE) {
    message("Did not find PROJECT_BASE variable. Library and DLLs will be created in " $$GCF_BASE)
    exists( $$GCF_BASE/tools/tools.pro ) {
        SUBDIRS += tools
    }
    
} else {
    message("Library and DLLs will be created in " $$PROJECT_BASE)
}

message()
win32 {
    message("    DLLs go to: " $$DLLDESTDIR)
    message("    LIBs go to: " $$DESTDIR)
} else {
    message("    Shared Objects go to: " $$DESTDIR)
}
message()



