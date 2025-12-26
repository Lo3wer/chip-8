#include "test_cpu.h"

/* External test declarations */
/* Display ops */
extern void test_opcode_00E0_clears_screen(void);
extern void test_opcode_DXYN_draw_sprite_no_collision(void);
extern void test_opcode_DXYN_draw_sprite_with_collision(void);
extern void test_opcode_DXYN_wraps_around_screen(void);

/* Flow control */
extern void test_opcode_1NNN_jumps(void);
extern void test_opcode_2NNN_call_and_00EE_return(void);
extern void test_opcode_BNNN_jump_with_offset(void);
extern void test_opcode_3XNN_skip_if_equal(void);
extern void test_opcode_3XNN_no_skip_if_not_equal(void);
extern void test_opcode_4XNN_skip_if_not_equal(void);
extern void test_opcode_4XNN_no_skip_if_equal(void);
extern void test_opcode_5XY0_skip_if_registers_equal(void);
extern void test_opcode_9XY0_skip_if_registers_not_equal(void);

/* Register ops */
extern void test_opcode_6XNN_sets_register(void);
extern void test_opcode_7XNN_adds_to_register(void);
extern void test_opcode_7XNN_wraps_around(void);
extern void test_opcode_8XY0_assign(void);
extern void test_opcode_8XY1_or(void);
extern void test_opcode_8XY2_and(void);
extern void test_opcode_8XY3_xor(void);
extern void test_opcode_8XY4_add_with_carry(void);
extern void test_opcode_8XY4_add_no_carry(void);
extern void test_opcode_8XY5_sub_no_borrow(void);
extern void test_opcode_8XY5_sub_with_borrow(void);
extern void test_opcode_8XY7_subn_no_borrow(void);
extern void test_opcode_8XY6_shift_right(void);
extern void test_opcode_8XYE_shift_left(void);

/* Memory ops */
extern void test_opcode_ANNN_set_index(void);
extern void test_opcode_FX1E_add_to_index(void);
extern void test_opcode_FX1E_overflow(void);
extern void test_opcode_FX29_font_character(void);
extern void test_opcode_FX33_bcd(void);
extern void test_opcode_FX33_bcd_edge_cases(void);
extern void test_opcode_FX55_reg_dump(void);
extern void test_opcode_FX65_reg_load(void);

/* Timers */
extern void test_delay_timer_decrements(void);
extern void test_sound_timer_decrements(void);
extern void test_timer_doesnt_go_below_zero(void);
extern void test_opcode_FX07_get_delay_timer(void);
extern void test_opcode_FX15_set_delay_timer(void);
extern void test_opcode_FX18_set_sound_timer(void);

/* Input */
extern void test_opcode_EX9E_skip_if_key_pressed(void);
extern void test_opcode_EX9E_no_skip_if_key_not_pressed(void);
extern void test_opcode_EXA1_skip_if_key_not_pressed(void);
extern void test_opcode_FX0A_wait_for_key(void);

/* Random */
extern void test_opcode_CXNN_random(void);

int main(void)
{
    printf("=== CHIP-8 CPU Test Suite ===\n\n");

    printf("--- Display Operations ---\n");
    RUN_TEST(test_opcode_00E0_clears_screen);
    RUN_TEST(test_opcode_DXYN_draw_sprite_no_collision);
    RUN_TEST(test_opcode_DXYN_draw_sprite_with_collision);
    RUN_TEST(test_opcode_DXYN_wraps_around_screen);

    printf("\n--- Flow Control ---\n");
    RUN_TEST(test_opcode_1NNN_jumps);
    RUN_TEST(test_opcode_2NNN_call_and_00EE_return);
    RUN_TEST(test_opcode_BNNN_jump_with_offset);
    RUN_TEST(test_opcode_3XNN_skip_if_equal);
    RUN_TEST(test_opcode_3XNN_no_skip_if_not_equal);
    RUN_TEST(test_opcode_4XNN_skip_if_not_equal);
    RUN_TEST(test_opcode_4XNN_no_skip_if_equal);
    RUN_TEST(test_opcode_5XY0_skip_if_registers_equal);
    RUN_TEST(test_opcode_9XY0_skip_if_registers_not_equal);

    printf("\n--- Register Operations ---\n");
    RUN_TEST(test_opcode_6XNN_sets_register);
    RUN_TEST(test_opcode_7XNN_adds_to_register);
    RUN_TEST(test_opcode_7XNN_wraps_around);
    RUN_TEST(test_opcode_8XY0_assign);
    RUN_TEST(test_opcode_8XY1_or);
    RUN_TEST(test_opcode_8XY2_and);
    RUN_TEST(test_opcode_8XY3_xor);
    RUN_TEST(test_opcode_8XY4_add_with_carry);
    RUN_TEST(test_opcode_8XY4_add_no_carry);
    RUN_TEST(test_opcode_8XY5_sub_no_borrow);
    RUN_TEST(test_opcode_8XY5_sub_with_borrow);
    RUN_TEST(test_opcode_8XY7_subn_no_borrow);
    RUN_TEST(test_opcode_8XY6_shift_right);
    RUN_TEST(test_opcode_8XYE_shift_left);

    printf("\n--- Memory Operations ---\n");
    RUN_TEST(test_opcode_ANNN_set_index);
    RUN_TEST(test_opcode_FX1E_add_to_index);
    RUN_TEST(test_opcode_FX1E_overflow);
    RUN_TEST(test_opcode_FX29_font_character);
    RUN_TEST(test_opcode_FX33_bcd);
    RUN_TEST(test_opcode_FX33_bcd_edge_cases);
    RUN_TEST(test_opcode_FX55_reg_dump);
    RUN_TEST(test_opcode_FX65_reg_load);

    printf("\n--- Timers ---\n");
    RUN_TEST(test_delay_timer_decrements);
    RUN_TEST(test_sound_timer_decrements);
    RUN_TEST(test_timer_doesnt_go_below_zero);
    RUN_TEST(test_opcode_FX07_get_delay_timer);
    RUN_TEST(test_opcode_FX15_set_delay_timer);
    RUN_TEST(test_opcode_FX18_set_sound_timer);

    printf("\n--- Input Operations ---\n");
    RUN_TEST(test_opcode_EX9E_skip_if_key_pressed);
    RUN_TEST(test_opcode_EX9E_no_skip_if_key_not_pressed);
    RUN_TEST(test_opcode_EXA1_skip_if_key_not_pressed);
    RUN_TEST(test_opcode_FX0A_wait_for_key);

    printf("\n--- Random ---\n");
    RUN_TEST(test_opcode_CXNN_random);

    PRINT_RESULTS();
    return 0;
}