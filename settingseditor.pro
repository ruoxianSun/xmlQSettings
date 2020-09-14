QT += widgets
requires(qtConfig(tablewidget))

HEADERS       = locationdialog.h \
                csettings.h \
                csettingstree.h \
                formsettingseditor.h \
                mainwindow.h \
                settings.h \
                settingstree.h \
                variantdelegate.h
SOURCES       = locationdialog.cpp \
                csettings.cpp \
                csettingstree.cpp \
                formsettingseditor.cpp \
                main.cpp \
                mainwindow.cpp \
                settings.cpp \
                settingstree.cpp \
                variantdelegate.cpp

EXAMPLE_FILES = inifiles

# install
target.path = $$[QT_INSTALL_EXAMPLES]/widgets/tools/settingseditor
INSTALLS += target

FORMS += \
    formsettingseditor.ui
