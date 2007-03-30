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

#include <stdlib.h>
#include <string.h>
#include <axutil_env.h>
#include <axis2_error_default.h>
#include <axis2_log_default.h>
#include <axis2_string.h>

AXIS2_EXTERN axutil_env_t * AXIS2_CALL 
axutil_env_create_all(const axis2_char_t *log_file,
    const axis2_log_levels_t log_level)
{
    axutil_env_t *env = NULL;
    axis2_error_t *error = NULL;
    axis2_log_t *log = NULL;
    axutil_allocator_t *allocator = NULL;
    axis2_thread_pool_t *thread_pool = NULL;

    allocator = axutil_allocator_init(NULL);
    error = axis2_error_create(allocator);

    if (log_file)
        log = axis2_log_create(allocator, NULL, log_file);
    else
        log = axis2_log_create_default(allocator);

    if (!log)
        log = axis2_log_create_default(allocator);

    thread_pool = axis2_thread_pool_init(allocator);
    env = axutil_env_create_with_error_log_thread_pool(allocator, error, log, thread_pool);
    if (env->log)
    {
        env->log->level = log_level;
    }
    axis2_error_init();

    return env;
}

AXIS2_EXTERN void AXIS2_CALL  
axutil_env_free(axutil_env_t *env)
{
    axutil_allocator_t *allocator = NULL;

    if (env && env->allocator)
        allocator = env->allocator;

    if (env && env->log)
    {
        AXIS2_LOG_FREE(env->allocator, env->log);
        env->log = NULL;
    }
    if (env && env->error)
    {
        AXIS2_ERROR_FREE(env->error);
        env->error = NULL;
    }
    if (env && env->thread_pool)
    {
        AXIS2_THREAD_POOL_FREE(env->thread_pool);
        env->thread_pool = NULL;
    }
    if (env && env->allocator)
    {
        AXIS2_FREE(env->allocator, env);
        env = NULL;
    }
    if (allocator)
    {
        AXIS2_FREE(allocator, allocator);
        allocator = NULL;
    }

    return;
}

AXIS2_EXTERN axutil_env_t* AXIS2_CALL
axutil_env_create(axutil_allocator_t *allocator)
{
    axutil_env_t *environment;
    axis2_log_t *log = NULL;

    if (! allocator)
        return NULL;

    environment =
        (axutil_env_t *) AXIS2_MALLOC(allocator, sizeof(axutil_env_t));

    if (! environment)
        return NULL;

    log = axis2_log_create_default(allocator);

    environment->allocator = allocator;

    /* Create default error */
    environment->error = axis2_error_create(allocator);
    if (! environment->error)
        return NULL;
    environment->log = log;
    environment->thread_pool = NULL;
    return environment;

}

AXIS2_EXTERN axutil_env_t* AXIS2_CALL
axutil_env_create_with_error(axutil_allocator_t *allocator, 
    axis2_error_t *error)
{
    return axutil_env_create_with_error_log(allocator, error, NULL);
}

AXIS2_EXTERN axutil_env_t * AXIS2_CALL
axutil_env_create_with_error_log(axutil_allocator_t *allocator, 
    axis2_error_t *error, 
    axis2_log_t *log)
{
    axutil_env_t *environment;
    if (! allocator)
        return NULL;
    if (! error)
        return NULL;

    environment = (axutil_env_t *) AXIS2_MALLOC(allocator, sizeof(axutil_env_t));

    if (! environment)
        return NULL;

    environment->allocator = allocator;
    environment->error = error;


    if (! log)
    {
        environment->log_enabled = AXIS2_FALSE;
    }
    else
    {
        environment->log_enabled = AXIS2_TRUE;
        environment->log = log;
    }

    environment->thread_pool = NULL;
    axis2_error_init();
    return environment;
}

AXIS2_EXTERN axutil_env_t * AXIS2_CALL
axutil_env_create_with_error_log_thread_pool(axutil_allocator_t *allocator, 
    axis2_error_t *error, 
    axis2_log_t *log, 
    axis2_thread_pool_t *pool)
{
    axutil_env_t *environment;
    if (! allocator)
        return NULL;
    if (! error)
        return NULL;
    if (! pool)
        return NULL;

    environment =
        (axutil_env_t *) AXIS2_MALLOC(allocator, sizeof(axutil_env_t));

    if (! environment)
        return NULL;

    environment->allocator = allocator;
    environment->error = error;
    environment->thread_pool = pool;

    if (! log)
        environment->log_enabled = AXIS2_FALSE;
    environment->log_enabled = AXIS2_TRUE;
    environment->log = log;

    return environment;
}

AXIS2_EXTERN  axis2_status_t  AXIS2_CALL
axutil_env_check_status(const axutil_env_t *env)
{
    AXIS2_ENV_CHECK(env, AXIS2_CRITICAL_FAILURE);

    return AXIS2_ERROR_GET_STATUS_CODE(env->error);
}

AXIS2_EXTERN axis2_status_t AXIS2_CALL 
axutil_env_enable_log(axutil_env_t *env, 
    axis2_bool_t enable)
{
    AXIS2_ENV_CHECK(env, AXIS2_CRITICAL_FAILURE);

    env->log_enabled = enable;

    return AXIS2_SUCCESS;
}

AXIS2_EXTERN void AXIS2_CALL  
axutil_env_free_masked(axutil_env_t *env, 
    char mask)
{
    if (mask & 0x1)
    {
        AXIS2_LOG_FREE(env->allocator, env->log);
    }
    if (mask & 0x2)
    {
        AXIS2_ERROR_FREE(env->error);
    }
    if (mask & 0x4)
    {
        AXIS2_THREAD_POOL_FREE(env->thread_pool);
    }
    if (env)
        AXIS2_FREE(env->allocator, env);
    
    return;
}






