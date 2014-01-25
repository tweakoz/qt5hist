TEMPLATE = subdirs

# These examples contain some C++ and need to be built
SUBDIRS = \
    animation \
    calculator \
    cppextensions \
    flickr \
    i18n \
    imageelements \
    keyinteraction/focus \
    minehunt \
    modelviews \
    photoviewer \
    positioners \
    righttoleft \
    rssnews \
    samegame \
    shadereffects \
    snake \
    sqllocalstorage \
    text \
    threading \
    touchinteraction \
    toys \
    tutorials \
    twitter \
    ui-components

# OpenGL shader examples requires opengl and they contain some C++ and need to be built
contains(QT_CONFIG, opengl): SUBDIRS += shadereffects

# These examples contain no C++ and can simply be copied
EXAMPLE_FILES = \
    helper \
    screenorientation \
    xml