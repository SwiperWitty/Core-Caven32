#include "steer_app.h"

float x_Angle = 0,y_Angle = 0,z_Angle;

int Steer_app_init (int Set)
{
    int retval = 0;
    Mode_Init.Steering_Engine(ENABLE);
	Mode_Use.Steering_Engine.Set_Angle(1,x_Angle);
	Mode_Use.Steering_Engine.Set_Angle(2,y_Angle);
    return retval;
}

int Steer_app (Caven_App_Type * message)
{
    int retval = 0;
    int run_num = 0;
    int temp_num;
    static int first = 1;
    Caven_Control_Type control;
    char string_temp[50];
    
    if (message != NULL)
    {
        memcpy(&control,message->p_Data,sizeof(control));
        if(first)
        {
            message->str_switch = 1;
            message->cursor = 0;
            message->layer = 2;
            first = 0;
        }
        //
        sprintf(message->string,"Caven Open power");            // 0
        run_num += sizeof("Caven Open power");

        if (message->layer == 1)
        {
            if(control.Control_botton == 1)
            {
                switch (message->cursor)            // 光标
                {
                    case (2):
                        message->layer ++;
                        break;
                    default:
                        message->app_ID = 1;        // 返回home
                        first = 1;
                        break;
                }
            }
            if(control.Control_y > 0)
            {
                if(message->cursor >= (10 - 1))
                {
                    message->cursor = 2;
                }
                else
                {
                    message->cursor ++;
                }
                if(message->cursor > 2)        //!!!! 
                {
                    message->cursor = (10 - 1);
                }
            }
            else if(control.Control_y < 0)
            {
                if(message->cursor <= 2)
                {
                    message->cursor = (10 - 1);
                }
                else if(message->cursor == (10 - 1))
                {
                    message->cursor = 2;
                }
                else
                {
                    message->cursor --;
                }
            }
            sprintf(message->string + run_num," ");             // 1
            run_num += sizeof(" ");
            sprintf(message->string + run_num,"Start Steer control !");  // 2
            run_num += sizeof("Start Steer control !");
            sprintf(message->string + run_num,"\nBack [user/steer]");    // end(x)
        }
        else if (message->layer == 2)
        {
            if(control.Control_botton == 1)
            {
                switch (message->cursor)            // 光标
                {
                    case (2):
                        message->layer --;
                        break;
                    default:
                        break;
                }
            }
            if(control.Control_y > 0)
            {
                y_Angle += 0.3;
                if(control.Control_y > 1)
                {
                    y_Angle += 1.0;
                }
            }
            else if(control.Control_y < 0)
            {
                y_Angle -= 0.3;
                if(control.Control_y < -1)
                {
                    y_Angle -= 1.0;
                }
            }
            if(control.Control_x > 0)
            {
                x_Angle += 0.3;
                if(control.Control_x > 1)
                {
                    x_Angle += 1.0;
                }
            }
            else if(control.Control_x < 0)
            {
                x_Angle -= 0.3;
                if(control.Control_x < -1)
                {
                    x_Angle -= 1.0;
                }
            }
            x_Angle = MAX(x_Angle,-90);
            y_Angle = MAX(y_Angle,-90);
            x_Angle = MIN(x_Angle,90);
            y_Angle = MIN(y_Angle,90);
            
            sprintf(message->string + run_num," ");             // 1
            run_num += sizeof(" ");
            sprintf(string_temp,"Start x: %4.1f ",x_Angle);     // 2
            temp_num = strlen(string_temp);
            memcpy(message->string + run_num,string_temp,temp_num);
            run_num += strlen(string_temp) + 1;
            sprintf(string_temp,"Start y: %4.1f ",y_Angle);     // 3
            temp_num = strlen(string_temp);
            memcpy(message->string + run_num,string_temp,temp_num);
            run_num += strlen(string_temp) + 1;
            sprintf(message->string + run_num,"\nBack [user/steer]");    // end(x)

            Mode_Use.Steering_Engine.Set_Angle(1,x_Angle);
            Mode_Use.Steering_Engine.Set_Angle(2,y_Angle);
        }

        message->cursor = MAX(message->cursor,2);       // 第一个可选是2
    }

    return retval;
}

