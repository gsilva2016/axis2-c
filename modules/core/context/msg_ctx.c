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

#include <axis2_msg_ctx.h>
#include <axis2_conf_ctx.h>
#include <axis2_op.h>
#include <axis2_svc.h>
#include <axis2_svc_grp.h>
#include <axis2_conf.h>
#include <axis2_transport_in_desc.h>
#include <axis2_transport_out_desc.h>
#include <axiom_soap_envelope.h>
#include <axiom_soap_const.h>
#include <axis2_options.h>

struct axis2_msg_ctx
{
    /** base context struct */
    axis2_ctx_t *base;
    /** parent of message context is an op context instance */
    struct axis2_op_ctx *parent;
    /** process fault enabled? */
    axis2_bool_t process_fault;
    /**
     * Addressing Information for Axis 2
     * Following Properties will be kept inside this, these fields will be initially filled by
     * the transport. Then later an addressing handler will make relevant changes to this, if addressing
     * information is present in the SOAP header.
     */
    axis2_msg_info_headers_t *msg_info_headers;
    axis2_bool_t msg_info_headers_deep_copy;

    struct axis2_op_ctx *op_ctx;
    struct axis2_svc_ctx *svc_ctx;
    struct axis2_svc_grp_ctx *svc_grp_ctx;
    struct axis2_conf_ctx *conf_ctx;

    /** op */
    axis2_op_t *op;
    /** service */
    axis2_svc_t *svc;
    /** service group */
    axis2_svc_grp_t *svc_grp;

    axis2_transport_in_desc_t *transport_in_desc;
    axis2_transport_out_desc_t *transport_out_desc;

    /** SOAP envelope */
    axiom_soap_envelope_t *soap_envelope;
    /** SOAP Fault envelope */
    axiom_soap_envelope_t *fault_soap_envelope;
    /** response written? */
    axis2_bool_t response_written;
    /** in fault flow? */
    axis2_bool_t in_fault_flow;
    /** is this server side? */
    axis2_bool_t server_side;
    /** message ID */
    axis2_char_t *message_id;
    /** new thread required? */
    axis2_bool_t new_thread_required;
    /** paused */
    axis2_bool_t paused;
    axis2_bool_t keep_alive;
    /** output written? */
    axis2_bool_t output_written;
    /** service context ID */
    axis2_char_t *svc_ctx_id;
    /** paused phase name */
    axis2_char_t *paused_phase_name;
    /** paused handler name */
    axis2_qname_t *paused_handler_name;
    /** SOAP action */
    axis2_char_t *soap_action;
    /** are we doing MTOM now? */
    axis2_bool_t doing_mtom;
    /** are we doing REST now? */
    axis2_bool_t doing_rest;
    /** Rest through HTTP POST? */
    axis2_bool_t do_rest_through_post;
    /** use SOAP 1.1? */
    axis2_bool_t is_soap_11;
    /** service group context id */
    axis2_char_t *svc_grp_ctx_id;
    /** qname of transport in */
    axis2_qname_t *transport_in_desc_qname;
    /** qname of transport out */
    axis2_qname_t *transport_out_desc_qname;
    /** service group id */
    axis2_char_t *svc_grp_id;
    /** service description qname */
    axis2_qname_t *svc_qname;
    /** op qname */
    axis2_qname_t *op_qname;
    /* To keep track of the direction */
    int flow;
    /** The chain of Handlers/Phases for processing this message */
    axis2_array_list_t *execution_chain;
    /** Index into the execution chain of the currently executing handler */
    int current_handler_index;
    /** Index of the paused  handler */
    int paused_handler_index;
    /** Index of the paused  phase */
    int paused_phase_index;
    /** Index into the current Phase of the currently executing handler (if any)*/
    int current_phase_index;

    /**
     * Finds the service to be invoked. This function is used by dispatchers 
     * to locate the service to be invoked.
     * @param msg_ctx message context
     * @param env pointer to environment struct
     * @return pointer to service to be invoked
     */
    struct axis2_svc *(AXIS2_CALL *
            find_svc)(axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env);
    /**
     * Finds the operation to be invoked in the given service. This function 
     * is used by dispatchers to locate the operation to be invoked.
     * @param msg_ctx message context
     * @param env pointer to environment struct
     * @param svc pointer to service to whom the operation belongs 
     * @return pointer to the operation to be invoked
     */
    struct axis2_op *(AXIS2_CALL *
            find_op)( axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env,
                struct axis2_svc *svc);

};

AXIS2_EXTERN axis2_msg_ctx_t *AXIS2_CALL
axis2_msg_ctx_create(
    const axis2_env_t *env,
    struct axis2_conf_ctx *conf_ctx,
    struct axis2_transport_in_desc *transport_in_desc,
    struct axis2_transport_out_desc *transport_out_desc)
{
    axis2_msg_ctx_t *msg_ctx = NULL;

    AXIS2_ENV_CHECK(env, NULL);

    msg_ctx = AXIS2_MALLOC(env->allocator, sizeof(axis2_msg_ctx_t));
    if (!msg_ctx)
    {
        AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
        return NULL;
    }

    msg_ctx->base = NULL;
    msg_ctx->process_fault = AXIS2_FALSE;
    msg_ctx->msg_info_headers = NULL;
    msg_ctx->op_ctx = NULL;
    msg_ctx->svc_ctx = NULL;
    msg_ctx->svc_grp_ctx = NULL;
    msg_ctx->conf_ctx = NULL;
    msg_ctx->op = NULL;
    msg_ctx->svc = NULL;
    msg_ctx->svc_grp = NULL;
    msg_ctx->transport_in_desc = NULL;
    msg_ctx->transport_out_desc = NULL;
    msg_ctx->soap_envelope = NULL;
    msg_ctx->fault_soap_envelope = NULL;
    msg_ctx->response_written = AXIS2_FALSE;
    msg_ctx->in_fault_flow = AXIS2_FALSE;
    msg_ctx->server_side = AXIS2_FALSE;
    msg_ctx->message_id = NULL;
    msg_ctx->new_thread_required = AXIS2_FALSE;
    msg_ctx->paused = AXIS2_FALSE;
    msg_ctx->keep_alive = AXIS2_FALSE;
    msg_ctx->output_written = AXIS2_FALSE;
    msg_ctx->svc_ctx_id = NULL;
    msg_ctx->paused_phase_name = NULL;
    msg_ctx->paused_handler_name = NULL;
    msg_ctx->soap_action = NULL;
    msg_ctx->doing_mtom = AXIS2_FALSE;
    msg_ctx->doing_rest = AXIS2_FALSE;
    msg_ctx->do_rest_through_post = AXIS2_FALSE;
    msg_ctx->is_soap_11 = AXIS2_FALSE;
    msg_ctx->svc_grp_ctx_id = NULL;
    msg_ctx->transport_in_desc_qname = NULL;
    msg_ctx->transport_out_desc_qname = NULL;
    msg_ctx->svc_grp_id = NULL;
    msg_ctx->svc_qname = NULL;
    msg_ctx->op_qname = NULL;
    msg_ctx->flow = AXIS2_IN_FLOW;
    msg_ctx->execution_chain = NULL;
    msg_ctx->current_handler_index = -1;
    msg_ctx->paused_handler_index = -1;
    msg_ctx->current_phase_index = 0;
    msg_ctx->paused_phase_index = 0;

    msg_ctx->base = axis2_ctx_create(env);
    if (!(msg_ctx->base))
    {
        axis2_msg_ctx_free(msg_ctx, env);
        return NULL;
    }

    if (transport_in_desc)
        msg_ctx->transport_in_desc = transport_in_desc;
    if (transport_out_desc)
        msg_ctx->transport_out_desc = transport_out_desc;
    if (conf_ctx)
        msg_ctx->conf_ctx = conf_ctx;

    if (msg_ctx->transport_in_desc)
        msg_ctx->transport_in_desc_qname =
            (axis2_qname_t *)AXIS2_TRANSPORT_IN_DESC_GET_QNAME(transport_in_desc, env);
    if (msg_ctx->transport_out_desc)
        msg_ctx->transport_out_desc_qname =
            (axis2_qname_t *)AXIS2_TRANSPORT_OUT_DESC_GET_QNAME(transport_out_desc, env);

    msg_ctx->msg_info_headers = axis2_msg_info_headers_create(env, NULL, NULL);
    if (!(msg_ctx->msg_info_headers))
    {
        axis2_msg_ctx_free(msg_ctx, env);
        return NULL;
    }
    msg_ctx->msg_info_headers_deep_copy = AXIS2_TRUE;

    return msg_ctx;
}

/******************************************************************************/
struct axis2_ctx *AXIS2_CALL
            axis2_msg_ctx_get_base(
                const axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->base;
}


struct axis2_op_ctx *AXIS2_CALL
            axis2_msg_ctx_get_parent(
                const axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->parent;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_parent(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    struct axis2_op_ctx *parent)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (parent)
    {
        msg_ctx->parent = parent;
    }

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_free(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->paused || msg_ctx->keep_alive)
        return AXIS2_SUCCESS;
    
    if (msg_ctx->base)
    {
        AXIS2_CTX_FREE(msg_ctx->base, env);
        msg_ctx->base = NULL;
    }

    if (msg_ctx->msg_info_headers && msg_ctx->msg_info_headers_deep_copy)
    {
        AXIS2_MSG_INFO_HEADERS_FREE(msg_ctx->msg_info_headers, env);
        msg_ctx->msg_info_headers = NULL;
    }

    if (msg_ctx->message_id)
    {
        AXIS2_FREE(env->allocator, msg_ctx->message_id);
        msg_ctx->message_id = NULL;
    }

    if (msg_ctx->svc_ctx_id)
    {
        AXIS2_FREE(env->allocator, msg_ctx->svc_ctx_id);
        msg_ctx->svc_ctx_id = NULL;
    }

    if (msg_ctx->paused_phase_name)
    {
        AXIS2_FREE(env->allocator, msg_ctx->paused_phase_name);
        msg_ctx->paused_phase_name = NULL;
    }

    if (msg_ctx->soap_action)
    {
        AXIS2_FREE(env->allocator, msg_ctx->soap_action);
        msg_ctx->soap_action = NULL;
    }

    if (msg_ctx->svc_grp_ctx_id)
    {
        AXIS2_FREE(env->allocator, msg_ctx->svc_grp_ctx_id);
        msg_ctx->svc_grp_ctx_id = NULL;
    }

    if (msg_ctx->soap_envelope)
    {
        AXIOM_SOAP_ENVELOPE_FREE(msg_ctx->soap_envelope, env);
        msg_ctx->soap_envelope = NULL;
    }

    if (msg_ctx->fault_soap_envelope)
    {
        AXIOM_SOAP_ENVELOPE_FREE(msg_ctx->fault_soap_envelope, env);
        msg_ctx->fault_soap_envelope = NULL;
    }

    AXIS2_FREE(env->allocator, msg_ctx);
    msg_ctx = NULL;

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_init(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    struct axis2_conf *conf)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, conf, AXIS2_FAILURE);

    if (msg_ctx->transport_in_desc_qname)
    {
        msg_ctx->transport_in_desc = AXIS2_CONF_GET_TRANSPORT_IN(conf, env,
                msg_ctx->transport_in_desc_qname);
    }

    if (msg_ctx->transport_out_desc_qname)
    {
        msg_ctx->transport_out_desc = AXIS2_CONF_GET_TRANSPORT_OUT(conf,
                env, msg_ctx->transport_out_desc_qname);
    }

    if (msg_ctx->svc_grp_id)
    {
        msg_ctx->svc_grp = AXIS2_CONF_GET_SVC_GRP(conf, env,
                msg_ctx->svc_grp_id);
    }

    if (msg_ctx->svc_qname)
    {
        msg_ctx->svc = AXIS2_CONF_GET_SVC(conf, env,
                AXIS2_QNAME_GET_LOCALPART(msg_ctx->svc_qname, env));
    }

    if (msg_ctx->op_qname)
    {
        if (msg_ctx->svc)
            msg_ctx->op = AXIS2_SVC_GET_OP_WITH_QNAME(msg_ctx->svc,
                    env, msg_ctx->op_qname);
    }

    return AXIS2_SUCCESS;
}

/**
 * @return
 */
axis2_endpoint_ref_t *AXIS2_CALL
axis2_msg_ctx_get_fault_to(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_GET_FAULT_TO(msg_ctx->msg_info_headers, env);
    }

    return NULL;
}


axis2_endpoint_ref_t *AXIS2_CALL
axis2_msg_ctx_get_from(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_GET_FROM(msg_ctx->msg_info_headers, env);
    }

    return NULL;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_in_fault_flow(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->in_fault_flow;
}

axiom_soap_envelope_t *AXIS2_CALL
axis2_msg_ctx_get_soap_envelope(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->soap_envelope;
}

axiom_soap_envelope_t *AXIS2_CALL
axis2_msg_ctx_get_fault_soap_envelope(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->fault_soap_envelope;
}

const axis2_char_t *AXIS2_CALL
axis2_msg_ctx_get_msg_id(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_GET_MESSAGE_ID(
                    msg_ctx->msg_info_headers, env);
    }

    return NULL;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_process_fault(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->process_fault;
}

axis2_relates_to_t *AXIS2_CALL
axis2_msg_ctx_get_relates_to(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_GET_RELATES_TO(msg_ctx->msg_info_headers, env);
    }

    return NULL;
}

axis2_endpoint_ref_t *AXIS2_CALL
axis2_msg_ctx_get_reply_to(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_GET_REPLY_TO(msg_ctx->msg_info_headers, env);
    }

    return NULL;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_response_written(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->response_written;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_server_side(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->server_side;
}

axis2_endpoint_ref_t *AXIS2_CALL
axis2_msg_ctx_get_to(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_GET_TO(msg_ctx->msg_info_headers, env);
    }

    return NULL;
}


axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_fault_to(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_endpoint_ref_t *reference)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_SET_TO(msg_ctx->msg_info_headers,
                env, reference);
    }

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_from(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_endpoint_ref_t *reference)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_SET_FROM(msg_ctx->msg_info_headers,
                env, reference);
    }

    return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_in_fault_flow(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t in_fault_flow)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->in_fault_flow = in_fault_flow;
    return  AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_soap_envelope(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (soap_envelope)
    {
        int soap_v = AXIOM_SOAP12;
        soap_v = AXIOM_SOAP_ENVELOPE_GET_SOAP_VERSION(soap_envelope, env);
        msg_ctx->is_soap_11 = (soap_v == AXIOM_SOAP12) ? AXIS2_FALSE : AXIS2_TRUE;
        msg_ctx->soap_envelope = soap_envelope;
    }
    else
    {
        msg_ctx->soap_envelope = NULL;
    }

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_fault_soap_envelope(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axiom_soap_envelope_t *soap_envelope)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->fault_soap_envelope  = soap_envelope ;
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_message_id(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *message_id)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_SET_MESSAGE_ID(
                    msg_ctx->msg_info_headers, env, message_id);
    }

    return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_process_fault(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t process_fault)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->process_fault  = process_fault;
    return  AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_relates_to(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_relates_to_t *reference)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_SET_RELATES_TO(
                    msg_ctx->msg_info_headers, env, reference);
    }

    return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_reply_to(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_endpoint_ref_t *reference)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_SET_REPLY_TO(
                    msg_ctx->msg_info_headers, env, reference);
    }

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_response_written(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t response_written)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->response_written  = response_written ;
    return  AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_server_side(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t server_side)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->server_side = server_side;
    return  AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_to(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_endpoint_ref_t *reference)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_SET_TO(msg_ctx->msg_info_headers,
                env, reference);
    }

    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_new_thread_required(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->new_thread_required;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_new_thread_required(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t new_thread_required)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->new_thread_required = new_thread_required;
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_wsa_action(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *action_uri)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_SET_ACTION(msg_ctx->msg_info_headers,
                env, action_uri);
    }

    return AXIS2_SUCCESS;
}

const axis2_char_t *AXIS2_CALL
axis2_msg_ctx_get_wsa_action(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_GET_ACTION(msg_ctx->msg_info_headers,
                env);
    }

    return NULL;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_wsa_message_id(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *message_id)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_SET_MESSAGE_ID(
                    msg_ctx->msg_info_headers, env, message_id);
    }

    return AXIS2_SUCCESS;
}

const axis2_char_t *AXIS2_CALL
axis2_msg_ctx_get_wsa_message_id(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    if (msg_ctx->msg_info_headers)
    {
        return AXIS2_MSG_INFO_HEADERS_GET_MESSAGE_ID(
                    msg_ctx->msg_info_headers, env);
    }

    return NULL;

}

axis2_msg_info_headers_t *AXIS2_CALL
axis2_msg_ctx_get_msg_info_headers(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->msg_info_headers;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_paused(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->paused;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_paused(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t paused)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->paused = paused;
    msg_ctx->paused_phase_index = msg_ctx->current_phase_index;
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_keep_alive(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t keep_alive)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->keep_alive = keep_alive;
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_is_keep_alive(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->keep_alive;
}

struct axis2_transport_in_desc *AXIS2_CALL
            axis2_msg_ctx_get_transport_in_desc(
                const axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->transport_in_desc;
}

struct axis2_transport_out_desc *AXIS2_CALL
            axis2_msg_ctx_get_transport_out_desc(
                const axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->transport_out_desc;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_transport_in_desc(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    struct axis2_transport_in_desc *transport_in_desc)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (transport_in_desc)
    {
        msg_ctx->transport_in_desc = transport_in_desc;
        msg_ctx->transport_in_desc_qname =
            (axis2_qname_t *)AXIS2_TRANSPORT_IN_DESC_GET_QNAME(transport_in_desc, env);
    }

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_transport_out_desc(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    struct axis2_transport_out_desc *transport_out_desc)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (transport_out_desc)
    {
        msg_ctx->transport_out_desc = transport_out_desc;
        msg_ctx->transport_out_desc_qname =
            (axis2_qname_t *)AXIS2_TRANSPORT_OUT_DESC_GET_QNAME(transport_out_desc, env);
    }

    return AXIS2_SUCCESS;
}

struct axis2_op_ctx *AXIS2_CALL
            axis2_msg_ctx_get_op_ctx(
                const axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->op_ctx;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_op_ctx(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    struct axis2_op_ctx *op_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (op_ctx)
    {
        msg_ctx->op_ctx = op_ctx;

        if (msg_ctx->svc_ctx)
        {
            if (!(AXIS2_OP_CTX_GET_PARENT(msg_ctx->op_ctx, env)))
            {
                AXIS2_OP_CTX_SET_PARENT(msg_ctx->op_ctx, env,
                        msg_ctx->svc_ctx);
            }
        }
        axis2_msg_ctx_set_parent(msg_ctx, env, op_ctx);
        axis2_msg_ctx_set_op(msg_ctx, env, AXIS2_OP_CTX_GET_OP(op_ctx, env));
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_output_written(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->output_written;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_output_written(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t output_written)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->output_written = output_written;
    return AXIS2_SUCCESS;
}

const axis2_char_t *AXIS2_CALL
axis2_msg_ctx_get_svc_ctx_id(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->svc_ctx_id;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_svc_ctx_id(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *svc_ctx_id)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->svc_ctx_id)
    {
        AXIS2_FREE(env->allocator, msg_ctx->svc_ctx_id);
        msg_ctx->svc_ctx_id = NULL;
    }

    if (svc_ctx_id)
    {
        msg_ctx->svc_ctx_id = AXIS2_STRDUP(svc_ctx_id, env);
        if (!(msg_ctx->svc_ctx_id))
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
    }
    return AXIS2_SUCCESS;
}

struct axis2_conf_ctx *AXIS2_CALL
            axis2_msg_ctx_get_conf_ctx(
                const axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->conf_ctx;
}

struct axis2_svc_ctx *AXIS2_CALL
            axis2_msg_ctx_get_svc_ctx(
                const axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->svc_ctx;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_conf_ctx(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    struct axis2_conf_ctx *conf_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (conf_ctx)
    {
        msg_ctx->conf_ctx = conf_ctx;
    }

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_svc_ctx(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    struct axis2_svc_ctx *svc_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (svc_ctx)
    {
        msg_ctx->svc_ctx = svc_ctx;

        if (msg_ctx->op_ctx)
        {
            if (!AXIS2_OP_CTX_GET_PARENT(msg_ctx->op_ctx, env))
                AXIS2_OP_CTX_SET_PARENT(msg_ctx->op_ctx, env, svc_ctx);
        }
        axis2_msg_ctx_set_svc(msg_ctx, env, AXIS2_SVC_CTX_GET_SVC(svc_ctx, env));
    }

    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_msg_info_headers(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_msg_info_headers_t *msg_info_headers)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_info_headers)
    {
        if (msg_ctx->msg_info_headers && 
            msg_ctx->msg_info_headers_deep_copy)
        {
            AXIS2_MSG_INFO_HEADERS_FREE(msg_ctx->msg_info_headers, env);
            msg_ctx->msg_info_headers = NULL;
        }
        msg_ctx->msg_info_headers = msg_info_headers;
        msg_ctx->msg_info_headers_deep_copy = AXIS2_FALSE;
    }

    return AXIS2_SUCCESS;
}


axis2_param_t *AXIS2_CALL
axis2_msg_ctx_get_parameter(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *key)
{
    axis2_param_t *param = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->op)
    {
        param = AXIS2_OP_GET_PARAM(msg_ctx->op, env, key);
        if (param)
        {
            return param;
        }
    }

    if (msg_ctx->svc)
    {
        param = AXIS2_SVC_GET_PARAM(msg_ctx->svc, env, key);
        if (param)
        {
            return param;
        }
    }

    if (msg_ctx->svc_grp)
    {
        param = AXIS2_SVC_GRP_GET_PARAM(msg_ctx->svc_grp, env, key);
        if (param)
        {
            return param;
        }
    }

    if (msg_ctx->conf_ctx)
    {
        axis2_conf_t *conf = AXIS2_CONF_CTX_GET_CONF(msg_ctx->conf_ctx, env);
        param = AXIS2_CONF_GET_PARAM(conf, env, key);
    }

    return param;
}


axis2_param_t *AXIS2_CALL
axis2_msg_ctx_get_module_parameter(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *key,
    const axis2_char_t *module_name,
    axis2_handler_desc_t *handler_desc)
{
    /** NOTE: This method is not used in anywhere, hence can be removed*/
    /*
    axis2_param_t *param = NULL;
    axis2_module_desc_t *module_desc = NULL;
    axis2_qname_t *qname = NULL;
    axis2_conf_t *conf = NULL;*/

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    /*AXIS2_PARAM_CHECK(env->error, module_name, AXIS2_FAILURE);

    qname = axis2_qname_create(env, module_name, NULL, NULL);

    if (msg_ctx->op)
    {
        module_desc = AXIS2_OP_GET_MODULE_DESC(msg_ctx->op, env, qname);
        AXIS2_QNAME_FREE(qname, env);
        if (module_desc) 
        {
            param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, key);
            if (param)
            {
                return param;
            } 
            else 
            {
                param = AXIS2_OP_GET_PARAM(msg_ctx->op, env, key);
                if (param)
                {
                    return param;
                }
            }
        }
    }

    if (msg_ctx->svc)
    {
        module_desc = AXIS2_SVC_GET_MODULE_DESC(msg_ctx->svc, env, qname);
        AXIS2_QNAME_FREE(qname, env);
        if (module_desc) 
        {
            param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, key);
            if (param)
            {
                return param;
            } 
            else 
            {
                param = AXIS2_SVC_GET_PARAM(msg_ctx->svc, env, key);
                if (param)
                {
                    return param;
                }
            }
        }
    }

    if (msg_ctx->svc_grp)
    {
        module_desc = AXIS2_SVC_GRP_GET_MODULE_DESC(msg_ctx->svc_grp, env, qname);
        AXIS2_QNAME_FREE(qname, env);
        if (module_desc) 
        {
            param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, key);
            if (param)
            {
                return param;
            } 
            else 
            {
                param = AXIS2_SVC_GRP_GET_PARAM(msg_ctx->svc_grp, env, key);
                if (param)
                {
                    return param;
                }
            }
        }
    }

    conf = AXIS2_CONF_CTX_GET_CONF(msg_ctx->conf_ctx, env);
    if (conf)
    {
        module_desc = AXIS2_CONF_GET_MODULE_DESC(conf, env, qname);
        AXIS2_QNAME_FREE(qname, env);
    }    

    if (module_desc) 
    {
        param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, key);
        if (param)
        {
            return param;
        } 
        else 
        {
            param = AXIS2_CONF_GET_PARAM(conf, env, key);
            if (param)
            {
                return param;
            }
        }
    }

    if (conf)
    {
        module_desc = AXIS2_CONF_GET_MODULE(conf, env, qname);
        AXIS2_QNAME_FREE(qname, env);
    }

    if (module_desc) 
    {
        param = AXIS2_MODULE_DESC_GET_PARAM(module_desc, env, key);
        if (param)
        {
            return param;
        }
    }

    param = AXIS2_HANDLER_DESC_GET_PARAM(handler_desc, env, key);

    return param;
    */
    return NULL;
}

axis2_property_t *AXIS2_CALL
axis2_msg_ctx_get_property(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *key,
    const axis2_bool_t persistent)
{
    void *obj = NULL;
    axis2_ctx_t *ctx = NULL;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    /* search in message context */
    obj = AXIS2_CTX_GET_PROPERTY(msg_ctx->base, env, key, persistent);
    if (obj)
    {
        return obj;
    }

    if (msg_ctx->op_ctx)
    {
        ctx = AXIS2_OP_CTX_GET_BASE(msg_ctx->op_ctx, env);
        if (ctx)
        {
            obj = AXIS2_CTX_GET_PROPERTY(ctx, env, key, persistent);
            if (obj)
            {
                return obj;
            }
        }
    }

    if (msg_ctx->svc_ctx)
    {
        ctx = AXIS2_SVC_CTX_GET_BASE(msg_ctx->svc_ctx, env);
        if (ctx)
        {
            obj = AXIS2_CTX_GET_PROPERTY(ctx, env, key, persistent);
            if (obj)
            {
                return obj;
            }
        }
    }

    if (msg_ctx->svc_grp_ctx)
    {
        ctx = AXIS2_SVC_GRP_CTX_GET_BASE(msg_ctx->svc_grp_ctx, env);
        if (ctx)
        {
            obj = AXIS2_CTX_GET_PROPERTY(ctx, env, key, persistent);
            if (obj)
            {
                return obj;
            }
        }
    }

    if (msg_ctx->conf_ctx)
    {
        ctx = AXIS2_CONF_CTX_GET_BASE(msg_ctx->conf_ctx, env);
        if (ctx)
        {
            obj = AXIS2_CTX_GET_PROPERTY(ctx, env, key, persistent);
            if (obj)
            {
                return obj;
            }
        }
    }

    return NULL;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_property(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *key,
    axis2_property_t *value,
    axis2_bool_t persistent)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    return AXIS2_CTX_SET_PROPERTY(msg_ctx->base, env, key, value, persistent);
}

const axis2_qname_t *AXIS2_CALL
axis2_msg_ctx_get_paused_handler_name(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->paused_handler_name;
}

const axis2_char_t *AXIS2_CALL
axis2_msg_ctx_get_paused_phase_name(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->paused_phase_name;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_paused_phase_name(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *paused_phase_name)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->paused_phase_name)
    {
        AXIS2_FREE(env->allocator, msg_ctx->paused_phase_name);
        msg_ctx->paused_phase_name = NULL;
    }

    if (paused_phase_name)
    {
        msg_ctx->paused_phase_name = AXIS2_STRDUP(paused_phase_name, env);
        if (!(msg_ctx->paused_phase_name))
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
    }
    return AXIS2_SUCCESS;
}

const axis2_char_t *AXIS2_CALL
axis2_msg_ctx_get_soap_action(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);

    return msg_ctx->soap_action;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_soap_action(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *soap_action)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->soap_action)
    {
        AXIS2_FREE(env->allocator, msg_ctx->soap_action);
        msg_ctx->soap_action = NULL;
    }

    if (soap_action)
    {
        msg_ctx->soap_action = AXIS2_STRDUP(soap_action, env);
        if (!(msg_ctx->soap_action))
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_doing_mtom(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->doing_mtom;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_doing_mtom(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t doing_mtom)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->doing_mtom = doing_mtom;
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_doing_rest(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->doing_rest;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_doing_rest(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t doing_rest)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->doing_rest = doing_rest;
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_do_rest_through_post(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t do_rest_through_post)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->do_rest_through_post = do_rest_through_post;
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_do_rest_through_post(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->do_rest_through_post;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_get_is_soap_11(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->is_soap_11;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_is_soap_11(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_bool_t is_soap11)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->is_soap_11 = is_soap11;
    return AXIS2_SUCCESS;
}

struct axis2_svc_grp_ctx *AXIS2_CALL
            axis2_msg_ctx_get_svc_grp_ctx(
                const axis2_msg_ctx_t *msg_ctx,
                const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->svc_grp_ctx;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_svc_grp_ctx(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    struct axis2_svc_grp_ctx *svc_grp_ctx)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (svc_grp_ctx)
    {
        msg_ctx->svc_grp_ctx = svc_grp_ctx;
    }

    return AXIS2_SUCCESS;
}

axis2_op_t *AXIS2_CALL
axis2_msg_ctx_get_op(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->op;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_op(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_op_t *op)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (op)
    {
        msg_ctx->op = op;
        msg_ctx->op_qname = (axis2_qname_t *)AXIS2_OP_GET_QNAME(op, env);
    }

    return AXIS2_SUCCESS;
}

axis2_svc_t *AXIS2_CALL
axis2_msg_ctx_get_svc(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->svc;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_svc(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_svc_t *svc)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (svc)
    {
        axis2_svc_grp_t *svc_grp = NULL;
        msg_ctx->svc = svc;
        msg_ctx->svc_qname = (axis2_qname_t *)AXIS2_SVC_GET_QNAME(svc, env);

        svc_grp = AXIS2_SVC_GET_PARENT(svc, env);
        if (svc_grp)
        {
            msg_ctx->svc_grp = svc_grp;
            msg_ctx->svc_grp_id =
                (axis2_char_t *)AXIS2_SVC_GRP_GET_NAME(svc_grp, env);
        }
    }

    return AXIS2_SUCCESS;
}

axis2_svc_grp_t *AXIS2_CALL
axis2_msg_ctx_get_svc_grp(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->svc_grp;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_svc_grp(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    axis2_svc_grp_t *svc_grp)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (svc_grp)
    {
        msg_ctx->svc_grp = svc_grp;
        msg_ctx->svc_grp_id =
            (axis2_char_t *)AXIS2_SVC_GRP_GET_NAME(svc_grp, env);
    }

    return AXIS2_SUCCESS;
}

const axis2_char_t *AXIS2_CALL
axis2_msg_ctx_get_svc_grp_ctx_id(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->svc_grp_ctx_id;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_svc_grp_ctx_id(
    struct axis2_msg_ctx *msg_ctx,
    const axis2_env_t *env,
    const axis2_char_t *svc_grp_ctx_id)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);

    if (msg_ctx->svc_grp_ctx_id)
    {
        AXIS2_FREE(env->allocator, msg_ctx->svc_grp_ctx_id);
        msg_ctx->svc_grp_ctx_id = NULL;
    }

    if (svc_grp_ctx_id)
    {
        msg_ctx->svc_grp_ctx_id = AXIS2_STRDUP(svc_grp_ctx_id, env);
        if (!(msg_ctx->svc_grp_ctx_id))
        {
            AXIS2_ERROR_SET(env->error, AXIS2_ERROR_NO_MEMORY, AXIS2_FAILURE);
            return AXIS2_FAILURE;
        }
    }
    return AXIS2_SUCCESS;
}

axis2_bool_t AXIS2_CALL
axis2_msg_ctx_is_paused(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    return msg_ctx->paused;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_find_svc(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env, 
    void *func)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    msg_ctx->find_svc = func;
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_find_op(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    void *func)
{
    AXIS2_ENV_CHECK(env, AXIS2_FALSE);
    msg_ctx->find_op = func;
    return AXIS2_SUCCESS;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_options(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    axis2_options_t *options)
{
    axis2_property_t *rest_val = NULL;
	axis2_char_t *value;
	const axis2_char_t *soap_action = NULL;;

    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    AXIS2_PARAM_CHECK(env->error, options, AXIS2_FAILURE);

    if (msg_ctx->msg_info_headers && msg_ctx->msg_info_headers_deep_copy)
    {
        AXIS2_MSG_INFO_HEADERS_FREE(
            msg_ctx->msg_info_headers, env);
        msg_ctx->msg_info_headers = NULL;
    }
    msg_ctx->msg_info_headers =
        AXIS2_OPTIONS_GET_MSG_INFO_HEADERS(options, env);
    msg_ctx->msg_info_headers_deep_copy = AXIS2_FALSE;

    AXIS2_CTX_SET_NON_PERSISTANT_MAP(msg_ctx->base, env,
            AXIS2_OPTIONS_GET_PROPERTIES(options, env));
    rest_val = (axis2_property_t *) AXIS2_MSG_CTX_GET_PROPERTY(msg_ctx, env,
            AXIS2_ENABLE_REST, AXIS2_FALSE);
    if (rest_val)
    {
		value = (axis2_char_t *)AXIS2_PROPERTY_GET_VALUE(rest_val, env);
		if (value)
		{
			if (AXIS2_STRCMP(value, AXIS2_VALUE_TRUE) == 0)
				AXIS2_MSG_CTX_SET_DOING_REST(msg_ctx, env, AXIS2_TRUE);
		}
    }

    if (msg_ctx->soap_action)
    {
        AXIS2_FREE(env->allocator, msg_ctx->soap_action);
        msg_ctx->soap_action = NULL;
    }
    
    soap_action = AXIS2_OPTIONS_GET_SOAP_ACTION(options, env);
    if (AXIS2_OPTIONS_GET_SOAP_ACTION(options, env))
    {
        msg_ctx->soap_action = AXIS2_STRDUP(soap_action, env);
    }

    return AXIS2_SUCCESS;
}


axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_flow(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    int flow)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->flow = flow;
    return AXIS2_SUCCESS;
}

int AXIS2_CALL
axis2_msg_ctx_get_flow(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, -1);
    return msg_ctx->flow;
}


axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_execution_chain(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    axis2_array_list_t *execution_chain)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->execution_chain = execution_chain;
    msg_ctx->current_handler_index = -1;
    msg_ctx->paused_handler_index = -1;
    msg_ctx->current_phase_index = 0;
    return AXIS2_SUCCESS;
}

axis2_array_list_t *AXIS2_CALL
axis2_msg_ctx_get_execution_chain(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, NULL);
    return msg_ctx->execution_chain;
}

axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_current_handler_index(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    const int index)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->current_handler_index = index;
    if (msg_ctx->execution_chain)
    {
        axis2_handler_t *handler = (axis2_handler_t *)
                AXIS2_ARRAY_LIST_GET(msg_ctx->execution_chain,
                        env, index);
        if (handler)
        {
            msg_ctx->paused_handler_name =
                (axis2_qname_t *)AXIS2_HANDLER_GET_QNAME(handler, env);
        }
    }
    return AXIS2_SUCCESS;
}

int AXIS2_CALL
axis2_msg_ctx_get_current_handler_index(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->current_handler_index;
}

int AXIS2_CALL
axis2_msg_ctx_get_paused_handler_index(const axis2_msg_ctx_t *msg_ctx,
        const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->paused_handler_index;
}


axis2_status_t AXIS2_CALL
axis2_msg_ctx_set_current_phase_index(
    axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    const int index)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    msg_ctx->current_phase_index = index;
    return AXIS2_SUCCESS;

}

int AXIS2_CALL
axis2_msg_ctx_get_current_phase_index(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->current_phase_index;
}

int AXIS2_CALL
axis2_msg_ctx_get_paused_phase_index(
    const axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_FAILURE);
    return msg_ctx->paused_phase_index;
}

AXIS2_EXTERN axis2_svc_t * AXIS2_CALL
axis2_msg_ctx_find_svc(axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env)
{
    return msg_ctx->find_svc(msg_ctx, env);
}

AXIS2_EXTERN axis2_op_t * AXIS2_CALL
axis2_msg_ctx_find_op(axis2_msg_ctx_t *msg_ctx,
    const axis2_env_t *env,
    axis2_svc_t *svc)
{
    return msg_ctx->find_op(msg_ctx, env, svc);
}

