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
 * @file   scpi_error.h
 * @date   Thu Nov 15 10:58:45 UTC 2012
 *
 * @brief  Error handling and storing routines
 *
 *
 */

#ifndef SCPI_ERROR_H
#define SCPI_ERROR_H

#include "scpi/config.h"
#include "scpi/types.h"

#ifdef __cplusplus
extern "C" {
#endif

    void SCPI_ErrorInit(scpi_t * context, scpi_error_t * data, int16_t size);
    void SCPI_ErrorClear(scpi_t * context);
    scpi_bool_t SCPI_ErrorPop(scpi_t * context, scpi_error_t * error);
    void SCPI_ErrorPushEx(scpi_t * context, int16_t err, char * info, size_t info_len);
    void SCPI_ErrorPush(scpi_t * context, int16_t err);
    int32_t SCPI_ErrorCount(scpi_t * context);
    const char * SCPI_ErrorTranslate(scpi_t * context, int16_t err);


    /* Using X-Macro technique to define everything once
     * http://en.wikipedia.org/wiki/X_Macro
     *
     * The MINIMAL_LIST_OF_ERRORS macro defines the minimal set of errors used by the library itself
     */
#define MINIMAL_LIST_OF_ERRORS \
    X(SCPI_ERROR_NO_ERROR,                         0, "No error")                                     \
    X(SCPI_ERROR_INVALID_CHARACTER,             -101, "Invalid character")                            \
    X(SCPI_ERROR_INVALID_SEPARATOR,             -103, "Invalid separator")                            \
    X(SCPI_ERROR_DATA_TYPE_ERROR,               -104, "Data type error")                              \
    X(SCPI_ERROR_PARAMETER_NOT_ALLOWED,         -108, "Parameter not allowed")                        \
    X(SCPI_ERROR_MISSING_PARAMETER,             -109, "Missing parameter")                            \
    X(SCPI_ERROR_UNDEFINED_HEADER,              -113, "Undefined header")                             \
    X(SCPI_ERROR_INVALID_SUFFIX,                -131, "Invalid suffix")                               \
    X(SCPI_ERROR_SUFFIX_NOT_ALLOWED,            -138, "Suffix not allowed")                           \
    X(SCPI_ERROR_INVALID_STRING_DATA,           -151, "Invalid string data")                          \
    X(SCPI_ERROR_EXPRESSION_PARSING_ERROR,      -170, "Expression error")                             \
    X(SCPI_ERROR_EXECUTION_ERROR,               -200, "Execution error")                              \
    X(SCPI_ERROR_ILLEGAL_PARAMETER_VALUE,       -224, "Illegal parameter value")                      \
    X(SCPI_ERROR_SYSTEM_ERROR,                  -310, "System error")                                 \
    X(SCPI_ERROR_QUEUE_OVERFLOW,                -350, "Queue overflow")                               \
    X(SCPI_ERROR_INPUT_BUFFER_OVERRUN,          -363, "Input buffer overrun")                         \


#define DEF_SCPI_USER_ERR(NAME, CODE, MSG) { .code=CODE, .msg=MSG },

    // The DEFINE_SCPI_ERROR_* macros are for selectively composing an application-specific list of errors
#define DEFINE_SCPI_ERROR_COMMAND                      DEF_SCPI_USER_ERR(SCPI_ERROR_COMMAND,                      -100, "Command error")
#define DEFINE_SCPI_ERROR_SYNTAX                       DEF_SCPI_USER_ERR(SCPI_ERROR_SYNTAX,                       -102, "Syntax error")
#define DEFINE_SCPI_ERROR_GET_NOT_ALLOWED              DEF_SCPI_USER_ERR(SCPI_ERROR_GET_NOT_ALLOWED,              -105, "GET not allowed")
#define DEFINE_SCPI_ERROR_COMMAND_HEADER               DEF_SCPI_USER_ERR(SCPI_ERROR_COMMAND_HEADER,               -110, "Command header error")
#define DEFINE_SCPI_ERROR_HEADER_SEPARATOR             DEF_SCPI_USER_ERR(SCPI_ERROR_HEADER_SEPARATOR,             -111, "Header separator error")
#define DEFINE_SCPI_ERROR_PRG_MNEMONIC_TOO_LONG        DEF_SCPI_USER_ERR(SCPI_ERROR_PRG_MNEMONIC_TOO_LONG,        -112, "Program mnemonic too long")
#define DEFINE_SCPI_ERROR_HEADER_SUFFIX_OUTOFRANGE     DEF_SCPI_USER_ERR(SCPI_ERROR_HEADER_SUFFIX_OUTOFRANGE,     -114, "Header suffix out of range")
#define DEFINE_SCPI_ERROR_UNEXP_NUM_OF_PARAMETER       DEF_SCPI_USER_ERR(SCPI_ERROR_UNEXP_NUM_OF_PARAMETER,       -115, "Unexpected number of parameters")
#define DEFINE_SCPI_ERROR_NUMERIC_DATA_ERROR           DEF_SCPI_USER_ERR(SCPI_ERROR_NUMERIC_DATA_ERROR,           -120, "Numeric data error")
#define DEFINE_SCPI_ERROR_INVAL_CHAR_IN_NUMBER         DEF_SCPI_USER_ERR(SCPI_ERROR_INVAL_CHAR_IN_NUMBER,         -121, "Invalid character in number")
#define DEFINE_SCPI_ERROR_EXPONENT_TOO_LONG            DEF_SCPI_USER_ERR(SCPI_ERROR_EXPONENT_TOO_LONG,            -123, "Exponent too large")
#define DEFINE_SCPI_ERROR_TOO_MANY_DIGITS              DEF_SCPI_USER_ERR(SCPI_ERROR_TOO_MANY_DIGITS,              -124, "Too many digits")
#define DEFINE_SCPI_ERROR_NUMERIC_DATA_NOT_ALLOWED     DEF_SCPI_USER_ERR(SCPI_ERROR_NUMERIC_DATA_NOT_ALLOWED,     -128, "Numeric data not allowed")
#define DEFINE_SCPI_ERROR_SUFFIX_ERROR                 DEF_SCPI_USER_ERR(SCPI_ERROR_SUFFIX_ERROR,                 -130, "Suffix error")
#define DEFINE_SCPI_ERROR_SUFFIX_TOO_LONG              DEF_SCPI_USER_ERR(SCPI_ERROR_SUFFIX_TOO_LONG,              -134, "Suffix too long")
#define DEFINE_SCPI_ERROR_CHARACTER_DATA_ERROR         DEF_SCPI_USER_ERR(SCPI_ERROR_CHARACTER_DATA_ERROR,         -140, "Character data error")
#define DEFINE_SCPI_ERROR_INVAL_CHARACTER_DATA         DEF_SCPI_USER_ERR(SCPI_ERROR_INVAL_CHARACTER_DATA,         -141, "Invalid character data")
#define DEFINE_SCPI_ERROR_CHARACTER_DATA_TOO_LONG      DEF_SCPI_USER_ERR(SCPI_ERROR_CHARACTER_DATA_TOO_LONG,      -144, "Character data too long")
#define DEFINE_SCPI_ERROR_CHARACTER_DATA_NOT_ALLOWED   DEF_SCPI_USER_ERR(SCPI_ERROR_CHARACTER_DATA_NOT_ALLOWED,   -148, "Character data not allowed")
#define DEFINE_SCPI_ERROR_STRING_DATA_ERROR            DEF_SCPI_USER_ERR(SCPI_ERROR_STRING_DATA_ERROR,            -150, "String data error")
#define DEFINE_SCPI_ERROR_STRING_DATA_NOT_ALLOWED      DEF_SCPI_USER_ERR(SCPI_ERROR_STRING_DATA_NOT_ALLOWED,      -158, "String data not allowed")
#define DEFINE_SCPI_ERROR_BLOCK_DATA_ERROR             DEF_SCPI_USER_ERR(SCPI_ERROR_BLOCK_DATA_ERROR,             -160, "Block data error")
#define DEFINE_SCPI_ERROR_INVALID_BLOCK_DATA           DEF_SCPI_USER_ERR(SCPI_ERROR_INVALID_BLOCK_DATA,           -161, "Invalid block data")
#define DEFINE_SCPI_ERROR_BLOCK_DATA_NOT_ALLOWED       DEF_SCPI_USER_ERR(SCPI_ERROR_BLOCK_DATA_NOT_ALLOWED,       -168, "Block data not allowed")
#define DEFINE_SCPI_ERROR_INVAL_EXPRESSION             DEF_SCPI_USER_ERR(SCPI_ERROR_INVAL_EXPRESSION,             -171, "Invalid expression")
#define DEFINE_SCPI_ERROR_EXPRESSION_DATA_NOT_ALLOWED  DEF_SCPI_USER_ERR(SCPI_ERROR_EXPRESSION_DATA_NOT_ALLOWED,  -178, "Expression data not allowed")
#define DEFINE_SCPI_ERROR_MACRO_DEFINITION_ERROR       DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_DEFINITION_ERROR,       -180, "Macro error")
#define DEFINE_SCPI_ERROR_INVAL_OUTSIDE_MACRO_DEF      DEF_SCPI_USER_ERR(SCPI_ERROR_INVAL_OUTSIDE_MACRO_DEF,      -181, "Invalid outside macro DEFINEinition")
#define DEFINE_SCPI_ERROR_INVAL_INSIDE_MACRO_DEF       DEF_SCPI_USER_ERR(SCPI_ERROR_INVAL_INSIDE_MACRO_DEF,       -183, "Invalid inside macro DEFINEinition")
#define DEFINE_SCPI_ERROR_MACRO_PARAMETER_ERROR        DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_PARAMETER_ERROR,        -184, "Macro parameter error")
#define DEFINE_SCPI_ERROR_INVAL_WHILE_IN_LOCAL         DEF_SCPI_USER_ERR(SCPI_ERROR_INVAL_WHILE_IN_LOCAL,         -201, "Invalid while in local")
#define DEFINE_SCPI_ERROR_SETTINGS_LOST_DUE_TO_RTL     DEF_SCPI_USER_ERR(SCPI_ERROR_SETTINGS_LOST_DUE_TO_RTL,     -202, "Settings lost due to rtl")
#define DEFINE_SCPI_ERROR_COMMAND_PROTECTED            DEF_SCPI_USER_ERR(SCPI_ERROR_COMMAND_PROTECTED,            -203, "Command protected")
#define DEFINE_SCPI_ERROR_TRIGGER_ERROR                DEF_SCPI_USER_ERR(SCPI_ERROR_TRIGGER_ERROR,                -210, "Trigger error")
#define DEFINE_SCPI_ERROR_TRIGGER_IGNORED              DEF_SCPI_USER_ERR(SCPI_ERROR_TRIGGER_IGNORED,              -211, "Trigger ignored")
#define DEFINE_SCPI_ERROR_ARM_IGNORED                  DEF_SCPI_USER_ERR(SCPI_ERROR_ARM_IGNORED,                  -212, "Arm ignored")
#define DEFINE_SCPI_ERROR_INIT_IGNORED                 DEF_SCPI_USER_ERR(SCPI_ERROR_INIT_IGNORED,                 -213, "Init ignored")
#define DEFINE_SCPI_ERROR_TRIGGER_DEADLOCK             DEF_SCPI_USER_ERR(SCPI_ERROR_TRIGGER_DEADLOCK,             -214, "Trigger deadlock")
#define DEFINE_SCPI_ERROR_ARM_DEADLOCK                 DEF_SCPI_USER_ERR(SCPI_ERROR_ARM_DEADLOCK,                 -215, "Arm deadlock")
#define DEFINE_SCPI_ERROR_PARAMETER_ERROR              DEF_SCPI_USER_ERR(SCPI_ERROR_PARAMETER_ERROR,              -220, "Parameter error")
#define DEFINE_SCPI_ERROR_SETTINGS_CONFLICT            DEF_SCPI_USER_ERR(SCPI_ERROR_SETTINGS_CONFLICT,            -221, "Settings conflict")
#define DEFINE_SCPI_ERROR_DATA_OUT_OF_RANGE            DEF_SCPI_USER_ERR(SCPI_ERROR_DATA_OUT_OF_RANGE,            -222, "Data out of range")
#define DEFINE_SCPI_ERROR_TOO_MUCH_DATA                DEF_SCPI_USER_ERR(SCPI_ERROR_TOO_MUCH_DATA,                -223, "Too much data")
#define DEFINE_SCPI_ERROR_OUT_OF_MEMORY_FOR_REQ_OP     DEF_SCPI_USER_ERR(SCPI_ERROR_OUT_OF_MEMORY_FOR_REQ_OP,     -225, "Out of memory")
#define DEFINE_SCPI_ERROR_LISTS_NOT_SAME_LENGTH        DEF_SCPI_USER_ERR(SCPI_ERROR_LISTS_NOT_SAME_LENGTH,        -226, "Lists not same length")
#define DEFINE_SCPI_ERROR_DATA_CORRUPT                 DEF_SCPI_USER_ERR(SCPI_ERROR_DATA_CORRUPT,                 -230, "Data corrupt or stale")
#define DEFINE_SCPI_ERROR_DATA_QUESTIONABLE            DEF_SCPI_USER_ERR(SCPI_ERROR_DATA_QUESTIONABLE,            -231, "Data questionable")
#define DEFINE_SCPI_ERROR_INVAL_VERSION                DEF_SCPI_USER_ERR(SCPI_ERROR_INVAL_VERSION,                -233, "Invalid version")
#define DEFINE_SCPI_ERROR_HARDWARE_ERROR               DEF_SCPI_USER_ERR(SCPI_ERROR_HARDWARE_ERROR,               -240, "Hardware error")
#define DEFINE_SCPI_ERROR_HARDWARE_MISSING             DEF_SCPI_USER_ERR(SCPI_ERROR_HARDWARE_MISSING,             -241, "Hardware missing")
#define DEFINE_SCPI_ERROR_MASS_STORAGE_ERROR           DEF_SCPI_USER_ERR(SCPI_ERROR_MASS_STORAGE_ERROR,           -250, "Mass storage error")
#define DEFINE_SCPI_ERROR_MISSING_MASS_STORAGE         DEF_SCPI_USER_ERR(SCPI_ERROR_MISSING_MASS_STORAGE,         -251, "Missing mass storage")
#define DEFINE_SCPI_ERROR_MISSING_MASS_MEDIA           DEF_SCPI_USER_ERR(SCPI_ERROR_MISSING_MASS_MEDIA,           -252, "Missing media")
#define DEFINE_SCPI_ERROR_CORRUPT_MEDIA                DEF_SCPI_USER_ERR(SCPI_ERROR_CORRUPT_MEDIA,                -253, "Corrupt media")
#define DEFINE_SCPI_ERROR_MEDIA_FULL                   DEF_SCPI_USER_ERR(SCPI_ERROR_MEDIA_FULL,                   -254, "Media full")
#define DEFINE_SCPI_ERROR_DIRECTORY_FULL               DEF_SCPI_USER_ERR(SCPI_ERROR_DIRECTORY_FULL,               -255, "Directory full")
#define DEFINE_SCPI_ERROR_FILE_NAME_NOT_FOUND          DEF_SCPI_USER_ERR(SCPI_ERROR_FILE_NAME_NOT_FOUND,          -256, "File name not found")
#define DEFINE_SCPI_ERROR_FILE_NAME_ERROR              DEF_SCPI_USER_ERR(SCPI_ERROR_FILE_NAME_ERROR,              -257, "File name error")
#define DEFINE_SCPI_ERROR_MEDIA_PROTECTED              DEF_SCPI_USER_ERR(SCPI_ERROR_MEDIA_PROTECTED,              -258, "Media protected")
#define DEFINE_SCPI_ERROR_EXPRESSION_EXECUTING_ERROR   DEF_SCPI_USER_ERR(SCPI_ERROR_EXPRESSION_EXECUTING_ERROR,   -260, "Expression error")
#define DEFINE_SCPI_ERROR_MATH_ERROR_IN_EXPRESSION     DEF_SCPI_USER_ERR(SCPI_ERROR_MATH_ERROR_IN_EXPRESSION,     -261, "Math error in expression")
#define DEFINE_SCPI_ERROR_MACRO_UNDEF_EXEC_ERROR       DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_UNDEF_EXEC_ERROR,       -270, "Macro error")
#define DEFINE_SCPI_ERROR_MACRO_SYNTAX_ERROR           DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_SYNTAX_ERROR,           -271, "Macro syntax error")
#define DEFINE_SCPI_ERROR_MACRO_EXECUTION_ERROR        DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_EXECUTION_ERROR,        -272, "Macro execution error")
#define DEFINE_SCPI_ERROR_ILLEGAL_MACRO_LABEL          DEF_SCPI_USER_ERR(SCPI_ERROR_ILLEGAL_MACRO_LABEL,          -273, "Illegal macro label")
#define DEFINE_SCPI_ERROR_IMPROPER_USED_MACRO_PARAM    DEF_SCPI_USER_ERR(SCPI_ERROR_IMPROPER_USED_MACRO_PARAM,    -274, "Macro parameter error")
#define DEFINE_SCPI_ERROR_MACRO_DEFINITION_TOO_LONG    DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_DEFINITION_TOO_LONG,    -275, "Macro DEFINEinition too long")
#define DEFINE_SCPI_ERROR_MACRO_RECURSION_ERROR        DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_RECURSION_ERROR,        -276, "Macro recursion error")
#define DEFINE_SCPI_ERROR_MACRO_REDEF_NOT_ALLOWED      DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_REDEF_NOT_ALLOWED,      -277, "Macro redefinition not allowed")
#define DEFINE_SCPI_ERROR_MACRO_HEADER_NOT_FOUND       DEF_SCPI_USER_ERR(SCPI_ERROR_MACRO_HEADER_NOT_FOUND,       -278, "Macro header not found")
#define DEFINE_SCPI_ERROR_PROGRAM_ERROR                DEF_SCPI_USER_ERR(SCPI_ERROR_PROGRAM_ERROR,                -280, "Program error")
#define DEFINE_SCPI_ERROR_CANNOT_CREATE_PROGRAM        DEF_SCPI_USER_ERR(SCPI_ERROR_CANNOT_CREATE_PROGRAM,        -281, "Cannot create program")
#define DEFINE_SCPI_ERROR_ILLEGAL_PROGRAM_NAME         DEF_SCPI_USER_ERR(SCPI_ERROR_ILLEGAL_PROGRAM_NAME,         -282, "Illegal program name")
#define DEFINE_SCPI_ERROR_ILLEGAL_VARIABLE_NAME        DEF_SCPI_USER_ERR(SCPI_ERROR_ILLEGAL_VARIABLE_NAME,        -283, "Illegal variable name")
#define DEFINE_SCPI_ERROR_PROGRAM_CURRENTLY_RUNNING    DEF_SCPI_USER_ERR(SCPI_ERROR_PROGRAM_CURRENTLY_RUNNING,    -284, "Program currently running")
#define DEFINE_SCPI_ERROR_PROGRAM_SYNTAX_ERROR         DEF_SCPI_USER_ERR(SCPI_ERROR_PROGRAM_SYNTAX_ERROR,         -285, "Program syntax error")
#define DEFINE_SCPI_ERROR_PROGRAM_RUNTIME_ERROR        DEF_SCPI_USER_ERR(SCPI_ERROR_PROGRAM_RUNTIME_ERROR,        -286, "Program runtime error")
#define DEFINE_SCPI_ERROR_MEMORY_USE_ERROR             DEF_SCPI_USER_ERR(SCPI_ERROR_MEMORY_USE_ERROR,             -290, "Memory use error")
#define DEFINE_SCPI_ERROR_OUT_OF_MEMORY                DEF_SCPI_USER_ERR(SCPI_ERROR_OUT_OF_MEMORY,                -291, "Out of memory")
#define DEFINE_SCPI_ERROR_REF_NAME_DOES_NOT_EXIST      DEF_SCPI_USER_ERR(SCPI_ERROR_REF_NAME_DOES_NOT_EXIST,      -292, "Referenced name does not exist")
#define DEFINE_SCPI_ERROR_REF_NAME_ALREADY_EXISTS      DEF_SCPI_USER_ERR(SCPI_ERROR_REF_NAME_ALREADY_EXISTS,      -293, "Referenced name already exists")
#define DEFINE_SCPI_ERROR_INCOMPATIBLE_TYPE            DEF_SCPI_USER_ERR(SCPI_ERROR_INCOMPATIBLE_TYPE,            -294, "Incompatible type")
#define DEFINE_SCPI_ERROR_DEVICE_ERROR                 DEF_SCPI_USER_ERR(SCPI_ERROR_DEVICE_ERROR,                 -300, "Device specific error")
#define DEFINE_SCPI_ERROR_MEMORY_ERROR                 DEF_SCPI_USER_ERR(SCPI_ERROR_MEMORY_ERROR,                 -311, "Memory error")
#define DEFINE_SCPI_ERROR_PUD_MEMORY_LOST              DEF_SCPI_USER_ERR(SCPI_ERROR_PUD_MEMORY_LOST,              -312, "PUD memory lost")
#define DEFINE_SCPI_ERROR_CALIBRATION_MEMORY_LOST      DEF_SCPI_USER_ERR(SCPI_ERROR_CALIBRATION_MEMORY_LOST,      -313, "Calibration memory lost")
#define DEFINE_SCPI_ERROR_SAVE_RECALL_MEMORY_LOST      DEF_SCPI_USER_ERR(SCPI_ERROR_SAVE_RECALL_MEMORY_LOST,      -314, "Save/recall memory lost")
#define DEFINE_SCPI_ERROR_CONFIGURATION_MEMORY_LOST    DEF_SCPI_USER_ERR(SCPI_ERROR_CONFIGURATION_MEMORY_LOST,    -315, "Configuration memory lost")
#define DEFINE_SCPI_ERROR_STORAGE_FAULT                DEF_SCPI_USER_ERR(SCPI_ERROR_STORAGE_FAULT,                -320, "Storage fault")
#define DEFINE_SCPI_ERROR_OUT_OF_DEVICE_MEMORY         DEF_SCPI_USER_ERR(SCPI_ERROR_OUT_OF_DEVICE_MEMORY,         -321, "Out of memory")
#define DEFINE_SCPI_ERROR_SELF_TEST_FAILED             DEF_SCPI_USER_ERR(SCPI_ERROR_SELF_TEST_FAILED,             -330, "Self-test failed")
#define DEFINE_SCPI_ERROR_CALIBRATION_FAILED           DEF_SCPI_USER_ERR(SCPI_ERROR_CALIBRATION_FAILED,           -340, "Calibration failed")
#define DEFINE_SCPI_ERROR_COMMUNICATION_ERROR          DEF_SCPI_USER_ERR(SCPI_ERROR_COMMUNICATION_ERROR,          -360, "Communication error")
#define DEFINE_SCPI_ERROR_PARITY_ERROR_IN_CMD_MSG      DEF_SCPI_USER_ERR(SCPI_ERROR_PARITY_ERROR_IN_CMD_MSG,      -361, "Parity error in program message")
#define DEFINE_SCPI_ERROR_FRAMING_ERROR_IN_CMD_MSG     DEF_SCPI_USER_ERR(SCPI_ERROR_FRAMING_ERROR_IN_CMD_MSG,     -362, "Framing error in program message")
#define DEFINE_SCPI_ERROR_TIME_OUT                     DEF_SCPI_USER_ERR(SCPI_ERROR_TIME_OUT,                     -365, "Time out error")
#define DEFINE_SCPI_ERROR_QUERY_ERROR                  DEF_SCPI_USER_ERR(SCPI_ERROR_QUERY_ERROR,                  -400, "Query error")
#define DEFINE_SCPI_ERROR_QUERY_INTERRUPTED            DEF_SCPI_USER_ERR(SCPI_ERROR_QUERY_INTERRUPTED,            -410, "Query INTERRUPTED")
#define DEFINE_SCPI_ERROR_QUERY_UNTERMINATED           DEF_SCPI_USER_ERR(SCPI_ERROR_QUERY_UNTERMINATED,           -420, "Query UNTERMINATED")
#define DEFINE_SCPI_ERROR_QUERY_DEADLOCKED             DEF_SCPI_USER_ERR(SCPI_ERROR_QUERY_DEADLOCKED,             -430, "Query DEADLOCKED")
#define DEFINE_SCPI_ERROR_QUERY_UNTERM_INDEF_RESP      DEF_SCPI_USER_ERR(SCPI_ERROR_QUERY_UNTERM_INDEF_RESP,      -440, "Query UNTERMINATED after indefinite response")
#define DEFINE_SCPI_ERROR_POWER_ON                     DEF_SCPI_USER_ERR(SCPI_ERROR_POWER_ON,                     -500, "Power on")
#define DEFINE_SCPI_ERROR_USER_REQUEST                 DEF_SCPI_USER_ERR(SCPI_ERROR_USER_REQUEST,                 -600, "User request")
#define DEFINE_SCPI_ERROR_REQUEST_CONTROL              DEF_SCPI_USER_ERR(SCPI_ERROR_REQUEST_CONTROL,              -700, "Request control")
#define DEFINE_SCPI_ERROR_OPERATION_COMPLETE           DEF_SCPI_USER_ERR(SCPI_ERROR_OPERATION_COMPLETE,           -800, "Operation complete")

    enum {
#define X(def, val, str) def = val,
        MINIMAL_LIST_OF_ERRORS
#undef X
    };

#ifdef __cplusplus
}
#endif

#endif /* SCPI_ERROR_H */

