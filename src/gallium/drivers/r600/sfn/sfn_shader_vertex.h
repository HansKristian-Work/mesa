/* -*- mesa-c++  -*-
 *
 * Copyright (c) 2018 Collabora LTD
 *
 * Author: Gert Wollny <gert.wollny@collabora.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * on the rights to use, copy, modify, merge, publish, distribute, sub
 * license, and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHOR(S) AND/OR THEIR SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef sfn_vertex_shader_from_nir_h
#define sfn_vertex_shader_from_nir_h

#include "sfn_shader_base.h"

namespace r600 {

class VertexShaderFromNir : public ShaderFromNirProcessor {
public:
   VertexShaderFromNir(r600_pipe_shader *sh,
                       r600_pipe_shader_selector &sel,
                       const r600_shader_key &key);

   bool do_emit_load_deref(const nir_variable *in_var, nir_intrinsic_instr* instr) override;
   bool scan_sysvalue_access(nir_instr *instr) override;
protected:
   bool emit_varying_pos(const nir_variable *out_var, nir_intrinsic_instr* instr,
                         std::array<uint32_t, 4> *swizzle_override = nullptr);
   bool emit_varying_param(const nir_variable *out_var, nir_intrinsic_instr* instr);
   bool emit_clip_vertices(const nir_variable *out_var, nir_intrinsic_instr* instr);
   bool emit_stream(int stream);

   // todo: encapsulate
   unsigned m_num_clip_dist;
   ExportInstruction *m_last_param_export;
   ExportInstruction *m_last_pos_export;
   r600_pipe_shader *m_pipe_shader;
   unsigned m_enabled_stream_buffers_mask;
   const pipe_stream_output_info *m_so_info;
   void do_finalize() override;
private:

   bool do_process_inputs(nir_variable *input) override;
   bool allocate_reserved_registers() override;
   bool do_process_outputs(nir_variable *output) override;
   bool emit_intrinsic_instruction_override(nir_intrinsic_instr* instr) override;

   virtual void finalize_exports() = 0;

   unsigned m_cur_param;
   std::map<unsigned, unsigned> m_param_map;
   unsigned m_cur_clip_pos;

   PValue m_vertex_id;
   PValue m_instance_id;
   r600_shader_key m_key;
};

class VertexShaderFromNirForFS :  public VertexShaderFromNir {
public:
   using VertexShaderFromNir::VertexShaderFromNir;

   bool do_emit_store_deref(const nir_variable *out_var, nir_intrinsic_instr* instr) override;
private:
   void finalize_exports() override;
};

}

#endif 