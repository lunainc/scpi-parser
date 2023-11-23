/*-
 * BSD 2-Clause License
 *
 * Copyright (c) 2012-2018, Jan Breuer
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file   scpi_ieee488.c
 * @date   Thu Nov 15 10:58:45 UTC 2012
 * 
 * @brief  Implementation of IEEE488.2 commands and state model
 * 
 * 
 */

#include "scpi/parser.h"
#include "scpi/ieee488.h"
#include "scpi/error.h"
#include "scpi/constants.h"

#include <stdio.h>

uint8_t stb;
uint8_t sre;

scpi_subregister_data_t library_subregister_data[3];

// GROUP, PTR_PRESET, NTR_PRESET, ENAB_PRESET, PARENT_REG, PARENT_BIT
#define LIBRARY_STATUS_REGISTER_GROUPS                    \
X(SCPI_REG_ESR, 0x0000, 0x0000, 0x0000, SCPI_REG_STB, 5)  \
X(SCPI_REG_OPER, 0x7FFF, 0x0000, 0x0000, SCPI_REG_STB, 7) \
X(SCPI_REG_QUES, 0x7FFF, 0x0000, 0x0000, SCPI_REG_STB, 3)

#define X(GROUP, PTR_PRESET, NTR_PRESET, ENAB_PRESET, PARENT_REG, PARENT_BIT)\
{ \
    .data = library_subregister_data + GROUP, \
    .preset = { .ptr = PTR_PRESET, .ntr = NTR_PRESET, .enab = ENAB_PRESET },\
    .parent = { .reg = PARENT_REG, .bit = PARENT_BIT }\
},

const scpi_register_group_t library_register_groups[] = {
    LIBRARY_STATUS_REGISTER_GROUPS
};

static const scpi_register_group_t * get_register_group(scpi_t * context, uint16_t group) {
    uint16_t idx;
    const scpi_register_group_t * register_groups;

    if(group < SCPI_LIB_REG_GROUP_COUNT) {
        register_groups = library_register_groups;
        idx = group;
    } else {
        register_groups = context->user.register_groups.ptr;
        idx = SCPI_LIB_REG_GROUP_COUNT - group;
    }

    return register_groups + idx;
}

static scpi_reg_val_t * get_subregister(scpi_t * context, uint16_t group, uint16_t subreg) {
    const scpi_register_group_t * target_group = get_register_group(context, group);
    switch (subreg) {
        case SCPI_SUBREG_COND:
            return &(target_group->data->cond);
        case SCPI_SUBREG_EVENT:
            return &(target_group->data->event);
        case SCPI_SUBREG_ENAB:
            return &(target_group->data->enab);
        case SCPI_SUBREG_PTR:
            return &(target_group->data->ptr);
        case SCPI_SUBREG_NTR:
            return &(target_group->data->ntr);
        default:
            return NULL;
    }
}

/**
 * Get register value
 * @param name - register name
 * @return register value
 */
scpi_reg_val_t SCPI_RegGet(scpi_t * context, int16_t group, uint16_t subreg) {
    if(group >= 0) {
        scpi_reg_val_t val = *(get_subregister(context, group, subreg));
        if(subreg == SCPI_SUBREG_EVENT) {
            SCPI_RegSet(context, group, subreg, 0);
        }

        return val;
    }

    return 0x8000;
}

/**
 * Wrapper function to control interface from context
 * @param context
 * @param ctrl number of controll message
 * @param value value of related register
 */
static size_t writeControl(scpi_t * context, scpi_ctrl_name_t ctrl, scpi_reg_val_t val) {
    if (context && context->interface && context->interface->control) {
        return context->interface->control(context, ctrl, val);
    } else {
        return 0;
    }
}

static void adjust_stb_bit6(scpi_t * context) {
    uint8_t stb_no_bit6 = stb & ~STB_SRQ;
    uint8_t sre_no_bit6 = sre & ~STB_SRQ;

    if((stb_no_bit6 & sre_no_bit6) > 0) {
        stb = stb | STB_SRQ;
        writeControl(context, SCPI_CTRL_SRQ, stb);
    } else {
        stb = stb & ~STB_SRQ;
    }
}

/**
 * Set register value
 * @param name - register name
 * @param val - new value
 */
void SCPI_RegSet(scpi_t * context, int16_t group, uint16_t subreg, scpi_reg_val_t val) {
    if(group >= 0) {
        // Get a pointer to the specific subregister for the short circuiting checks
        scpi_reg_val_t * subreg_ptr = get_subregister(context, group, subreg);

        // If the value is unchanged, return immediately
        if(*subreg_ptr == val) return;

        // If the register group is the ESR and the subregister is not event or enable, return immediately
        if(group == SCPI_REG_ESR && !(subreg == SCPI_SUBREG_EVENT || subreg == SCPI_SUBREG_ENAB)) return;

        // If the subregister is not a condition register or event register, assign the value and return.
        if(subreg != SCPI_SUBREG_COND && subreg != SCPI_SUBREG_EVENT) {
            *subreg_ptr = val;
            return;
        }

        // If the program execution has gotten past this point, then register change propagation will be performed.

        const scpi_register_group_t * parent_register_group;
        const scpi_register_group_t * working_register_group = get_register_group(context, group);
        scpi_reg_val_t new_cond_reg_val;
        scpi_reg_val_t new_event_reg_val;
        scpi_reg_val_t old_val;
        uint8_t new_summary_val;
        scpi_bool_t skip_to_event_reg_logic;

        // If the initial sub-register is a condition register, do not skip the condition register change logic.
        // Otherwise, if it an event register, then skip the condition register change logic for the first iteration.
        if(subreg == SCPI_SUBREG_COND) {
            skip_to_event_reg_logic = false;
            new_cond_reg_val = val;
        } else {
            skip_to_event_reg_logic = true;
            new_event_reg_val = val;
        }

        // Loop for propagating register changes. One register group is changed per iteration.
        do {
            scpi_subregister_data_t * working_reg_data = working_register_group->data;
            // Condition Register Change Logic
            if(!skip_to_event_reg_logic) {
                // Store the old condition register value
                old_val = working_reg_data->cond;

                // Assign the new condition register value
                working_reg_data->cond = new_cond_reg_val;

                // Calculate the positive and negative bit transitions
                uint16_t transitions = old_val ^ new_cond_reg_val;
                uint16_t ptrans = transitions & new_cond_reg_val;
                uint16_t ntrans = transitions & ~ptrans;

                // Calculate the new event register value
                new_event_reg_val = ((ptrans & working_reg_data->ptr) | (ntrans & working_reg_data->ntr)) | working_reg_data->event;
            }

            // Event Register Logic
            {
                // Store the old event register value
                old_val = working_reg_data->event;

                // If the new event register value is identical to the old event register value, stop the propagation and return
                if(old_val == new_event_reg_val) return;

                // Assign the new event register value
                working_reg_data->event = new_event_reg_val;

                // Calculate the new summary bit value of the parent register
                new_summary_val = (working_reg_data->event & working_reg_data->enab) > 0;
            }

            // Summary Bit Logic
            {
                // If the parent register group is the STB, exit the loop
                if(working_register_group->parent.reg == SCPI_REG_STB) break;

                // Get the parent register group
                parent_register_group = get_register_group(context, working_register_group->parent.reg);

                // Store the old bit summary value
                old_val = (parent_register_group->data->cond & (1 << working_register_group->parent.bit)) > 0;

                // If the new summary value is identical to the old summary value, stop the propagation and return
                if(old_val == new_summary_val) return;

                // Calculate the new value of the condition register value in the parent register group
                if(new_summary_val > 0) {
                    new_cond_reg_val = parent_register_group->data->cond | (1 << working_register_group->parent.bit);
                } else {
                    new_cond_reg_val = parent_register_group->data->cond & ~(1 << working_register_group->parent.bit);
                }

                // Set the parent of the current working register group as the new working register group
                working_register_group = parent_register_group;

                // Ensure that the condition register will not be skipped in the next iteration
                skip_to_event_reg_logic = false;
            }
        } while(true);

        // If the program execution has reached this point then the only register left to update is the STB
        if(new_summary_val > 0) {
            stb = stb | (1 << working_register_group->parent.bit);
        } else {
            stb = stb & ~(1 << working_register_group->parent.bit);
        }

        // Adjust bit 6 of the STB depending on the value of the STB and SRE
        adjust_stb_bit6(context);
    } else if(group == SCPI_REG_SRE) {
        sre = val;
        adjust_stb_bit6(context);
    }
}

/**
 * Set register bits
 * @param name - register name
 * @param bits bit mask
 */
void SCPI_RegSetBits(scpi_t * context, int16_t group, uint16_t subreg, scpi_reg_val_t bits) {
    if(group == SCPI_REG_STB) {
        // Bits 0 and 1 in the STB are the only bits allowed to be set using this function
        // All of the other bits in the STB have pre-defined roles according to the SCPI standard
        //
        // If downstream projects assign bits 0 or 1 to the status of a queue or register,
        // then it is the responsibility of the downstream project developer to ensure that
        // this function is not used to set those downstream assigned bits.
        if(bits & STB_R01) stb |= STB_R01;
        if(bits & STB_PRO) stb |= STB_PRO;
        adjust_stb_bit6(context);
    }else if(group == SCPI_REG_SRE) {
        sre |= bits;
        adjust_stb_bit6(context);
    } else {
        scpi_reg_val_t reg_val = *(get_subregister(context, group, subreg));
        reg_val |= bits;
        SCPI_RegSet(context, group, subreg, reg_val);
    }
//    SCPI_RegSet(context, name, SCPI_RegGet(context, name) | bits);
}

/**
 * Clear register bits
 * @param name - register name
 * @param bits bit mask
 */
void SCPI_RegClearBits(scpi_t * context, int16_t group, uint16_t subreg, scpi_reg_val_t bits) {
    if(group == SCPI_REG_STB) {
        // Bits 0 and 1 in the STB are the only bits allowed to be clear using this function
        // All of the other bits in the STB have pre-defined roles according to the SCPI standard
        //
        // If downstream projects assign bits 0 or 1 to the status of a queue or register,
        // then it is the responsibility of the downstream project developer to ensure that
        // this function is not used to clear those downstream assigned bits.
        if(bits & STB_R01) stb &= ~(STB_R01);
        if(bits & STB_PRO) stb &= ~(STB_PRO);
        adjust_stb_bit6(context);
    }else if(group == SCPI_REG_SRE) {
        sre &= ~bits;
        adjust_stb_bit6(context);
    } else {
        scpi_reg_val_t reg_val = *(get_subregister(context, group, subreg));
        reg_val &= ~bits;
        SCPI_RegSet(context, group, subreg, reg_val);
    }
}

/**
 * *CLS - This command clears all status data structures in a device. 
 *        For a device which minimally complies with SCPI. (SCPI std 4.1.3.2)
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreCls(scpi_t * context) {
    SCPI_ErrorClear(context);
    stb = 0;
    int i;
    for (i = 0; i < SCPI_LIB_REG_GROUP_COUNT + context->user.register_groups.len; ++i) {
        scpi_reg_val_t * event_reg = get_subregister(context, i, SCPI_SUBREG_EVENT);
        *event_reg = 0;
    }
    return SCPI_RES_OK;
}

/**
 * *ESE
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreEse(scpi_t * context) {
    int32_t new_ESE;
    if (SCPI_ParamInt32(context, &new_ESE, TRUE)) {
        SCPI_RegSet(context, SCPI_REG_ESR, SCPI_SUBREG_ENAB, (scpi_reg_val_t) new_ESE);
        return SCPI_RES_OK;
    }
    return SCPI_RES_ERR;
}

/**
 * *ESE?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreEseQ(scpi_t * context) {
    SCPI_ResultInt32(context, SCPI_RegGet(context, SCPI_REG_ESR, SCPI_SUBREG_ENAB));
    return SCPI_RES_OK;
}

/**
 * *ESR?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreEsrQ(scpi_t * context) {
    SCPI_ResultInt32(context, SCPI_RegGet(context, SCPI_REG_ESR, SCPI_SUBREG_EVENT));
    SCPI_RegSet(context, SCPI_REG_ESR, SCPI_SUBREG_EVENT, 0);
    return SCPI_RES_OK;
}

/**
 * *IDN?
 * 
 * field1: MANUFACTURE
 * field2: MODEL
 * field4: SUBSYSTEMS REVISIONS
 * 
 * example: MANUFACTURE,MODEL,0,01-02-01
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreIdnQ(scpi_t * context) {
    int i;
    for (i = 0; i < 4; i++) {
        if (context->idn[i]) {
            SCPI_ResultMnemonic(context, context->idn[i]);
        } else {
            SCPI_ResultMnemonic(context, "0");
        }
    }
    return SCPI_RES_OK;
}

/**
 * *OPC
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreOpc(scpi_t * context) {
    SCPI_RegSetBits(context, SCPI_REG_ESR, SCPI_SUBREG_EVENT, ESR_OPC);
    return SCPI_RES_OK;
}

/**
 * *OPC?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreOpcQ(scpi_t * context) {
    /* Operation is always completed */
    SCPI_ResultInt32(context, 1);
    return SCPI_RES_OK;
}

/**
 * *RST
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreRst(scpi_t * context) {
    if (context && context->interface && context->interface->reset) {
        return context->interface->reset(context);
    }
    return SCPI_RES_OK;
}

/**
 * *SRE
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreSre(scpi_t * context) {
    int32_t new_SRE;
    if (SCPI_ParamInt32(context, &new_SRE, TRUE)) {
        sre = new_SRE;
        adjust_stb_bit6(context);
        return SCPI_RES_OK;
    }
    return SCPI_RES_ERR;
}

/**
 * *SRE?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreSreQ(scpi_t * context) {
    SCPI_ResultInt32(context, sre);
    return SCPI_RES_OK;
}

/**
 * *STB?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreStbQ(scpi_t * context) {
    SCPI_ResultInt32(context, stb);
    return SCPI_RES_OK;
}

/**
 * *TST?
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreTstQ(scpi_t * context) {
    (void) context;
    SCPI_ResultInt32(context, 0);
    return SCPI_RES_OK;
}

/**
 * *WAI
 * @param context
 * @return 
 */
scpi_result_t SCPI_CoreWai(scpi_t * context) {
    (void) context;
    /* NOP */
    return SCPI_RES_OK;
}

