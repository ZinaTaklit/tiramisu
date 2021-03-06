#include <tiramisu/debug.h>
#include <tiramisu/core.h>

#include <string.h>
#include <Halide.h>

#include "baryon_wrapper.h"
#include "benchmarks.h"


using namespace tiramisu;

/*
 * The goal is to generate code that implements the reference.
 * baryon_ref.cpp
 */
void generate_function(std::string name, int size)
{
    tiramisu::global::set_default_tiramisu_options();

    // -------------------------------------------------------
    // Layer I
    // -------------------------------------------------------

    tiramisu::function function0(name);

    tiramisu::constant N_CONST("N", tiramisu::expr((int32_t) size), p_int32, true, NULL, 0, &function0);
    tiramisu::constant d1("d1", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);
    tiramisu::constant d2("d2", tiramisu::expr((int32_t) 1), p_int32, true, NULL, 0, &function0);
    tiramisu::constant d3("d3", tiramisu::expr((int32_t) 2), p_int32, true, NULL, 0, &function0);
    tiramisu::constant t("t", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);
    tiramisu::constant a1("a1", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);
    tiramisu::constant a2("a2", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);
    tiramisu::constant a3("a3", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);
    tiramisu::constant xp0("xp0", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);
    tiramisu::constant KMAX("KMAX", tiramisu::expr((int32_t) size), p_int32, true, NULL, 0, &function0);
    tiramisu::constant b0("b0", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);
    tiramisu::constant b1("b1", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);
    tiramisu::constant b2("b2", tiramisu::expr((int32_t) 0), p_int32, true, NULL, 0, &function0);

    tiramisu::var i3("i3"), i2("i2"), i1("i1"), k("k");
    tiramisu::computation S("{S[p0, i3, i2, i1, p4, p5, p6]}", tiramisu::expr(), false, p_float32, &function0);
    tiramisu::computation wp("{wp[c1, c2, c3, b1, b2, b3, k]}", tiramisu::expr(), false, p_float32, &function0);

    tiramisu::computation Res0("[N]->{Res0[i3, i2, i1]: 0<=i3<N and 0<=i2<N and 0<=i1<N}", tiramisu::expr(), true, p_float32, &function0);
    Res0.set_expression(
		  S(d1, i3, i2, i1, t, a1, xp0) * S(d2, i3, i2, i1, t, a2, xp0) * S(d3, i3, i2, i1, t, a3, xp0)
                + S(d2, i3, i2, i1, t, a1, xp0) * S(d3, i3, i2, i1, t, a2, xp0) * S(d1, i3, i2, i1, t, a3, xp0)
                + S(d3, i3, i2, i1, t, a1, xp0) * S(d1, i3, i2, i1, t, a2, xp0) * S(d2, i3, i2, i1, t, a3, xp0)
                - S(d2, i3, i2, i1, t, a1, xp0) * S(d1, i3, i2, i1, t, a2, xp0) * S(d3, i3, i2, i1, t, a3, xp0)
                - S(d3, i3, i2, i1, t, a1, xp0) * S(d2, i3, i2, i1, t, a2, xp0) * S(d1, i3, i2, i1, t, a3, xp0)
                - S(d1, i3, i2, i1, t, a1, xp0) * S(d3, i3, i2, i1, t, a2, xp0) * S(d2, i3, i2, i1, t, a3, xp0)
	    );
    tiramisu::computation Res1("[N]->{Res1[i3, i2, i1, k]: 0<=i3<N and 0<=i2<N and 0<=i1<N and k=0}", tiramisu::expr((float) 0), true, p_float32, &function0);
    tiramisu::computation Res1_update_0("[N, KMAX]->{Res1_update_0[i3, i2, i1, k]: 0<=i3<N and 0<=i2<N and 0<=i1<N and 1<=k<KMAX}", tiramisu::expr(), true, p_float32, &function0);
    Res1_update_0.set_expression(Res1(i3, i2, i1, k-1) + wp(d1, d2, d3, b0, b1, b2, k) * Res0(i3, i2, i1));

    tiramisu::computation Res2_temp("[N]->{Res2_temp[i3, i2, i1]: 0<=i3<N and 0<=i2<N and 0<=i1<N}", tiramisu::expr((float) 0), true, p_float32, &function0);
    tiramisu::computation Res2_temp_update_0("[N]->{Res2_temp_update_0[i3, i2, i1]: 0<=i3<N and 0<=i2<N and 0<=i1<N}", tiramisu::expr(), true, p_float32, &function0);
    Res2_temp_update_0.set_expression(Res2_temp_update_0(i3, i2, i1) + Res1_update_0(i3, i2, i1, KMAX));

    tiramisu::computation Res2("[N]->{Res2[0]}", tiramisu::expr((float) 0), true, p_float32, &function0);
    tiramisu::computation Res2_update_0("[N]->{Res2_update_0[i3, i2, i1]: 0<=i3<N and 0<=i2<N and 0<=i1<N}", tiramisu::expr(), true, p_float32, &function0);
    Res2_update_0.set_expression(Res2_update_0(i3, i2, i1) + /* exp(i(i3*px+i2*py+i1*pz)) */ Res2_temp_update_0(i3, i2, i1));

    function0.add_context_constraints("[N, M, K]->{:N=16}");

    // -------------------------------------------------------
    // Layer III
    // -------------------------------------------------------

    tiramisu::buffer buf_res0("buf_res0", {N_CONST}, tiramisu::p_float32, a_temporary, &function0);
    buf_res0.set_auto_allocate(false);
    tiramisu::computation *alloc_res0 = buf_res0.allocate_at(Res0, i1);
    tiramisu::buffer buf_res1("buf_res1", {N_CONST}, tiramisu::p_float32, a_temporary, &function0);
    buf_res1.set_auto_allocate(false);
    tiramisu::computation *alloc_res1 = buf_res1.allocate_at(Res0, i1);
    tiramisu::buffer buf_res2_temp("buf_res2_temp", {N_CONST}, tiramisu::p_float32, a_temporary, &function0);
    tiramisu::buffer buf_res2("buf_res2", {tiramisu::expr((int32_t) 1)}, tiramisu::p_float32, a_output, &function0);

    // S(c1, i3, i2, i1, t, a1, x’0)
    tiramisu::buffer buf_S("buf_S", {tiramisu::expr((int32_t) BARYON_P1), N_CONST, N_CONST, N_CONST, tiramisu::expr((int32_t) BARYON_P), tiramisu::expr((int32_t) BARYON_P), tiramisu::expr((int32_t) BARYON_P)}, tiramisu::p_float32, a_input, &function0);
    // wp(c1, c2, c3, b0, b1, b2, k)
    tiramisu::buffer buf_wp("buf_wp", {tiramisu::expr((int32_t) BARYON_P1), tiramisu::expr((int32_t) BARYON_P1), tiramisu::expr((int32_t) BARYON_P1), tiramisu::expr((int32_t) BARYON_P), tiramisu::expr((int32_t) BARYON_P), tiramisu::expr((int32_t) BARYON_P), KMAX}, tiramisu::p_float32, a_input, &function0);

    Res0.set_access("{Res0[i3,i2,i1]->buf_res0[i1]}");
    Res1.set_access("{Res1[i3,i2,i1,k]->buf_res1[i1]}");
    Res1_update_0.set_access("{Res1_update_0[i3,i2,i1,k]->buf_res1[i1]}");
    Res2_temp.set_access("{Res2_temp[i3,i2,i1]->buf_res2_temp[i1]}");
    Res2_temp_update_0.set_access("{Res2_temp_update_0[i3,i2,i1]->buf_res2_temp[i1]}");
    Res2.set_access("{Res2[0]->buf_res2[0]}");
    Res2_update_0.set_access("{Res2_update_0[i3,i2,i1]->buf_res2[0]}");
    S.set_access("{S[d1,i3,i2,i1,t,a1,xp0]->buf_S[d1,i3,i2,i1,t,a1,xp0]}");
    wp.set_access("{wp[d1,d2,d3,b0,b1,b2,k]->buf_wp[d1,d2,d3,b0,b1,b2,k]}");

    // -------------------------------------------------------
    // Layer II
    // -------------------------------------------------------

    alloc_res0->after(Res2, tiramisu::computation::root);
    alloc_res1->after(*alloc_res0, i1);
    Res2_temp.after(*alloc_res1, i1);
    Res0.after(Res2_temp, i1);
    Res1.after(Res0, i1);
    Res1_update_0.after(Res1, i1);
    Res2_temp_update_0.after(Res1_update_0, i1);
    Res2_update_0.after(Res2_temp_update_0, i2);

    Res0.tag_vector_level(i1, BARYON_N);

    // -------------------------------------------------------
    // Code Generation
    // -------------------------------------------------------

    function0.set_arguments({&buf_res2, &buf_S, &buf_wp});
    function0.gen_time_space_domain();
    function0.gen_isl_ast();
    function0.gen_halide_stmt();
    function0.gen_halide_obj("generated_" + std::string(TEST_NAME_STR) + ".o");
}

int main(int argc, char **argv)
{
    generate_function("tiramisu_generated_code", BARYON_N);

    return 0;
}
