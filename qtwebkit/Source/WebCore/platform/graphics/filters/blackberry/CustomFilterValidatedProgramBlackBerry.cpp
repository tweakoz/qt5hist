/*
 * Copyright (C) 2012 Research In Motion Limited. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer.
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER “AS IS” AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(CSS_SHADERS)

#include "CustomFilterValidatedProgram.h"

#include "LayerCompiledProgram.h"

namespace WebCore {

LayerCompiledProgram* CustomFilterValidatedProgram::platformCompiledProgram()
{
    ASSERT(m_isInitialized && !m_validatedVertexShader.isNull() && !m_validatedFragmentShader.isNull());

    if (!m_platformCompiledProgram)
        m_platformCompiledProgram = LayerCompiledProgram::create(m_validatedVertexShader, m_validatedFragmentShader).leakRef();

    return m_platformCompiledProgram;
}

void CustomFilterValidatedProgram::platformInit()
{
    m_platformCompiledProgram = 0;
}

void CustomFilterValidatedProgram::platformDestroy()
{
    derefIfNotNull(m_platformCompiledProgram);
}

} // namespace WebCore

#endif // ENABLE(CSS_SHADERS)
