#!/usr/bin/env bash
cmake -DOpenGL_GL_PREFERENCE=GLVND -DCMAKE_BUILD_TYPE=Debug -S . -B build
