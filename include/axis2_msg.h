/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AXIS2_MSG_H
#define AXIS2_MSG_H

/** 
 * @defgroup axis2_msg message
 * @ingroup axis2_desc
 * message represents a message in a WSDL. It captures SOAP headers related to 
 * a given message, the direction as well as the phases to be invoked along
 * the flow. Based on the message direction, there could be only one flow 
 * associated with a message.
 * @{
 */

/**
  * @file axis2_msg.h
  */

#include <axis2_param_container.h>
#include <axis2_op.h>
#include <axis2_array_list.h>
#include <axis2_description.h>
#include <axis2_phase_meta.h>

#define AXIS2_MSG_IN            "InMessage"
#define AXIS2_MSG_OUT           "OutMessage"
#define AXIS2_MSG_IN_FAULT      "InFaultMessage"
#define AXIS2_MSG_OUT_FAULT     "OutFaultMessage"


#ifdef __cplusplus
extern "C"
{
#endif

    /** Type name for struct axis2_msg */ 
    typedef struct axis2_msg axis2_msg_t;

    /** 
     * Frees message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
     */
    axis2_status_t AXIS2_CALL
    axis2_msg_free(axis2_msg_t *msg,
        const axis2_env_t *env);

    /** 
     * Adds a parameter.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @param param pointer to parameter, message assumes ownership of 
     * parameter
     * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
     */
    axis2_status_t AXIS2_CALL
    axis2_msg_add_param(axis2_msg_t *msg,
        const axis2_env_t *env,
        axis2_param_t *param);

    /**
     * Gets the named parameter.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @param name parameter name string
     * @return pointer to parameter corresponding to the same name, returns
     * a reference, not a cloned copy
     */
    axis2_param_t *AXIS2_CALL
    axis2_msg_get_param(const axis2_msg_t *msg,
        const axis2_env_t *env,
        const axis2_char_t *name);

    /** 
     * Gets all parameters stored in message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @return pointer to list of parameters, returns a reference, not a 
     * cloned copy
     */
    axis2_array_list_t *AXIS2_CALL
    axis2_msg_get_all_params(const axis2_msg_t *msg,
        const axis2_env_t *env);

    /** 
     * Checks if the named parameter is locked.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @param param_name parameter name string
     * @return AXIS2_TRUE if the parameter is locked, else AXIS2_FALSE         
     */
    axis2_bool_t AXIS2_CALL
    axis2_msg_is_param_locked(axis2_msg_t *msg,
        const axis2_env_t *env,
        const axis2_char_t *param_name);

    /** 
     * Sets parent. Parent of a message is of type operation.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @param op pointer to parent operation, message does not assume 
     * ownership of parent
     * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
     */
    axis2_status_t AXIS2_CALL
    axis2_msg_set_parent(axis2_msg_t *msg,
        const axis2_env_t *env,
        axis2_op_t *op);

    /** 
     * Gets parent. Parent of a message is of type operation.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @return pointer to parent operation, returns a reference, not a 
     * cloned copy
     */
    axis2_op_t *AXIS2_CALL
    axis2_msg_get_parent(const axis2_msg_t *msg,
        const axis2_env_t *env);

    /**
     * Gets flow of execution associated with the message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @return pointer to array list containing the list of phases 
     * representing the flow
     */
    axis2_array_list_t *AXIS2_CALL
    axis2_msg_get_flow( const axis2_msg_t *msg,
        const axis2_env_t *env);

    /** 
     * Sets flow of execution associated with the message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @param flow pointer to array list of phases representing the flow,
     * message assumes ownership of flow
     * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
     */
    axis2_status_t AXIS2_CALL
    axis2_msg_set_flow(axis2_msg_t *msg,
        const axis2_env_t *env,
        axis2_array_list_t *flow);

    /** 
     * Gets direction of message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @return direction string
     */
    const axis2_char_t *AXIS2_CALL
    axis2_msg_get_direction(const axis2_msg_t *msg,
        const axis2_env_t *env);

    /** 
     * Sets direction of message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @param direction pointer to direction
     * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
     */
    axis2_status_t AXIS2_CALL
    axis2_msg_set_direction(axis2_msg_t *msg,
        const axis2_env_t *env,
        const axis2_char_t *direction);

    /** 
     * Gets QName representing message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @return pointer to QName, returns a reference, not a cloned copy
     */
    const axis2_qname_t *AXIS2_CALL
    axis2_msg_get_element_qname(const axis2_msg_t *msg,
        const axis2_env_t *env);

    /** 
     * Sets QName representing message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @param element_qname pointer to QName representing message, this 
     * function creates a clone of QName
     * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
     */
    axis2_status_t AXIS2_CALL
    axis2_msg_set_element_qname(axis2_msg_t *msg,
        const axis2_env_t *env,
        const axis2_qname_t *element_qname);

    /** 
     * Gets message name.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @return message name string.
     */
    const axis2_char_t *AXIS2_CALL
    axis2_msg_get_name(const axis2_msg_t *msg,
        const axis2_env_t *env);

    /** 
     * Sets message name.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @param message name string
     * @return AXIS2_SUCCESS on success, else AXIS2_FAILURE
     */
    axis2_status_t AXIS2_CALL
    axis2_msg_set_name(axis2_msg_t *msg,
        const axis2_env_t *env,
        const axis2_char_t *name);

    /** 
     * Gets XML schema element associated with the message.
     * @param msg pointer to message
     * @param env pointer to environment struct
     * @return pointer to XML schema element, returns a reference, not 
     * a cloned copy
     */
    /*xml_schema_element_t *AXIS2_CALL
    axis2_msg_get_schema_element(const axis2_msg_t *msg,
        const axis2_env_t *env);*/

    /**
     * Creates message struct instance.
     * @param env pointer to environment struct
     * @return pointer to newly created message
     */
    AXIS2_EXTERN axis2_msg_t *AXIS2_CALL
    axis2_msg_create(const axis2_env_t *env);


/** Frees message. */
#define AXIS2_MSG_FREE(msg, env) \
        axis2_msg_free (msg, env)

/** Adds given parameter. */
#define AXIS2_MSG_ADD_PARAM(msg, env, param) \
      axis2_msg_add_param (msg, env, param)

/** Gets named parameter. */
#define AXIS2_MSG_GET_PARAM(msg, env, key) \
      axis2_msg_get_param (msg, env, key)

/** Gets all parameters. */
#define AXIS2_MSG_GET_ALL_PARAMS(msg, env) \
      axis2_msg_get_all_params (msg, env)

/** Checks if named parameter is locked. */
#define AXIS2_MSG_IS_PARAM_LOCKED(msg, env, param_name) \
        axis2_msg_is_param_locked(msg, env, param_name)

/** Sets parent operation. */
#define AXIS2_MSG_SET_PARENT(msg, env, service_desc) \
        axis2_msg_set_parent (msg, env, service_desc)

/** Gets parent operation. */
#define AXIS2_MSG_GET_PARENT(msg, env) \
      axis2_msg_get_parent (msg, env)

/** Gets execution flow. */
#define AXIS2_MSG_GET_FLOW(msg, env) \
      axis2_msg_get_flow (msg, env)

/** Sets execution flow. */
#define AXIS2_MSG_SET_FLOW(msg, env, flow) \
      axis2_msg_set_flow (msg, env, flow)

/** Gets message direction. */
#define AXIS2_MSG_GET_DIRECTION(msg, env) \
      axis2_msg_get_direction (msg, env)

/** Sets message direction. */
#define AXIS2_MSG_SET_DIRECTION(msg, env, direction) \
      axis2_msg_set_direction (msg, env, direction)

/** Gets element QName. */
#define AXIS2_MSG_GET_ELEMENT_QNAME(msg, env) \
      axis2_msg_get_element_qname(msg, env)

/** Sets element QName. */
#define AXIS2_MSG_SET_ELEMENT_QNAME(msg, env, element_qname) \
      axis2_msg_set_element_qname(msg, env, element_qname)

/** Gets name. */
#define AXIS2_MSG_GET_NAME(msg, env) \
      axis2_msg_get_name(msg, env)

/** Sets name. */
#define AXIS2_MSG_SET_NAME(msg, env, name) \
      axis2_msg_set_name(msg, env, name)

/** Gets schema element associated with message. */
/*#define AXIS2_MSG_GET_SCHEMA_ELEMENT(msg, env) \
      axis2_msg_get_schema_element(msg, env)*/

/** @} */
#ifdef __cplusplus
}
#endif
#endif  /* AXIS2_MSG_H */
