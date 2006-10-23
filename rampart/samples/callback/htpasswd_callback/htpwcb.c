#include <stdio.h>
#include <axis2_defines.h>
#include <axis2_error.h>
#include <axis2_env.h>
#include <axis2_utils.h>
#include <rampart_callback.h>
#include <axis2_string.h>
#include <axis2_svc_skeleton.h>
#include <axis2_string.h>


axis2_char_t* AXIS2_CALL
get_ht_password(rampart_callback_t *rcb,
        const axis2_env_t *env, const axis2_char_t *username)
{
    axis2_char_t * password = NULL;
    /*TODO : Hard coded value must be changed. This is the common place to have
     * the htpasswd files. But not always*/
    axis2_char_t *filename = "/usr/local/apache2/passwd/passwords";
    FILE *file = NULL;
    
    file = fopen ( filename, "r" );
    if ( file != NULL )
    {
       axis2_char_t line [ 128 ]; 
       axis2_char_t ch = 0;
       axis2_char_t *res = NULL;
       axis2_char_t *un = NULL;
       axis2_char_t *pw = NULL;

       while ( fgets ( line, sizeof line, file ) != NULL ) 
       {
          res = AXIS2_STRSTR(line, ":");
          ch = res[0];
          res[0] = '\0';
          un = (axis2_char_t *) AXIS2_STRDUP(line, env);
          res[0] = ch;
          if(0 == AXIS2_STRCMP(un, username)){
             pw = (axis2_char_t *) AXIS2_STRDUP(&(res[1]), env);
             password = AXIS2_STRNDUP(pw, AXIS2_STRLEN(pw)-1, env); /*We need to remove the end of line character*/

             break;
          }
       }
       AXIS2_FREE(env->allocator, un);
       AXIS2_FREE(env->allocator, pw);
       fclose ( file );
    }else
    {
       AXIS2_LOG_INFO(env->log, "Cannot load the password file %s in the callback module", filename);
       perror ( filename ); 
    }
    
    return password;
}


/**
 * Following block distinguish the exposed part of the dll.
 */
AXIS2_EXPORT int
axis2_get_instance(rampart_callback_t **inst,
        const axis2_env_t *env)
{
    rampart_callback_t* rcb = NULL;

    rcb = AXIS2_MALLOC(env->allocator,
            sizeof(rampart_callback_t));

    rcb->ops = AXIS2_MALLOC(
                env->allocator, sizeof(rampart_callback_ops_t));

    /*assign function pointers*/

    rcb->ops->callback_password = get_ht_password;

    *inst = rcb;

    if (!(*inst))
    {
        return AXIS2_FAILURE;
    }

    return AXIS2_SUCCESS;
}

AXIS2_EXPORT int
axis2_remove_instance(rampart_callback_t *inst,
        const axis2_env_t *env)
{
    axis2_status_t status = AXIS2_FAILURE;
    if (inst)
    {
        status = AXIS2_SVC_SKELETON_FREE(inst, env);
    }
    return status;
}

